/**
 * =================================================================================================
 * @file GpioPinMode.hpp
 * -------------------------------------------------------------------------------------------------
 * @author      Ingo Möller
 * @date        05.03.2026
 * -------------------------------------------------------------------------------------------------
 * @brief       Defines the GpioPinMode struct with BitPos and Mode enums for GPIO pin configuration
 *              on RP2350.
 * @details     Defines the operational modes for GPIO pins on RP2350. Encapsulates Z-Mode, Input,
 *              Output, and Schmitt-Trigger. Uses Bit 7 & 6 for hardware-near configuration mapping.
 *
 * ### Features:
 * - **Mapping:** Hardware-near mapping of GPIO modes using bits 7 and 6 for efficient mode detection.
 * - **GPIO State Bits:** Clear definition of GPIO state bits for current state, last state,
 *     event latch, active low, initialization status, and external pin status.
 * - **Resource-Efficient:** Minimal memory footprint with bit-coded status registers.
 * - **Hardware Abstraction:** Encapsulates RP2350-specific GPIO and pull-up configurations.
 * -------------------------------------------------------------------------------------------------
 * ### Version History:
 * @version **v1.0.0** | 2026-03-05 | Initial definition of PinMode enum wrapper
 * =================================================================================================
 */
#pragma once

#include "pico/stdlib.h"
#include <stdio.h>

struct GpioPinMode {
    /**
     * @brief GPIO stateRegister bit positions
     * ---------------------------------------------------------------------------------------------
     * **Bit 0**: Current state of the pin (0 or 1)
     * **Bit 1**: State of the pin in the last cycle
     * **Bit 2**: Latched event for edge detection (Interrupt logic)
     * **Bit 3**: Active low configuration, input is pulled up (1 = inverted logic)
     * **Bit 4**: Initialization status of the hardware
     * **Bit 5**: External pin status (e.g., occupied by I2C/SPI)
     * **Bit 6**: Configuration bit A (hardware-near mapping)
     * **Bit 7**: Configuration bit B (hardware-near mapping)
     * ----------------------------------------------------------------------------------------------
     * @since v1.0.0
     * @author Ingo Moeller
     */
    enum class BitPos : uint8_t {
        CURRENT_STATE = 0,
        LAST_STATE = 1,
        EVENT_LATCH = 2,
        ACTIVE_LOW = 3,
        IS_INITIALIZED = 4,
        IS_EXTERNAL = 5,
        CONFIG_MODE_A = 6,
        CONFIG_MODE_B = 7
    };

    /**
     * @brief GPIO configuration modes (using bits 6 and 7 for hardware-near mapping)
     * ---------------------------------------------------------------------------------------------
     * Z-Mode:          00 (Bits 7 & 6 = 00) - High impedance (initialization standard state)
     * Input:           01 (Bits 7 & 6 = 01) - Standard input mode
     * Output:          10 (Bits 7 & 6 = 10) - Standard output mode
     * Schmitt-Trigger: 11 (Bits 7 & 6 = 11) - Schmitt trigger input mode
     * The actual mode can be determined by reading bits 7 and 6 of the state register.
     * ----------------------------------------------------------------------------------------------
     * @since v1.0.0
     * @author Ingo Moeller
     */
    enum class Mode : uint8_t {
        MODE_INITIAL = 0x00,
        MODE_Z_STATE = ((1 << static_cast<uint8_t>(BitPos::CONFIG_MODE_A)) |
                        (1 << static_cast<uint8_t>(BitPos::CONFIG_MODE_B))),
        MODE_INPUT = (1 << static_cast<uint8_t>(BitPos::CONFIG_MODE_A)),
        MODE_OUTPUT = (1 << static_cast<uint8_t>(BitPos::CONFIG_MODE_B)),
        MODE_SCHMITT = (1 << static_cast<uint8_t>(BitPos::CONFIG_MODE_A) |
                        (1 << static_cast<uint8_t>(BitPos::CONFIG_MODE_B)))
    };

