/*
 * Copyright (C) 2009 Philippe Gerum <rpm@xenomai.org>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 *
 * XDDP-based RT/NRT threads communication demo.
 *
 * Real-time Xenomai threads and regular Linux threads may want to
 * exchange data in a way that does not require the former to leave
 * the real-time domain (i.e. secondary mode). Message pipes - as
 * implemented by the RTDM-based XDDP protocol - are provided for this
 * purpose.
 *
 * On the Linux domain side, pseudo-device files named /dev/rtp<minor>
 * give regular POSIX threads access to non real-time communication
 * endpoints, via the standard character-based I/O interface. On the
 * Xenomai domain side, sockets may be bound to XDDP ports, which act
 * as proxies to send and receive data to/from the associated
 * pseudo-device files. Ports and pseudo-device minor numbers are
 * paired, meaning that e.g. port 7 will proxy the traffic for
 * /dev/rtp7. Therefore, port numbers may range from 0 to
 * CONFIG_XENO_OPT_PIPE_NRDEV - 1.
 *
 * All data sent through a bound/connected XDDP socket via sendto(2) or
 * write(2) will be passed to the peer endpoint in the Linux domain,
 * and made available for reading via the standard read(2) system
 * call. Conversely, all data sent using write(2) through the non
 * real-time endpoint will be conveyed to the real-time socket
 * endpoint, and made available to the recvfrom(2) or read(2) system
 * calls.
 *
 * Both threads can use the bi-directional data path to send and
 * receive datagrams in a FIFO manner, as illustrated by the simple
 * echoing process implemented by this program.
 *
 * realtime_thread------------------------------>-------+
 *   =>  get socket                                     |
 *   =>  bind socket to port 0                          v
 *   =>  write traffic to NRT domain via sendto()       |
 *   =>  read traffic from NRT domain via recvfrom() <--|--+
 *                                                      |  |
 * regular_thread---------------------------------------+  |
 *   =>  open /dev/rtp0                                 |  ^
 *   =>  read traffic from RT domain via read()         |  |
 *   =>  echo traffic back to RT domain via write()     +--+
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <rtdm/ipc.h>

#include <stdint.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <poll.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

/*** CONSTANTS_H ***/

// real-time operation
#define REALTIME 0

// Read boundary values from CSV file.
#define CSV_HAS_POSITION 0
#define CSV_HAS_VELOCITY 0
#define CSV_HAS_ACCELERATION 0
#define CSV_HAS_ANGULAR_VELOCITY 0
#define CSV_HAS_ANGULAR_ACCELERATION 0

// Interial mass of the care robot with safety module attached (kg).
#define INERTIAL_MASS 100

// Maximum allowed kinetic energy
#define MAX_KE 150
#define MAX_RE 250
#define MAX_STR 50
#define MAX_STR_PAYLOAD 80

// For sampling and calculus
// Δt (ms)
/* #define CHANGE_IN_TIME_MS 1 */
// Δt (μs)
#define CHANGE_IN_TIME_MICRO 100
// Δt (s)
/* #define CHANGE_IN_TIME (CHANGE_IN_TIME_MS * 1E-3) */
#define CHANGE_IN_TIME (CHANGE_IN_TIME_MICRO * 1E-6)


// Macros to control framebuffer device for led matrix.
#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"
#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

// Framebuffer
struct fb_t {
	uint16_t pixel[8][8];
};

enum LedColor_t {BLUE = 0x0006, GREEN = 0x0300, RED = 0x3000};

enum ErrorCode_t { OK = 0x00, NOT_OK = 0x01 };
/* enum Behavior_t {UNSAFE = false, SAFE = true}; */ 


/*** END CONSTANTS_H ***/

// arguments for threads
struct threadargs {
    // framebuffer mutex pointer.
    pthread_mutex_t* mutex_fb;
};


struct fb_t *fb;

pthread_t rt, nrt;
#define XDDP_PORT 0     /* [0..CONFIG-XENO_OPT_PIPE_NRDEV - 1] */

