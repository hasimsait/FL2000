# Fresco Logic FL2000 Linux/Android kernel driver

### 1. What is this?

This is an official driver release from Fresco Logic in an attempt to help the open-source community adopting the development and use of the FL2000DX device.
This driver only covers the USB part of the display logic. It does not support the Linux desktop logic (eg. extended desktop vs mirrored desktop).

### 2. On which kernel versions does this driver work?

This driver is tested on Manjaro kernel 5.14.10. All credit belongs to the aur user hcar (https://aur.archlinux.org/packages/fl2000-dkms/), all I did to the driver is applying their patch.

### 3. Target audience

This is a bad idea but I couldn't wait for fl2000drm. I am the target audience. You will need to edit screenshot/main.c if you want it to work.

### 4. How do I enable extended desktop/mirrored desktop on my X Window?

It mirrors the left monitor, you can probably make it extend instead (you'd need a 'virtual' screen).

### 5. FL2000DX limitation.

The FL2000DX chip is cheap by design where it doesn't have a frame buffer on its own.
It relies heavily on USB 3.0 transfer speed to accommodate continuous USB flow.
The larger the image is, the heavier it depends on USB bandwidth.
A typical 1920x1080@60 Hz requires 1920 * 1080 * 24bpp * 60 = 373,248,000 bytes/sec of traffic over the USB bus.
As such, USB2.0 speed is not supported.

Connecting more than one FL2000DX device to the same bus is deprecated.

### 6. How do I compile & test the kernel driver?
#### 6a. Compile the driver

At src, run `make` to create `fl2000.ko` and run `insmod fl2000.ko` to load the driver.

#### 6b. Test the driver

In the `sample` folder, run `make` to create `fltest`. If you you are using a
cross compiler to build the binary for specific platforms, you need to specify that specific
compiler in `src/Makefile`.
    
Run `./fltest 0` as superuser to run the test. The driver provides several
user mode buffer access methods (e.g  copy to kernel internal buffer, or
directly locking down user buffer). Look at `fl2000_ioctl.h` for detailed
information.

In the `screenshot` folder, run `gcc main.c -pthread -lX11 -lXext -m64 -mavx2 -o fltest -O3 && sudo ./fltest 1` to start mirroring the left screen to the screen attached to FL2000. It will not work, you'll need to fix stuff.

### 7. How do I file a bug to the Fresco Logic developers?

You can file bugs to [Github Issues](https://github.com/fresco-fl2000/fl2000/issues)