    /**
     * @brief Checks if the GPIO pin is fully initialized and ready for operation
     * ---------------------------------------------------------------------------------------------
     * This method evaluates Bit 4 (IS_INITIALIZED) of the provided state register.
     * If true, the hardware setup (direction, pulls) is completed.
     * ----------------------------------------------------------------------------------------------
     * @param stateRegister The 8-bit status register to evaluate
     * @return true if initialized, false otherwise
     * @since v1.0.0
     * @author Ingo Moeller
     */
    static inline bool isReady(uint8_t stateRegister) {
        return (stateRegister & (1 << static_cast<uint8_t>(GpioPinMode::BitPos::IS_INITIALIZED)));
    }

    /**
     * @brief Checks if the GPIO pin is reserved for external peripheral use
     * ---------------------------------------------------------------------------------------------
     * This method evaluates Bit 5 (IS_EXTERNAL). If set, the pin is occupied by
     * protocols like I2C, SPI, or PWM and should not be toggled manually.
     * ----------------------------------------------------------------------------------------------
     * @param stateRegister The 8-bit status register to evaluate
     * @return true if pin is used externally, false if available for GPIO
     * @since v1.0.0
     * @author Ingo Moeller
     */
    static inline bool isExternal(uint8_t stateRegister) {
        return (stateRegister & (1 << static_cast<uint8_t>(GpioPinMode::BitPos::IS_EXTERNAL)));
    }

    /**
     * @brief Checks if the GPIO pin uses inverted (active low) logic
     * ---------------------------------------------------------------------------------------------
     * Evaluates Bit 3 (ACTIVE_LOW). If true, a physical LOW level is interpreted
     * as logical HIGH (common for buttons with internal Pull-Ups).
     * ----------------------------------------------------------------------------------------------
     * @param stateRegister The 8-bit status register to evaluate
     * @return true if logic is inverted, false for standard logic
     * @since v1.0.0
     * @author Ingo Moeller
     */
    static inline bool isActiveLow(uint8_t stateRegister) {
        return (stateRegister & (1 << static_cast<uint8_t>(GpioPinMode::BitPos::ACTIVE_LOW)));
    }

    /**
     * @brief Extracts the current hardware configuration mode from the register
     * ---------------------------------------------------------------------------------------------
     * Returns the bit-pattern of Bits 7 and 6. The result matches the values
     * defined in the GpioPinMode::Mode enum (Z-State, Input, Output, Schmitt).
     * ----------------------------------------------------------------------------------------------
     * @param stateRegister The 8-bit status register to evaluate
     * @return uint8_t The isolated configuration bits (0x00, 0x40, 0x80, or 0xC0)
     * @since v1.0.0
     * @author Ingo Moeller
     */
    static inline uint8_t getCurrentMode(uint8_t stateRegister) {
        return (stateRegister & ((1 << static_cast<uint8_t>(GpioPinMode::BitPos::CONFIG_MODE_A)) |
                                 (1 << static_cast<uint8_t>(GpioPinMode::BitPos::CONFIG_MODE_B))));
    }
};

/*
Ein kleiner Tipp zur Flankenerkennung (Bits 0, 1, 2)

Mit dieser neuen Anordnung kannst du in deiner tick() Methode eine extrem schnelle Logik bauen:

    Schiebe Bit 0 (Current) in Bit 1 (Last).

    Lies den neuen Wert in Bit 0 ein.

    Flanke passiert, wenn: (Register & CUR) != ((Register & LST) >> 1).

    Wenn das wahr ist, setzt du Bit 2 (EventLatch) auf 1.

Soll ich dir zeigen, wie du mit dieser neuen Bit-Reihenfolge eine isPressed()-Methode schreibst, die ActiveLow
direkt mit einberechnet? (Das wäre dann ein schöner "Einzeiler" für deinen Treiber).
*/