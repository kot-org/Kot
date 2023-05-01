# Tools

## **Initrd**

Initrd stands for "Initial RAM Disk." It is a small temporary file system that is loaded into memory when a Linux system boots up, before the main file system is mounted. The initrd contains device driver modules, initialization scripts, and configuration files necessary to bootstrap the system. It is often used to add missing device drivers or to support special file systems during boot.

## **Disk**

Allows creating a virtual disk from the [**Kot file system**](https://github.com/kot-org/Kot).

## **Keyboard**

Allows you to configure your own keyboard with the **Keys.json** file. When built, it outputs a .bin file.

Double Key (dk): 0 = *nothing*, 1 = *shift*, 2 = *altgr*,...

## **Audio**

Allows you to create your own startup sound for Kot with an output file in .bin format.