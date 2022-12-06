CC=/opt/local/bin/i386-elf-gcc
LD=/opt/local/bin/i386-elf-ld
AS=/usr/local/bin/nasm
EMU=/opt/local/bin/qemu-system-i386

MODE=TEXT

CCFLAGS=-Wno-unused-const-variable -m32 -std=c11 -c -O0 -Wall -Wextra -Wpedantic -Isrc/kernel/
CCFLAGS+=-Wno-builtin-declaration-mismatch -Wno-unused-function -ffreestanding -fno-pie -fno-stack-protector 
CCFLAGS+=-fno-builtin-function -fno-builtin -nostdlib -nostdinc -Wno-unused-parameter -Wno-char-subscripts
CCFLAGS+=-D CONSOLE_$(MODE)_MODE
ASFLAGS=-D CONSOLE_$(MODE)_MODE
LDFLAGS=-nostdlib

OUT=bin/os.iso
BOOTSECT=bin/boot.bin
KERNEL=bin/kernel.bin

KERNEL_C_SRCS=$(wildcard src/kernel/*.c) $(wildcard src/kernel/**/*.c)
KERNEL_C_OBJS=$(KERNEL_C_SRCS:.c=.o)
KERNEL_C_HEADERS=$(wildcard src/kernel/*.h) $(wildcard src/kernel/**/*.h)
KERNEL_S_SRCS=$(wildcard src/kernel/*.S) $(wildcard src/kernel/**/*.S)
KERNEL_S_OBJS=$(KERNEL_S_SRCS:.S=.o)

all: run
	
src/kernel/%.o: src/kernel/%.S
	$(AS) -o $@ $^ $(ASFLAGS) -f elf32
	
src/kernel/%.o: src/kernel/%.c
	$(CC) -o $@ $^ $(CCFLAGS)

$(BOOTSECT): src/boot/boot.S src/boot/disk.S src/boot/print.S
	$(AS) -o $(BOOTSECT) $< $(ASFLAGS) -f bin

$(KERNEL): $(KERNEL_C_OBJS) $(KERNEL_S_OBJS)
	$(LD) -o $(KERNEL) $(KERNEL_S_OBJS) $(KERNEL_C_OBJS) $(LDFLAGS) -Tsrc/kernel/link.ld

$(OUT): $(ZEROES) $(BOOTSECT) $(KERNEL)
	dd if=/dev/zero of=$(OUT) bs=512 count=2880
	dd if=$(BOOTSECT) of=$(OUT) conv=notrunc bs=512 seek=0 count=1
	dd if=$(KERNEL) of=$(OUT) conv=notrunc bs=512 seek=1 count=2048

run: Makefile $(OUT) $(KERNEL_C_HEADERS)
	$(EMU) -drive format=raw,file=$(OUT) -m 128M

dis: $(OUT)
	ndisasm $(OUT) -b 32 > $(OUT).dis.txt

clean:
	rm -f src/kernel/*.o
	rm -f src/kernel/**/*.o
	rm -f src/boot/*.bin
	rm -f bin/*.bin
	rm -f $(OUT)
