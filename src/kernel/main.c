#include "input/keyboard.h"
#include "system/system.h"
#include "video/console.h"

void _main(void) {

    system_init();
    keyboard_init();
    cs_init();

    cs_setcolor(CS_LIGHT_GREEN, CS_BLACK);
    cs_clear();
    cs_writestr("Welcome to TRY3!\nThis is indeed my third attempt at writing an OS.\n");

    while(1);
}
