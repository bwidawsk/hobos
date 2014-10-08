# Generic script to debug the kernel. Note that this needs to change if you
# want to debug the bootloader. It also needs to change if you need to debug
# any pre-64b code.
target remote localhost:1234
set architecture i386:x86-64:intel
set variable wait = 0

# Set a hardware breakpoint at the first real thing we do so we get the proper
# 64b switching. (QEMU + GDB can easily get confused)
#hb new_beginning
