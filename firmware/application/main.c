/*
 * File: main.c
 * Author: Ted Salmon <tass2001@gmail.com>
 * Description:
 *     The main loop for our PIC24FJ
 */
#include <stdlib.h>
#include <stdio.h>
#include <xc.h>
#include "sysconfig.h"
#include "handler.h"
#include "mappings.h"
#include "upgrade.h"
#include "lib/bt.h"
#include "lib/config.h"
#include "lib/eeprom.h"
#include "lib/log.h"
#include "lib/i2c.h"
#include "lib/ibus.h"
#include "lib/pcm51xx.h"
#include "lib/timer.h"
#include "lib/uart.h"
#include "lib/utils.h"
#include "lib/wm88xx.h"
#include "ui/cli.h"

int main(void)
{
    // Set the IVT mode
    IVT_MODE = IVT_MODE_APP;

    // Set all ports to digital mode
    ANSB = 0;
    ANSC = 0;
    ANSD = 0;
    ANSE = 0;
    ANSF = 0;
    ANSG = 0;

    // Set all ports as outputs
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    TRISF = 0;
    TRISG = 0;

    // Drive all ports to logic low
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;
    LATF = 0;
    LATG = 0;

    // Set the I/O ports to outputs
    BT_DATA_SEL_MODE = 0;
    ON_LED_MODE = 0;
    UART_SEL_MODE = 0;
    IBUS_EN_MODE = 0;
    PAM_SHDN_MODE = 0;
    TEL_ON_MODE = 0;
    TEL_MUTE_MODE = 0;

    // Set the RX / input pins to inputs
    IBUS_UART_RX_PIN_MODE = 1;
    IBUS_UART_STATUS_MODE = 1;
    BT_UART_RX_PIN_MODE = 1;
    SYSTEM_UART_RX_PIN_MODE = 1;
    EEPROM_SDI_PIN_MODE = 1;
    SYS_DTR_MODE = 1;
    
    // Set the UART mode to MCU for the remainder of this application code
    UART_SEL = UART_SEL_MCU;
    // Enable the IBus Regulator
    IBUS_EN = 1;
    // Turn the PAM8406 off until it's required
    PAM_SHDN = 0;
    // Keep the vehicle unmuted and telephone on disengaged
    TEL_MUTE = 0;
    TEL_ON = 0;
    // Keep the BC127 out of data mode / Boot the BM83
    BT_DATA_SEL = 0;

    // Pull up the SDA/SCL lines
    IOCPUEbits.IOCPE6 = 1;
    IOCPUEbits.IOCPE7 = 1;

    // Initialize the system UART first, since we needed it for debug
    struct UART_t systemUart = UARTInit(
        SYSTEM_UART_MODULE,
        SYSTEM_UART_RX_RPIN,
        SYSTEM_UART_TX_RPIN,
        SYSTEM_UART_RX_PRIORITY,
        SYSTEM_UART_TX_PRIORITY,
        UART_BAUD_115200,
        UART_PARITY_NONE
    );

    // All UART handler registrations need to be done at
    // this level to maintain a global scope
    UARTAddModuleHandler(&systemUart);
    LogMessage("", "**** BlueBus ****");

    // Initialize low level modules
    EEPROMInit();
    TimerInit();
    I2CInit();

    struct BT_t bt = BTInit();
    UARTAddModuleHandler(&bt.uart);

    struct IBus_t ibus = IBusInit();
    UARTAddModuleHandler(&ibus.uart);

    // WM8804 and PCM5122 must be initialized after the I2C Bus
    WM88XXInit();
    PCM51XXInit();

    ON_LED = 1;

    // Initialize handlers
    HandlerInit(&bt, &ibus);
    // Initialize the CLI
    CLIInit(&systemUart, &bt, &ibus);
    // Run any applicable updates
    UpgradeProcess(&bt, &ibus);
    // Run the PCM51XX Start-up process
    PCM51XXStartup();

    // Process events
    while (1) {
        BTProcess(&bt);
        IBusProcess(&ibus);
        TimerProcessScheduledTasks();
        CLIProcess();
    }

    return 0;
}

// Trap Catches
void TrapWait()
{
    ON_LED = 0;
    // Wait five seconds before resetting
    //uint16_t sleepCount = 0;
    while (1) {
        //TimerDelayMicroseconds(1000);
        //sleepCount++;
    }
    UtilsReset();
}

void __attribute__ ((__interrupt__, auto_psv)) _AltOscillatorFail()
{
    // Clear the trap flag
    INTCON1bits.OSCFAIL = 0;
    ConfigSetTrapIncrement(CONFIG_TRAP_OSC);
    TrapWait();
}

void __attribute__ ((__interrupt__, auto_psv)) _AltAddressError()
{
    // Clear the trap flag
    INTCON1bits.ADDRERR = 0;
    ConfigSetTrapIncrement(CONFIG_TRAP_ADDR);
    TrapWait();
}

void __attribute__ ((__interrupt__, auto_psv)) _AltStackError()
{
    // Clear the trap flag
    INTCON1bits.STKERR = 0;
    ConfigSetTrapIncrement(CONFIG_TRAP_STACK);
    TrapWait();
}

void __attribute__ ((__interrupt__, auto_psv)) _AltMathError()
{
    // Clear the trap flag
    INTCON1bits.MATHERR = 0;
    ConfigSetTrapIncrement(CONFIG_TRAP_MATH);
    TrapWait();
}

void __attribute__ ((__interrupt__, auto_psv)) _AltNVMError()
{
    ConfigSetTrapIncrement(CONFIG_TRAP_NVM);
    TrapWait();
}

void __attribute__ ((__interrupt__, auto_psv)) _AltGeneralError()
{
    ConfigSetTrapIncrement(CONFIG_TRAP_GEN);
    TrapWait();
}
