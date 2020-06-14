# Documentation

In the thesis, we used a Raspberry Pi 3 and Raspbian Buster for the safety
module. We think that, with some minor adjustments, the Raspberry Pi 4 could be
used as well. On a few places we note some minor adjustments that need to be
made in case the safety module is installed on a Raspberry Pi 4, but we haven't
tested it on a Raspberry Pi 4 fully so we are not sure what the implications
are.

We suggest to make a backup of the raspberry pi before executing any of the
commands given here.

## Operating system

Installation of Raspberry Pi OS Buster (previously Raspbian Buster) can be done
using the basic commands as they are found [on the official raspberry pi website](https://www.raspberrypi.org/documentation/installation/installing-images/).


For Linux, this can be done using the `dd` command:

```sh
dd bs=4M if=/path/to/img  of=/dev/the_sd_card conv=fsync
```

After installation, the software of the pi can be updated without
causing a problem for the installation of the safety module. We will see that
Dezyne uses its own version of software anyway (namely `npm`). And, as far as we know,
RTIMULib is not being updated anymore.


It is suggested to setup `ssh` and disable the desktop GUI using `raspi-config`
to increase performance of the soon to be installed safety module.


### On installing Xenomai

We will now look at the installation of the Xenomai framework which is used to
offer real-time performance to the pi. For this installation a combination of
the [official xenomai website](https://xenomai.org/),
[lemariva](https://lemariva.com/blog/2018/07/raspberry-pi-xenomai-patching-tutorial-for-kernel-4-14-y)
, [simple robot rpi4](http://www.simplerobot.net/2019/12/xenomai-3-for-raspberry-pi-4.html) and
[simple robot rpi3](http://www.simplerobot.net/2018/06/build-realtime-xenomai-3-kernel-for_3.html)
tutorials have been used. We use Xenomai version 3.0.7, but we expect the safety
module to work with other versions of Xenomai as well.


We start on a host PC (this could be done on the raspberry pi, but it would be
very slow) on which we patch the linux kernel for use with xenomai.

Building the Linux kernel on another PC requires a toolchain. The following
command installs a toolchain on Debian like operating systems:

```sh
~$ apt-get install gcc-arm-linux-gnueabihf
```


Start by cloning the linux kernel and `tools`. The `rpi-kernel` directory is
used to build the patched kernel in.

```sh
~$ mkdir rpi-kernel
~$ cd rpi-kernel
~/rpi-kernel$ git clone https://github.com/raspberrypi/linux.git
~/rpi-kernel$ git clone https://github.com/raspberrypi/tools.git --depth 3
```

Jump to an appropriate branch that can be patched for xenomai, like so:

```sh
~/rpi-kernel$ cd linux
~/rpi-kernel$ git reset --hard c078c64fecb325ee86da705b91ed286c90aae3f6
```

We now need to download Xenomai from
[here](https://xenomai.org/downloads/). We used `xenomai-3.0.7.tar.bz2`, but
other versions of xenomai should work just fine.

We also need an ipipe patch. For the kernel version we used, we need a special
modified ipipe.  This can be downloaded
[here](https://github.com/thanhtam-h/rpi4-xeno3/tree/master/scripts) For
convenience, this specific file can be found in this directory of the repository
as well. If problems appear with installation using this ipipe, we suggest to
try using a different kernel branch and version to patch.  We found that a
specific kernel version and branch is needed to be able to execute the ipipe
patch.


Now we can  patch the kernel

```sh
~/rpi-kernel$ xenomai3.0.7/scripts/prepare-kernel.sh --linux=linux --arch=arm --ipipe=ipipe-core-4.19.82-arm-6-mod-4.49.86.patch
```

We will now build the patched kernel.

Set the following variables:

```sh
mkdir -p ~/rpi-kernel/rt-kernel
~/rpi-kernel$ export ARCH=arm
~/rpi-kernel$ export
CROSS_COMPILE=~/rpi-kernel/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-
~/rpi-kernel$ export INSTALL_MOD_PATH=~/rpi-kernel/rt-kernel
~/rpi-kernel$ export INSTALL_DTBS_PATH=~/rpi-kernel/rt-kernel
~/rpi-kernel$ export KERNEL=kernel7 # NOTE, use KERNEL=kernel7l for raspberry pi 4
```

Let us make the kernel configuration. On our kernel branch and version, there is
a bug in the code that generates the following error

```sh
init/Kconfig:2044: can't open file "arch/$SRCARCH/xenomai/Kconfig"
make[1]: *** [scripts/kconfig/Makefile:104: bcm2711_defconfig] Error 1
make: *** [Makefile:534: bcm2711_defconfig] Error 2
```

this, however, can be easily fixed by changing `$SRCARCH` to `arm` in
~/rpi-kernel/linux/init/KConfig line 2044.



```sh
~/rpi-kernel$ cd ~/rpi-kernel/linux/
~/rpi-kernel/linux/$ make bcm2711_defconfig # NOTE, use bcm2709_defconfig for raspberry pi 3
```

Next we need to configure kernel options. Execute the following to make the
kernel config:

```sh
~/rpi-kernel/linux/$ make menuconfig
```

You may need to install libncurses to make menuconfig.

```sh
~/rpi-kernel/linux/$ sudo apt-get install libncurses-dev
```

In the kernel configuration, configure the following options. The location of
these settings varies per kernel version, but one can use the search
functionality to look up certain settings (although we found sometimes just
looking for them one by one gave better results, as the search seemed to not
always show us the results we want).

```
Set to preemptible kernel: General Setup → Preemption model →
Disable CPU Frequency scaling: CPU Power Management → CPU Frequency scaling → CPU Frequency scaling
Disable Allow for memory compaction: Kernel Features → Contiguous Memory Allocator
Disable Contiguous Memory Allocator: Kernel Features → Allow for memory compaction
Disable KGDB: kernel debugger: KGDB: kernel debugger → Kernel Hacking
Enable XDDP support: Xenomai/cobalt → Drivers → Real-time IPC drivers → XDDP
```

without the last you will get an error saying that 'the address family is not
supported' when running the safety module, so make sure to enable that option.


We are now ready to build the kernel:

```sh
~/rpi-kernel/linux$ make -j4 zImage 
~/rpi-kernel/linux$ make -j4 modules 
~/rpi-kernel/linux$ make -j4 dtbs 
~/rpi-kernel/linux$ make -j4 modules_install 
~/rpi-kernel/linux$ make -j4 dtbs_install
```

Lastly execute the following:

```sh
~/rpi-kernel/linux$ mkdir $INSTALL_MOD_PATH/boot
~/rpi-kernel/linux$ ./scripts/mkknlimg ./arch/arm/boot/zImage $INSTALL_MOD_PATH/boot/$KERNEL.img
```


We now need to transfer the kernel to our raspberry pi system:

```sh
~/rpi-kernel/linux$ cd $INSTALL_MOD_PATH
~/rpi-kernel/rt-kernel$ tar czf ../xenomai-kernel.tgz *
~/rpi-kernel/rt-kernel$ cd ..
~/rpi-kernel$ scp xenomai-kernel.tgz pi@<ipaddress>:/tmp
```

where `<ipaddress>` is the ip address of the raspberry pi.


Next, on the raspberry pi, we can install the kernel using the following
commands:

```sh
~$ cd /tmp
/tmp$ tar xzf xenomai-kernel.tgz
/tmp$ sudo cp *.dtb /boot/
/tmp$ cd boot
/tmp/boot$ sudo cp -rd * /boot/
/tmp/boot$ cd ../lib
/tmp/lib$ sudo cp -dr * /lib/
/tmp/lib$ cd ../overlays
/tmp/overlays$ sudo cp -d * /boot/overlays
/tmp/overlays$ cd ..
/tmp$ sudo cp -d bcm* /boot/
```

Add the following to `/boot/config.txt`

```sh
kernel=${zImage name}
device_tree=bcm2710-rpi-3-b.dtb
```

Add the following options to the end of `/boot/cmdline.txt`:

```sh
dwc_otg.fiq_enable=0 dwc_otg.fiq_fsm_enable=0 dwc_otg.nak_holdoff=0
```

Reboot the raspberry pi. If everything went well, the following command will
return the installed xenomai version, in our case version 3.0.7.

```sh
pi@raspberrypi:~ $ dmesg | grep -i xenomai
[    0.232286] [Xenomai] scheduling class idle registered.
[    0.232297] [Xenomai] scheduling class rt registered.
[    0.232558] I-pipe: head domain Xenomai registered.
[    0.237209] [Xenomai] Cobalt v3.0.7 (Lingering Dawn)
```

### On installing Xenomai tools

We also need to install tools to be able to test xenomai. The commands used in
this part are a complete copy of what is given in [this tutorial](https://lemariva.com/blog/2018/07/raspberry-pi-xenomai-patching-tutorial-for-kernel-4-14-y)
under compiling xenomai tools.

Let us compile the tools on the host PC:

```sh
~/rpi-kernel$ cd xenomai-3.0.7
~/rpi-kernel/xenomai-3.0.7$ ./scripts/bootstrap --with-core=cobalt –enable-debug=partial
~/rpi-kernel/xenomai-3.0.7$ ./configure CFLAGS="-march=armv7-a -mfpu=vfp3" LDFLAGS="-mtune=cortex-a53" --build=i686-pc-linux-gnu --host=arm-linux-gnueabihf --with-core=cobalt --enable-smp CC=${CROSS_COMPILE}gcc LD=${CROSS_COMPILE}ld
~/rpi-kernel/xenomai-3.0.7$ make -j4 install DESTDIR=${PWD}/target
```

Transfer to the raspberry pi:

```sh
~/rpi-kernel/xenomai-3.0.7$ cd target
~/rpi-kernel/xenomai-3.0.7/target$ tar czf ../../xenomai-tools.tgz *
~/rpi-kernel/xenomai-3.0.7$ cd ~/rpi-kernel/
~/rpi-kernel$ scp xenomai-tools.tgz pi@<ipaddress>:/tmp
```

Now, going over to the raspbbery pi:

```sh
~$ cd /tmp
/tmp$ tar xzf xenomai-tools.tgz
/tmp$ sudo cp dev/* /dev/
/tmp$ sudo cp usr/* /usr/
```

We can run a testing program like `latency` as follows:

```sh
~$ sudo /usr/xenomai/bin/latency 
```

## Dezyne

To be able to run the safety module, we need to have the dezyne command line
installed. Installation instructions can be found on Verum's website of Dezyne,
at the time of writing
[here](https://dezyne.org/dezyne/manual/dezyne.html#Installation) (but the
website is subject to change, either way the website can be found with a search on 'verum dezyne'). In our installation we used Dezyne version 2.9.0.

## Building and running the safety module

The makefile in `safety-module-for-care-robot-rose/src/dezyne/safety_module/`
can be used to build the safety module. This can be done by copying all files in that directory to the raspberry pi and executing `make`. If all goes well, it generates an executeable called `sm`. The
safety module can then be run with `sudo ./sm`. Sudo is required by Xenomai.

Press `q` followed by enter to stop running.
