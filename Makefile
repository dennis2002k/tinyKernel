CFLAGS = -m32 -fno-stack-protector -fno-builtin -Iinclude

all: clean kernel boot image

clean:
	rm -rf *.o

kernel:
	gcc $(CFLAGS) -c src/kernel/kernel.c -o kernel.o
	gcc $(CFLAGS) -c src/drivers/vga.c -o vga.o
	gcc $(CFLAGS) -c src/drivers/io.c -o io.o
	gcc $(CFLAGS) -c src/cpu/gdt.c -o gdt.o
	gcc $(CFLAGS) -c src/drivers/timer.c -o timer.o
	gcc $(CFLAGS) -c src/drivers/keyboard.c -o keyboard.o
	gcc $(CFLAGS) -c src/drivers/memory.c -o memory.o
	gcc $(CFLAGS) -c src/cpu/idt.c -o idt.o

boot:
	nasm -f elf32 src/boot/boot.s -o boot.o
	nasm -f elf32 src/cpu/gdts.s -o gdts.o
	nasm -f elf32 src/cpu/isr.s -o idts.o
image:
	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o io.o gdts.o gdt.o vga.o idt.o idts.o timer.o keyboard.o memory.o 
	mv kernel isodir/boot/kernel
	grub-mkrescue -o kernel.iso isodir/
	rm *.o