static int is_event_device(const struct dirent *dir)
{
    return strncmp(EVENT_DEV_NAME, dir->d_name,
            strlen(EVENT_DEV_NAME)-1) == 0;
}

static int open_evdev(const char *dev_name)
{
    struct dirent **namelist;
    int i, ndev;
    int fd = -1;

    ndev = scandir(DEV_INPUT_EVENT, &namelist, is_event_device, versionsort);
    if (ndev <= 0)
        return ndev;

    for (i = 0; i < ndev; i++)
    {
        char fname[64];
        char name[256];

        snprintf(fname, sizeof(fname),
                "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if (fd < 0)
            continue;
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        if (strcmp(dev_name, name) == 0)
            break;
        close(fd);
    }

    for (i = 0; i < ndev; i++)
        free(namelist[i]);

    return fd;
}

static int is_framebuffer_device(const struct dirent *dir)
{
    return strncmp(FB_DEV_NAME, dir->d_name,
            strlen(FB_DEV_NAME)-1) == 0;
}


// Open framebuffer device.
static int open_fbdev(const char *dev_name) {
    struct dirent **namelist;
    int i, ndev;
    int fd = -1;
    struct fb_fix_screeninfo fix_info;

    ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
    if (ndev <= 0)
        return ndev;

    for (i = 0; i < ndev; i++) {
        char fname[64];
        char name[256];

        snprintf(fname, sizeof(fname),
                "%s/%s", DEV_FB, namelist[i]->d_name);
        fd = open(fname, O_RDWR);
        if (fd < 0)
            continue;
        ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
        if (strcmp(dev_name, fix_info.id) == 0)
            break;
        close(fd);
        fd = -1;
    }
    for (i = 0; i < ndev; i++)
        free(namelist[i]);

    return fd;
}

void render() {
    int x = rand() % 8;
    int y = rand() % 8;
    int z = rand() % 8;
    int w = rand() % 8;
    int u = rand() % 8;
    int v = rand() % 8;
    memset(fb, 0, 128);
    fb->pixel[x][y]=0xF800;
    fb->pixel[z][w] = 0x7E0;
    fb->pixel[u][v]=0xFFFF;
}

void color_all(unsigned color) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            fb->pixel[i][j]= color;
        }
    }
}


static const char* color_blue = "0x0006";
static const char* color_red = "0x3000";

static void fail(const char *reason)
{
    perror(reason);
    exit(EXIT_FAILURE);
}



static void* realtime_thread(void *arg)
{
    struct sockaddr_ipc saddr;
    int ret, s, n = 0, len;
    struct timespec ts;
    size_t poolsz;
    char buf[128];
    static bool toggle = true;
    /*
     * Get a datagram socket to bind to the RT endpoint. Each
     * endpoint is represented by a port number within the XDDP
     * protocol namespace.
     */
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    /*
     * Set a local 16k pool for the RT endpoint. Memory needed to
     * convey datagrams will be pulled from this pool, instead of
     * Xenomai's system pool.
     */
    poolsz = 16384; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ,
            &poolsz, sizeof(poolsz));
    if (ret)
        fail("setsockopt");
    /*
     * Bind the socket to the port, to setup a proxy to channel
     * traffic to/from the Linux domain.
     *
     * saddr.sipc_port specifies the port number to use.
     */
    memset(&saddr, 0, sizeof(saddr));
    saddr.sipc_family = AF_RTIPC;
    saddr.sipc_port = XDDP_PORT;
    ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
        fail("bind");
    for (;;) {
        /*
         * Send a datagram to the NRT endpoint via the proxy.
         * We may pass a NULL destination address, since a
         * bound socket is assigned a default destination
         * address matching the binding address (unless
         * connect(2) was issued before bind(2), in which case
         * the former would prevail).
         */
        char* msg;
        // Toggle between sending blue and sending red.
        if (toggle) {
            strcpy(msg, color_blue);
            toggle = false;
        } else {
            strcpy(msg, color_red);
            toggle = true;
        }

        len = strlen(msg);
        ret = sendto(s, msg, len, 0, NULL, 0);
        if (ret != len)
            fail("sendto");
        printf("%s: sent %d bytes, \"%.*s\"\n",
                __FUNCTION__, ret, ret, msg);
        /* Read back packets echoed by the regular thread */
        ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
        if (ret <= 0)
            fail("recvfrom");
        printf("   => \"%.*s\" echoed by peer\n", ret, buf);
        n = (n + 1) % (sizeof(msg) / sizeof(msg[0]));
        /*
         * We run in full real-time mode (i.e. primary mode),
         * so we have to let the system breathe between two
         * iterations.
         */
        ts.tv_sec = 0;
        ts.tv_nsec = 500000000; /* 500 ms */
        clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
    }
    return NULL;
}

