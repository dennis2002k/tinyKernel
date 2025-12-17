# Tiny x86 Kernel (C + Assembly) 

A hobby operating system kernel written in **C and x86 assembly**, built from scratch and booted using **GRUB (Multiboot)**.  
Runs in **QEMU** and **Bochs**.

This project focuses on low-level OS concepts including memory management, paging, interrupts, and hardware drivers.



## Features

- Multiboot-compliant kernel (GRUB)
- 32-bit x86 protected mode
- Global Descriptor Table (GDT)
- Interrupt Descriptor Table (IDT)
- Programmable Interrupt Controller (PIC) remapping
- Timer (PIT) interrupts
- VGA text-mode driver
- Keyboard-input driver
- Physical Memory Manager (bitmap-based)
- Virtual memory with paging
- Kernel heap (`kmalloc`)



## Memory Management

- Physical memory manager using a bitmap
- 4 KB paging
- Higher-half kernel (0xC0000000+)
- Recursive paging for page table access
- Dynamic kernel heap allocation



### Build and run

## Requirements
- `gcc` with multilib support (for `-m32`)
- `nasm`
- `binutils` (ld)
- `grub-mkrescue` (from GRUB) to build a bootable ISO
- `qemu-system-i386` (optional) to run the ISO
- `bochs` (optional, for additional testing)


On Debian/Ubuntu you can install:

```bash
sudo apt install build-essential gcc-multilib binutils-multiarch grub-common grub-pc-bin qemu-system-x86
```
# Notes
If `grub-mkrescue` fails due to missing xorriso, install `xorriso`:

```bash
sudo apt install xorriso
```



# Build 

```bash
make # this produces the iso
```

# Run Qemu
```bash
qemu-system-i386 -cdrom tiny.iso
```

# Run bochs 
```bash
bochs -f bochs
```


### Notes
- Tested compilation under Linux (WSL or native), but kernel runs in QEMU/Bochs on any platform.
