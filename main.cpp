#include "pico/stdlib.h"
#include <stdio.h>

#include "lib/hal/gpio/include/GpioPinMode.hpp"

int main() {
    stdio_init_all();

    volatile GpioPinMode pinMode;

    while (true) {
        printf("Testing GPIO Pin Mode Status %d...\n", pinMode.isReady(0x00));
        sleep_ms(1000);
    }
}
