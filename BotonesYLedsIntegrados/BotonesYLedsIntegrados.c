//*****************************************************************************
//
// Proyecto para ejemplificar el uso de los botones y leds integrados en la tarjeta de desarrollo.
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
#include "driverlib/pin_map.h"

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


// ************** Declaracion de funciones ************************************
void habilitarLeds(void);
void habilitarBotones(void);
void PortJIntHandler(void);


//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
//
//*****************************************************************************
int main(void)
{   //
    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    //
    uint32_t ui32SysClock;
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_240), 120000000);
    // Invocamos a las funciones de configuración:
    habilitarLeds();
    habilitarBotones();
    //
    // Loop Forever
    //
    while(1)
    {

    }
}

//***************** Definicion de funciones ***********************************
void habilitarLeds(void){
    // Pines de salida para leds integrados:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    // Pines PN0, PN1, PF0 y PF4 como salida:
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
}
void habilitarBotones(){
    // Pines de entrada para los switch buttons integrados:
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));
    // Pines J0 y J1 como entrada:
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
    // Habilitar resistencia de pull-up en PJ0 y PJ1 (los botones usan lógica invertida)
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    // Configurar interrupción por flanco de bajada (cuando se presiona el botón)
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_FALLING_EDGE);
    // Habilitar interrupciones en PJ0 y PJ1
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Registrar y habilitar la interrupción en el controlador NVIC
    IntEnable(INT_GPIOJ);
    IntMasterEnable(); // Habilitar interrupciones globales
}

// Función que se ejecutará cuando ocurra la interrupción en PJ0 o PJ1
void PortJIntHandler(void) {

    // Detectar qué pin generó la interrupción
    uint32_t status = GPIOIntStatus(GPIO_PORTJ_BASE, true);

    // Limpiar la bandera de interrupción
    GPIOIntClear(GPIO_PORTJ_BASE, status);

    if (status & GPIO_PIN_0) {
        // Acción si se presiona el botón PJ0
        GPIOPinWrite(GPIO_PORTN_BASE, (GPIO_PIN_0 | GPIO_PIN_1 ),0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4 ),0x00);
    }

    if (status & GPIO_PIN_1) {
        // Acción si se presiona el botón PJ1
        GPIOPinWrite(GPIO_PORTN_BASE, (GPIO_PIN_0 | GPIO_PIN_1 ),0x03);
        GPIOPinWrite(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_4 ),0x11);
    }
}
