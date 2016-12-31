

#include <stdbool.h>

// CMSIS
#include    "LPC17xx.h"
#include    "core_cm3.h"

// NXP
#include    "lpc17xx_nvic.h"
#include    "lpc17xx_pinsel.h"
#include    "lpc17xx_gpio.h"


// Local
#include    "sbl_iap.h"
#include    "sbl_config.h"
#include    "uart.h"
#include    "debug.h"
#include    "ios.h"

#include "config_pins.h"

// in msc_usb_start.c
void init_usb_msc_device(void);

//
#define extruder_heater_off()   digital_write(EXTRUDER_0_HEATER_PORT, EXTRUDER_0_HEATER_PIN, LOW);
#define heated_bed_off()        digital_write(HEATED_BED_0_HEATER_PORT, HEATED_BED_0_HEATER_PIN, LOW);
#define extruder_fan_off()      digital_write(EXTRUDER_0_FAN_PORT, EXTRUDER_0_FAN_PIN, LOW);
#define buzzer_off()            digital_write(BUZZER_PORT, BUZZER_PIN, LOW);

#define x_disable()         digital_write(X_ENABLE_PORT, X_ENABLE_PIN, 1)
#define x_step()            digital_write(X_STEP_PORT, X_STEP_PIN, 1)
#define y_disable()         digital_write(Y_ENABLE_PORT, Y_ENABLE_PIN, 1)
#define y_step()            digital_write(Y_STEP_PORT, Y_STEP_PIN, 1)
#define y_unstep()            digital_write(Y_STEP_PORT, Y_STEP_PIN, 0)
#define z_disable()         digital_write(Z_ENABLE_PORT, Z_ENABLE_PIN, 1)
#define z_step()            digital_write(Z_STEP_PORT, Z_STEP_PIN, 1)
#define e_disable()         digital_write(E_ENABLE_PORT, E_ENABLE_PIN, 1)
#define e_step()            digital_write(E_STEP_PORT, E_STEP_PIN, 1)



//
void reboot (void)
{
    NVIC_SystemReset();
}


// from sbl_iap
void configure_boot_button (void)
{
    /* Configure bootloader IO button */
    PINSEL_CFG_Type pin;
    pin.Portnum = BOOT_BUTTON_PORT;
    pin.Pinnum = BOOT_BUTTON_PIN;
    pin.Funcnum = PINSEL_FUNC_0;
    pin.Pinmode = PINSEL_PINMODE_PULLUP;
    pin.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin);

    /* set as input */
    GPIO_SetDir(BOOT_BUTTON_PORT, (1<<BOOT_BUTTON_PIN), 0);
}

bool bootloader_button_pressed(void)
{
    /* Verify if bootloader pin is activated */
    if(GPIO_ReadValue(BOOT_BUTTON_PORT) & (1<<BOOT_BUTTON_PIN))
        return false;
    return true;
}

void set_safe_outputs(void)
{
    /*
     * Disable some pins like the ones for heaters while on bootloader, if not, the heaters would be ON
     */
    /* Extruder 0 Heater pin */
    pin_mode(EXTRUDER_0_HEATER_PORT, EXTRUDER_0_HEATER_PIN, OUTPUT);
    extruder_heater_off();
    /* Heated Bed 0 Heater pin */
    pin_mode(HEATED_BED_0_HEATER_PORT, HEATED_BED_0_HEATER_PIN, OUTPUT);
    heated_bed_off();
    /* Extruder fan pin */
    pin_mode(EXTRUDER_0_FAN_PORT, EXTRUDER_0_FAN_PIN, OUTPUT);
    extruder_fan_off();
    if (BUZZER_PORT != 0xFF)
    {
        /* Buzzer fan pin */
        pin_mode(BUZZER_PORT, BUZZER_PIN, OUTPUT);
        buzzer_off();
    }
    if (STEPPERS_RESET_PORT != 0xFF)
    {
        /* Disable reset for all stepper motors */
        pin_mode(STEPPERS_RESET_PORT, STEPPERS_RESET_PIN, OUTPUT);
        digital_write(STEPPERS_RESET_PORT, STEPPERS_RESET_PIN, 1);
    }
    /* Disable all motors BUT enable step pin, so each LED will be ON */
    pin_mode(X_STEP_PORT, X_STEP_PIN, OUTPUT);
    pin_mode(X_ENABLE_PORT, X_ENABLE_PIN, OUTPUT);
    x_disable();
    x_step();
    
    pin_mode(Y_STEP_PORT, Y_STEP_PIN, OUTPUT);
    pin_mode(Y_ENABLE_PORT, Y_ENABLE_PIN, OUTPUT);
    y_disable();
    y_unstep();
    
    pin_mode(Z_STEP_PORT, Z_STEP_PIN, OUTPUT);
    pin_mode(Z_ENABLE_PORT, Z_ENABLE_PIN, OUTPUT);
    z_disable();
    z_step();
    
    pin_mode(E_STEP_PORT, E_STEP_PIN, OUTPUT);
    pin_mode(E_ENABLE_PORT, E_ENABLE_PIN, OUTPUT);
    e_disable();
    e_step();
}


int main() 
{
    // DeInit NVIC and SCBNVIC
    NVIC_DeInit();
    NVIC_SCBDeInit();

    /* Configure the NVIC Preemption Priority Bits:
     * two (2) bits of preemption priority, six (6) bits of sub-priority.
     * Since the Number of Bits used for Priority Levels is five (5), so the
     * actual bit number of sub-priority is three (3)
     */
    NVIC_SetPriorityGrouping(0x05);

    NVIC_SetVTOR(0x00000000);

    set_safe_outputs();

    configure_boot_button();
    
    #ifdef  UARTDEBUG
        hal_uart_init();
    #endif

    DBG("== bootem V0.1 ==" );
        
    if (bootloader_button_pressed())
    {
        DBG("entering bootloader");
        reboot();
    }
    else 
    {
        if (user_code_present()) 
        {
            DBG("starting user code...");
            execute_user_code();
        }
        else 
        {
            DBG("user code invalid");
            // oops
            while (1);
        }
    }        
}