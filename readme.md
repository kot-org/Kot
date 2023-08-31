<p align="center">
	<img src="./meta/images/laptop.png?raw=true" width="90%"/> 
</p>

# The New Kot
A 64-bit x86 operating system written in C/C++

[![C/C++ CI](https://github.com/kot-org/new-kot/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/kot-org/new-kot/actions/workflows/c-cpp.yml)

<img src="https://discordapp.com/api/guilds/901353839557476363/widget.png?style=banner3" alt="Discord Banner 3"/>

# Screenshots

<p align="center">
	<img src="./meta/screenshots/0.png?raw=true" width="90%"/> 
</p>

# Build it yourself
That's easy! Run `make init` to install all the required build tools and then run `make build` to start the building process. Right after, `make run` will start **TNK** in Qemu.

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



