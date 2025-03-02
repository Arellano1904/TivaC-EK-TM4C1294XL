//*****************************************************************************
//
// Proyecto para ejemplificar el uso del modulo UART0 conectado al puerto USB-debug
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

// Declaracion de funciones:
void habilitarUART(uint32_t);
void habilitarBotones(void);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
//
//*****************************************************************************
int
main(void)
{
    // Varaible para almacenar la frecuencia del reloj:
    uint32_t ui32SysClock;
    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    //
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_240), 120000000);

    // Invocamos a la funcion de configuracion:
    habilitarUART(ui32SysClock);
    habilitarBotones();

    //
    // Loop Forever
    //
    while(1)
    {

    }
}

// Definicion de funciones:

void habilitarUART(uint32_t ui32SysClock){
    // Habilitar el reloj para UART0 y el puerto GPIOA (donde est�n los pines UART0)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))

    // Configurar los pines PA0 y PA1 como UART (funci�n alternativa)
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configurar UART0: 115200 baudios, 8 bits de datos, sin paridad, 1 bit de stop
    UARTConfigSetExpClk(UART0_BASE, ui32SysClock, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Habilitar UART0
    UARTEnable(UART0_BASE);
}

// Funci�n que se ejecutar� cuando ocurra la interrupci�n en PJ0 o PJ1
void PortJIntHandler(void) {

    // Detectar qu� pin gener� la interrupci�n
    uint32_t status = GPIOIntStatus(GPIO_PORTJ_BASE, true);

    // Limpiar la bandera de interrupci�n
    GPIOIntClear(GPIO_PORTJ_BASE, status);

    if (status & GPIO_PIN_0) {
        // Acci�n si se presiona el bot�n PJ0
        UARTCharPut(UART0_BASE, '0');
    }

    if (status & GPIO_PIN_1) {
        // Acci�n si se presiona el bot�n PJ1
        UARTCharPut(UART0_BASE, '1');
    }
}

// Enviar una cadena por UART
void UART0_SendString(const char *str) {
    while (*str) {
        UARTCharPut(UART0_BASE, *str++);
    }
}

void habilitarBotones(){
    // Pines de entrada para los switch buttons integrados:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));
    // Pines J0 y J1 como entrada:
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
    // Habilitar resistencia de pull-up en PJ0 y PJ1 (los botones usan l�gica invertida)
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    // Configurar interrupci�n por flanco de bajada (cuando se presiona el bot�n)
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_FALLING_EDGE);
    // Habilitar interrupciones en PJ0 y PJ1
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Registrar y habilitar la interrupci�n en el controlador NVIC
    IntEnable(INT_GPIOJ);
    IntMasterEnable(); // Habilitar interrupciones globales
}

