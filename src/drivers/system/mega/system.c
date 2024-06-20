#include "system.h"

#include "board.h"
#include "interrupt.h"
#include "sysclk.h"

void system_init() {
    irq_initialize_vectors();
    sysclk_init();
    board_init();
}
