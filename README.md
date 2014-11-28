# HOBOs

Yet another hobby operating system with no ultimate goals. This hobby OS is a
64b x86 based OS which may, or may not, run on real hardware. It *does* run on
bochs and QEMU. Included in the repo is a minimal bootloader, as well as a few
small applications that help to either test, or create the OS.

# Installation

The [wiki on gitorious](https://gitorious.org/hobos/pages/Home) contains all the
historical documentation. Please see that for details. Meanwhile, the high level
steps are:

* Clone this repo
* Install dependencies
  * QEMU (bochs should also work, not tested for a long time)
  * seabios
  * build tools
      * bash, GNU Make, GCC, binutils, clang (for userspace, can use gcc instead)
* Optional dependencies
  * urxvt (any terminal than can be invoked with a command will work)
  * gdb

# Running

Assuming you don't need to change anything for your local environment (mostly
urxvt, and where seabios is located), you can just type make.

`foo@bar$ make`

The command will build the relevant components, start QEMU, provide a terminal
which acts as the serial port, and provide a terminal for the QEMU monitor. It
does this by spawning 3 terminals in addition to the one which invoked make.
