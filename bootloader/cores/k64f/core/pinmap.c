/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fsl_clock_manager.h"
#include "fsl_port_hal.h"

#include "mbed_assert.h"
//#include "mbed_error.h"

#include "pinmap.h"

static uint32_t port_addrs[] = PORT_BASE_ADDRS;

void pin_function(PinName pin, int function) {
    MBED_ASSERT(pin != (PinName)NC);

    uint32_t instance = GPIO_PORT(pin);
    uint32_t pin_num = GPIO_PIN_NUM(pin);

    CLOCK_SYS_EnablePortClock(instance);
    PORT_HAL_SetMuxMode(port_addrs[instance], pin_num, (port_mux_t)function);
}

void pin_mode(PinName pin, PinMode mode) {
    MBED_ASSERT(pin != (PinName)NC);
    uint32_t instance = GPIO_PORT(pin);
    uint32_t pin_num = GPIO_PIN_NUM(pin);

    switch (mode) {
        case PullNone:
            PORT_HAL_SetPullCmd(port_addrs[instance], pin_num, false);
            PORT_HAL_SetPullMode(port_addrs[instance], pin_num, kPortPullDown);
            break;
        case PullDown:
            PORT_HAL_SetPullCmd(port_addrs[instance], pin_num, true);
            PORT_HAL_SetPullMode(port_addrs[instance], pin_num, kPortPullDown);
            break;
        case PullUp:
            PORT_HAL_SetPullCmd(port_addrs[instance], pin_num, true);
            PORT_HAL_SetPullMode(port_addrs[instance], pin_num, kPortPullUp);
            break;
        default:
            break;
    }
}
