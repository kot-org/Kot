<p align="center">
	<img src="./meta/images/laptop.png?raw=true" width="90%"/> 
</p>

# Kot

[![C/C++ CI](https://github.com/kot-org/new-kot/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/kot-org/new-kot/actions/workflows/c-cpp.yml)

[![Discord](https://discordapp.com/api/guilds/901353839557476363/widget.png?style=banner3)](https://discord.gg/XJzhM7uKkz)

# Screenshots

<p align="center">
	<img src="./meta/screenshots/0.png?raw=true" width="90%"/> 
</p>

# Build it yourself
That's easy! Run `make init` to install all the required build tools and then run `PACKAGE=all make build` to start the building process. Right after, `make run` will start **TNK** in Qemu.

# How to Use the Main Makefile

To effectively use the main Makefile in this project, follow these steps:

1. **Initialization:** Before you can build, run, or perform any other actions, you need to initialize the project. Run the following command: `make init`


2. **Build a Specific Package:**

To build a specific package, use the `PACKAGE` variable. For example, if you want to build the kernel, run: `PACKAGE=kernel make build`
This command will also work to rebuild the specified package if necessary.

3. **Build All Packages:**

To build all packages in the project, simply use the following command: `PACKAGE=all make build`
This command will build all packages and also rebuild them if needed.

4. **Configure a Specific Package:**

To configure a specific package, again use the `PACKAGE` variable. For instance, if you want to configure the kernel, run: `PACKAGE=kernel make reconfigure`
This command will also work to reconfigure the specified package if necessary.

5. **Configure All Packages:**

To configure all packages in the project, use the following command: `PACKAGE=all make reconfigure`
This command will configure all packages and also reconfigure them if needed.
By following these instructions, you can effectively use the main Makefile to build and configure packages in your project.

6. **Use USB key:**

**:warning: Kot is alpha software, use it at your own risk.** To use a USB key with **Kot**, please note that this method requires a certain level of knowledge about the Linux system, as improper usage can potentially damage your system's data. To utilize this method, you need to set the `BOOT_DEVICE` variable with the correct USB device name. Here's an example of how to do it: `BOOT_DEVICE=<device> PACKAGE=all make build`
Replace <device> with the actual device name of your USB key, such as /dev/sdc. Be aware that using the USB key in this way will format it, erasing any existing data. Ensure that you have correctly identified the USB device name, as specifying the wrong device can result in data loss. This command will start Kot in Qemu using the specified USB key as the boot device.

7. **Use your PC:**

**:warning: Kot is alpha software, use it at your own risk.** To use boot **Kot** on your PC, please note that this method requires a certain level of knowledge about the BIOS, as improper usage can potentially damage your system's data. First of all you have to run the command `INSTALL=true PACKAGE=all make build`, and then you can reboot your PC. Then enter into your BIOS and select `EFI/limine/BOOTX64.EFI` as a boot file.

