# Intel Galileo Gen 2

This x86 subtarget builds OpenWrt for the Intel Galileo Gen 2 board with
the Intel Quark X1000 SoC. It uses a 32-bit i586 kernel and Lakemont-tuned
userspace, with [QuarkCompat](https://github.com/jsjsjsjsjsjsjson/QuarkCompat)
built into the kernel for instruction emulation and Quark LOCK erratum
handling.

## Hardware support

The target includes drivers for onboard Ethernet, SD storage, PCI UART,
USB host and device controllers, I2C, GPIO and SPI. Drivers needed for the
SD root filesystem, Ethernet and USB ADB are built into the kernel.

The supported board profile is Intel Galileo Gen 2. Compatibility with
the first-generation Galileo is not guaranteed.

## Building

From the OpenWrt source directory, prepare the package feeds:

```sh
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig
```

Select:

- Target System: `x86`
- Subtarget: `Intel Galileo (Quark X1000)`
- Target Profile: `Intel Galileo Gen 2`

Then build using the normal OpenWrt workflow:

```sh
make defconfig
make download -j8
make -j"$(nproc)"
```

When reusing an existing configuration, check the selected board profile,
EFI image options and `Utilities -> adbd`. Explicit settings in an existing
configuration can override the target defaults.

Incremental builds normally do not require cleaning. After changing kernel
patches, use `make target/linux/clean` before rebuilding so the patches are
applied to a fresh kernel source tree.

## Images and booting

Build outputs are placed in `bin/targets/x86/galileo/`. For SD-card EFI boot,
use `openwrt-x86-galileo-galileo-squashfs-combined-efi.img.gz`, or the
corresponding ext4 image if selected. The combined EFI image contains the
partition layout, IA32 EFI bootloader, kernel and root filesystem. A
standalone rootfs image is not a bootable disk image.

Decompress the combined image and write it to the entire SD card, not an
individual partition. Writing the image replaces the card's partition
table and contents; back up any existing data first. Insert the card and
boot it using the board's EFI firmware.

The default Linux serial console is `ttyS1` at 115200 baud, 8 data bits,
no parity and 1 stop bit. GRUB uses the firmware console rather than a
legacy ISA serial port. The default reboot argument is `reboot=efi,warm`.

## USB ADB

The board profile includes an enabled `adbd` service for USB shell access
and file transfer. Power the board from its normal power supply and connect
the MicroUSB Client port to a computer with a data-capable cable. On the
computer, run:

```sh
adb devices -l
adb shell
adb push example.txt /tmp/
adb pull /etc/config/network
```

ADB provides a root shell. TCP ADB and Android framework services, such as
app installation and logcat, are not supported. On Linux hosts, a
`no permissions` message requires a host-side udev rule allowing access to
USB device `18d1:4ee7`.

USB host authentication is disabled by default. To restrict access, copy
each allowed host's public key from `~/.android/adbkey.pub` into
`/etc/adbd/adb_keys`, one key per line, and run on the board:

```sh
uci set adbd.main.auth='1'
uci commit adbd
/etc/init.d/adbd restart
```

There is no authorization dialog; hosts without an installed key are
rejected. To disable ADB:

```sh
uci set adbd.main.enabled='0'
uci commit adbd
/etc/init.d/adbd stop
/etc/init.d/adbd disable
```

## LEDs

The green L user LED is exposed as `galileo:green:user` and configured as
`L (User)` in OpenWrt's LED settings. It is on by default and supports the
standard LED brightness and trigger controls.

The LED shares Arduino D13 with SPI SCLK. The LED driver reserves D13 and
its routing controls, so it cannot simultaneously be used as a shield GPIO
or SPI clock.

The SD activity LED remains under the SD controller's control and is not
exposed as a configurable LED. The `mmc0` activity trigger remains available
for the user LED.

## Compatibility

The package architecture is `i386_lakemont`. Userspace is compiled with
`-march=lakemont -m80387` to use Quark's x87 FPU without requiring MMX or
SSE. Use packages and kernel modules built for this target; kernel modules
from other x86 subtargets have a different kernel ABI.

QuarkCompat does not provide general i686 compatibility or emulate MMX,
SSE and every instruction missing from Quark. Applications with additional
CPU requirements may need target-specific build options.
