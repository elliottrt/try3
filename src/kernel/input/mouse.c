#include "mouse.h"
#include "util.h"
#include "interrupts/irq.h"

static u8 mouse_cycle = 0;     //unsigned char
static i8 mouse_byte[3];    //signed char
static i8 mouse_x = 0;         //signed char
static i8 mouse_y = 0;         //signed char

// https://forum.osdev.org/viewtopic.php?t=10247

static void mouse_handler(struct Registers *regs) {
	switch(mouse_cycle)
	{
	case 0: {
			mouse_byte[0] = inb(0x60);
		  	mouse_cycle++;
		} break;
	case 1: {
		  	mouse_byte[1] = inb(0x60);
		  	mouse_cycle++;
		} break;
	case 2: {
		  	mouse_byte[2] = inb(0x60);
		  	mouse_x = mouse_byte[1];
		  	mouse_y = mouse_byte[2];
		  	mouse_cycle = 0;
		} break;
	}
}

void mouse_write(u8 val) //unsigned char
{
  //Wait to be able to send a command
  iowait();
  //Tell the mouse we are sending a command
  outb(0x64, 0xD4);
  //Wait for the final part
  iowait();
  //Finally write
  outb(0x60, val);
}

u8 mouse_read()
{
  //Get's response from mouse
  iowait();
  return inb(0x60);
}

void mouse_init(void) {

	u8 status = 0x00;

	iowait();
	outb(0x64, 0xA8);

	iowait();
	outb(0x64, 0x20);

	iowait();
	status = inb(0x60) | 2;

	iowait();
	outb(0x64, 0x60);

	iowait();
	outb(0x60, status);

	mouse_write(0xF6);
	mouse_read();

	mouse_write(0xF4);
  	mouse_read();

  	irq_install(12, mouse_handler);

}
