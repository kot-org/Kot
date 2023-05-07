# Embedded Artistry libcpp

This project supplies a C++ standard library and C++ ABI library that can be used for microcontroller-based embedded systems projects.

This project is based on the clang libc++ and libc++abi libraries. Alternative implementations are provided for various files to support embedded systems usage.

The builds are highly configurable, allowing you to create a libc++ and libc++abi set that is tuned specifically to your system's needs.

## Table of Contents

1. [About the Project](#about-the-project)
1. [Project Status](#project-status)
1. [Getting Started](#getting-started)
    1. [Requirements](#Requirements)
    1. [Getting the Source](#getting-the-source)
    1. [Building](#building)
    1. [Usage](#installation)
1. [Configuration Options](#configuration-options)
1. [Versioning](#versioning)
1. [How to Get Help](#how-to-get-help)
1. [Contributing](#contributing)
1. [License](#license)
1. [Authors](#authors)

## About the Project

This project supplies a C++ standard library and C++ ABI library that can be used for microcontroller-based embedded systems projects.

This project is based on the clang libc++ and libc++abi libraries. Alternative implementations are provided for various files to support embedded systems usage.

The builds are highly configurable, allowing you to create a libc++ and libc++abi set that is tuned specifically to your system's needs.

**[Back to top](#table-of-contents)**

## Project Status

This project currently builds libc++ and libc++abi for x86, x86_64, arm, and arm64 processors. All relevant library configuration options have been ported from the CMake builds. See [Configuration Options](#configuration-options) and [meson_options.txt](meson_options.txt) for the list of configurable settings.

This library has also been tested with [Embedded Artistry libc](https://github.com/embeddedartistry/libc) and is used on multiple Embedded Artistry projects.

**[Back to top](#table-of-contents)**

## Getting Started

### Requirements

This project uses [Embedded Artistry's standard Meson build system](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/), and dependencies are described in detail [on our website](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/).

At a minimum you will need:

* [`git-lfs`](https://git-lfs.github.com), which is used to store binary files in this repository
* [Meson](#meson-build-system) is the build system
* Some kind of compiler for your target system.
    - This repository has been tested with:
        - gcc-7, gcc-8, gcc-9
        - arm-none-eabi-gcc
        - Apple clang
        - Mainline clang

#### git-lfs

This project stores some files using [`git-lfs`](https://git-lfs.github.com).

To install `git-lfs` on Linux:

```
sudo apt install git-lfs
```

To install `git-lfs` on OS X:

```
brew install git-lfs
```

Additional installation instructions can be found on the [`git-lfs` website](https://git-lfs.github.com).

#### Meson Build System

The [Meson](https://mesonbuild.com) build system depends on `python3` and `ninja-build`.

To install on Linux:

```
sudo apt-get install python3 python3-pip ninja-build
```

To install on OSX:

```
brew install python3 ninja
```

Meson can be installed through `pip3`:

```
pip3 install meson
```

If you want to install Meson globally on Linux, use:

```
sudo -H pip3 install meson
```

### Getting the Source

This project uses [`git-lfs`](https://git-lfs.github.com), so please install it before cloning. If you cloned prior to installing `git-lfs`, simply run `git lfs pull` after installation.

This project is [hosted on GitHub](https://github.com/embeddedartistry/libcpp). You can clone the project directly using this command:

```
git clone --recursive git@github.com:embeddedartistry/libcpp.git
```

If you don't clone recursively, be sure to run the following command in the repository or your build will fail:

```
git submodule update --init
```

### Building

The library can be built by issuing the following command:

```
make
```

This will build all targets for the host system using the default options (specified in [`meson_options.txt`](meson_options.txt)). Build output will be placed in the `buildresults` folder.

You can clean builds using:

```
make clean
```

You can eliminate the generated `buildresults` folder using:

```
make distclean
```

You can also use the `meson` method for compiling.

You can choose your own build output folder with `meson`, but you must build using `ninja` within the build output folder.

```
$ meson my_build_output/
$ cd my_build_output/
$ ninja
```

At this point, `make` would still work.

You can also use  `meson` directly for compiling.

Create a build output folder:

```
meson buildresults
```

And build all targets by running

```
ninja -C buildresults
```

Cross-compilation is handled using `meson` cross files. Example files are included in the [`build/cross`](build/cross/) folder. You can write your own cross files for your specific processor by defining the toolchain, compilation flags, and linker flags. These settings will be used to compile `libc`. (or open an issue and we can help you).

Cross-compilation must be configured using the meson command when creating the build output folder. For example:

```
meson buildresults  --cross-file build/cross/arm.txt --cross-file build/cross/gcc_arm_cortex-m4.txt
```

Following that, you can run `make` (at the project root) or `ninja` to build the project.

Note that the standard settings may need to be adjusted when cross-compiling. For example, when using gnu-arm-none-eabi, you will likely need to set `enable-threading=false` and `libcxx-enable-chrono=false`.

Also note that if you are cross-compiling for ARM using the arm-none-eabi-gcc toolchain, you will need to use **version 9.0 or later**. If you cannot get this version for your platform due to package availability, you can build the most recent compiler version using the [arm-gcc-bleeding-edge](https://github.com/embeddedartistry/arm-gcc-bleeding-edge) project.

**Full instructions for building the project, using alternate toolchains, and running supporting tooling are documented in [Embedded Artistry's Standardized Meson Build System](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/) on our website.**

### Usage

If you don't use `meson` for your project, the best method to use this project is to build it separately and copy the headers and library contents into your source tree.

* Copy the `include/` directory contents into your source tree.
* Library artifacts are stored in the `buildresults/` folder
* Copy the desired library to your project and add the library to your link step.

Example linker flags:

```
-Lpath/to/libraries -lc++ -lc++abi
```

You can use libcpp as a subproject inside of another `meson` project. Include this project with the `subproject` command:

```
libcpp = subproject('libcpp')
```

Then make dependencies available to your project:

```
libcxx_full_dep = libcpp.get_variable('libcxx_full_dep')
libcxx_full_native_dep = libcpp.get_variable('libcxx_full_native_dep')
libcxx_header_include_dep = libcpp.get_variable('libcxx_header_include_dep')
libcxx_native_header_include_dep = libcpp.get_variable('libcxx_native_header_include_dep')
```

You can use these dependencies elsewhere in your project:

```
fwdemo_sim_platform_dep = declare_dependency(
    include_directories: fwdemo_sim_platform_inc,
    dependencies: [
        fwdemo_simulator_hw_platform_dep,
        fwdemo_platform_dep,
        libmemory_native_dep,
        libc_native_dep,
        libcxx_full_native_dep, # <---- here
    ],
    sources: files('boot.cpp', 'platform.cpp'),
)
```

## Configuration Options

Well, let's be honest: there are way too many options for this project (see [meson_options.txt](meson_options.txt)). But we support a variety of project-specific options as well as the majority of the useful options provided by the libc++ and libc++abi Cmake projects.

Here are the configurable options:

* `enable-werror`: Cause the build to fail if warnings are present
* `enable-pedantic-error`: Turn on `pedantic` warnings and errors
* `force-32-bit`: forces 32-bit compilation instead of 64-bit
* `os-header-path`: Path to the headers for your OS, if using a custom threading solutions
* `disable-rtti`: Build without RTTI support (excludes some C++ features such as name demangling)
* `disable-exceptions`: Build without exception support
* `use-compiler-rt`: Build with compiler-rt support
* `always-enable-assert`: Enable assert even with release builds
* `use-llvm-libunwind`: Tell libc++abi to use the llvm libunwinder (don't change unless you know what you're doing)
* `libcxx-enable-chrono`: Builds with chrono.cpp
* `enable-threading`: Build with threading support
* `libcxx-thread-library`: Select the threading library to use with libc++: none, pthread, or the framework thread shims
* `libcxx-has-external-thread-api`: Tell C++ to look for an __external_threading header with thread function shims
* `libcxx-build-external-thread-api`: ???
* `libcxx-enable-filesystem`: enable filesystem support
* `libcxx-enable-stdinout`: enable stdio support
* `libcxx-default-newdelete`: Enable support for the default new/delete implementations
* `libcxx-silent-terminate`: Enable silent termination. The default terminate handler attempts to demangle uncaught exceptions, which causes extra I/O and demangling code to be pulled in.
* `libcxx-monotonic-clock`: Enable/disable support for the monotonic clock (can only be disabled if threading is disabled)
 `use-libc-subproject`: When true, use the subproject defined in the libc-subproject option. An alternate approach is to override c_stdlib in your cross files.
* `libc-subproject`: This array is used in combination with `use-libc-subproject`. The first entry is the subproject name. The second is the cross-compilation dependency to use. The third value is optional. If used, it is a native dependency to use with native library targets.

Options can be specified using `-D` and the option name:

```
meson buildresults -Denable-werror=true
```

The same style works with `meson configure`:

```
cd buildresults
meson configure -Denable-werror=true
```

### Threading

You can enable threading support with an RTOS using an `__external_threading` header. Supply the include path to your RTOS headers:

```
meson buildresults --cross-file build/cross/gcc/arm/gcc_arm_cortex-m4.txt -Dlibcxx-thread-library=threadx -Dos-header-path=../../os/threadx/include
```

### Blocking new/delete

You can block the `new` and `delete` operators by setting the `libcxx-default-newdelete` to `false`:

```
meson buildresults -Dlibcxx-default-newdelete=false
```

You can also use `meson configure`:

```
cd buildresults
meson configure -Dlibcxx-default-newdelete=false
```

If you are using libcpp as a subproject, you can specify this setting in the containing project options.

### Using a Custom Libc

This project is designed to be used along with a `libc` implementation. If you are using this library, you may not be using the standard `libc` that ships with you compiler. This library needs to know about the particular `libc` implementation during its build, in case there are important differences in definitions.

There are two ways to tell this library about a `libc`:

1. [Override `c_stdlib` in a cross-file](https://mesonbuild.com/Cross-compilation.html#using-a-custom-standard-library), which will be automatically used when building this library.
2. Set `use-libc-subproject` to `true`
    1. By default, this will use the [Embedded Artistry libc](https://github.com/embeddedartistry/libc)
    2. You can specify another Meson subproject by configuring `libc-subproject`. This is an array: the first value is the subproject name, the second the libc dependency variable, and the third is an optional native dependency that will be used with native library variants.

NOTE: External libc dependencies are only used for building the library. They are not forwarded through dependencies. You are expected to handle that with the rest of your program.

## Versioning

This project itself is unversioned and simply pulls in the latest libc++ and libc++abi commits periodically.

## Need help?

If you need further assistance or have any questions, please [file a GitHub Issue](https://github.com/embeddedartistry/libmemory/issues/new) or send us an email using the [Embedded Artistry Contact Form](http://embeddedartistry.com/contact).

You can also reach out on Twitter: [\@mbeddedartistry](https://twitter.com/mbeddedartistry/).

## Contributing

If you are interested in contributing to this project, please read our [contributing guidelines](docs/CONTRIBUTING.md).

## License

This container project is licensed under the MIT license.

libc++ and libc++abi (and the llvm project in general) are released under [a modified Apache 2.0 license](libcpp/LICENSE.txt). Source files which have been modified are licensed under those terms.

## Authors

* **[Phillip Johnston](https://github.com/phillipjohnston)** - *Initial work* - [Embedded Artistry](https://github.com/embeddedartistry)

**[Back to top](#table-of-contents)**