static void* regular_thread(void *arg)
{
    struct threadargs *args = (struct threadargs *)arg;
    // Pointer to mutex is passed as an argument.
    pthread_mutex_t* mutex_fb = args->mutex_fb;

    char buf[128], *devname;
    int fd, ret;
    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT) < 0)
        fail("asprintf");
    fd = open(devname, O_RDWR);
    free(devname);
    if (fd < 0)
        fail("open");
    for (;;) {
        /* Get the next message from realtime_thread. */
        /* read what to color the led buffer in */
        ret = read(fd, buf, sizeof(buf));
        if (ret <= 0)
            fail("read");

        // Convert hex string to int.
        int color = (int)strtol(buf, NULL, 16);

        // Lock the frame buffer while setting color.
        if (0 != (errno = pthread_mutex_lock(mutex_fb))) {
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }
        // Color the matrix.
        color_all(color);
        // Unlock the frame buffer.
        if (0 != (errno = pthread_mutex_unlock(mutex_fb))) {
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }

        /* Echo the message back to realtime_thread. */
        ret = write(fd, buf, ret);
        if (ret <= 0)
            fail("write");
    }
    return NULL;
}

int main(int argc, char **argv)
{
    // Initialise framebuffer
    int ret = 0;
    int fbfd = 0;
    srand (time(NULL));

    // Open framebuffer device
	fbfd = open_fbdev("RPi-Sense FB");
	if (fbfd <= 0) {
		ret = fbfd;
		printf("Error: cannot open framebuffer device.\n");
        exit(EXIT_FAILURE);
	}
	fb = (struct fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!fb) {
		ret = EXIT_FAILURE;
		printf("Failed to mmap.\n");
        exit(EXIT_FAILURE);
	}
	memset(fb, 0, 128);


    struct sched_param rtparam = { .sched_priority = 42 };
    pthread_attr_t rtattr, regattr;
    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGHUP);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_attr_init(&rtattr);
    pthread_attr_setdetachstate(&rtattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&rtattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&rtattr, SCHED_FIFO);
    pthread_attr_setschedparam(&rtattr, &rtparam);

    errno = pthread_create(&rt, &rtattr, &realtime_thread, NULL);
    if (errno)
        fail("pthread_create");

    pthread_attr_init(&regattr);
    pthread_attr_setdetachstate(&regattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&regattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&regattr, SCHED_OTHER);

    // arguments for threads
    struct threadargs threadargs;

    // Initialise mutex.
    // Create a mutex with the default parameters
    pthread_mutex_t mutex_fb;
    if (0 != (errno = pthread_mutex_init(&mutex_fb, NULL)))
    {
        perror("pthread_mutex_init() failed");
        return EXIT_FAILURE;
    }
    threadargs.mutex_fb = &mutex_fb;

    errno = pthread_create(&nrt, &regattr, &regular_thread, (void*) &threadargs);
    if (errno)
        fail("pthread_create");

    sigwait(&set, &sig);
    pthread_cancel(rt);
    pthread_cancel(nrt);
    pthread_join(rt, NULL);
    pthread_join(nrt, NULL);

    // Destroy mutex
    if (0 != (errno = pthread_mutex_destroy(&mutex_fb)))
    {
        perror("pthread_mutex_destroy() failed");
        return EXIT_FAILURE;
    }
    return 0;
}
