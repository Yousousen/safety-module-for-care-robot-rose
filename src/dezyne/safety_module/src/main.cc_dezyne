#include <iostream>
#include <string>
#include <unistd.h>
#include <signal.h>

// Dezyne includes
#include <dzn/runtime.hh>
#include <dzn/locator.hh>
#include "System.hh"

// Framebuffer includes
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <poll.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/fb.h>


// Own
#include "constants.hh"


// Check if device is framebuffer.
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

std::string input;
Behavior_t b = SAFE;
struct fb_t *fb;
ErrorCode_t ret = OK;
int fbfd = 0;

void initialise_framebuffer() {

    // Initialise frame buffer for led matrix.
    std::cout << "Initialising framebuffer...\n";
	fbfd = open_fbdev("RPi-Sense FB");
	if (fbfd <= 0) {
		ret = NOT_OK;
        std::cerr << "Error: cannot open framebuffer device.\n";
        return;
	}
	fb = (struct fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!fb) {
		ret = NOT_OK;
        std::cerr << "Failed to mmap.\n";
        return;
	}
	memset(fb, 0, 128);
    std::cout << "Framebuffer initialised.\n";
}

void destruct_framebuffer() {
	memset(fb, 0, 128);
	munmap(fb, 128);
    close(fbfd);
}

void light_led(struct fb_t* fb, unsigned color) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            fb->pixel[i][j]= color;
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialise locator and runtime
    dzn::locator loc;
    dzn::runtime rt;
    loc.set(rt);

    System s(loc);
    s.dzn_meta.name = "System";

    // Bind native functions
    s.iProgram.out.initialise_framebuffer = initialise_framebuffer;
    s.iProgram.out.destruct_framebuffer = destruct_framebuffer;
    s.iProgram.out.light_led = light_led;

    // Check bindings
    s.check_bindings();


    // initialises framebuffer
    s.iProgram.in.start();
    std::string input;
    while(1) {
        std::cin >> input;
        if (input == "q") {
            break;
        } else if (input == "r") {
            s.iProgram.in.trigger_red(fb);
        } else if (input == "b") {
            s.iProgram.in.trigger_blue(fb);
        }
    }
    // Destructs framebuffer
    s.iProgram.in.stop();
}
