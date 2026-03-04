// ==============================================================================
// PROJECT MAIN SOURCE FILE
// ------------------------------------------------------------------------------
// Project:      PicoTemplateProject
// Description:  General boilerplate sample code for Pico 2 W (RP2350).
//               Demonstrates USB-Serial initialization and basic loop.
//
// Version History:
// v1.0.0 | 2026-02-25 | Initial professional boilerplate
// ==============================================================================

#include <stdio.h>
#include "pico/stdlib.h"

// 1. MAIN ENTRY POINT
// ------------------------------------------------------------------------------
int main()
{
    // Initialize all standard I/O (USB and UART as configured in CMake)
    stdio_init_all();

    // Use volatile for variables observed during debugging to prevent optimization
    volatile int counter = 0;

    // --- OPTIONAL: USB WAIT ---
    // Uncomment the following lines if you want the program to wait 
    // until a Serial Monitor is actually connected.
    /*
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    */

    // Initial startup message
    printf("System gestartet. Wert: %d\n", counter);

    // 2. MAIN LOOP
    // ------------------------------------------------------------------------------
    while (true) {
        counter++;
        
        // Output current state via USB-Serial
        printf("Counter: %d\n", counter);
        
        // Basic heartbeat delay
        sleep_ms(1000);
    }
}