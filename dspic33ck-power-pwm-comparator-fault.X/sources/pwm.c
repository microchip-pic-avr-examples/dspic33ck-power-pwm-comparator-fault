/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File: pwc.c 
 * Author: A14426
 * Comments:  source file of pwm.h header file
 * Revision history: 0
 */

#include <xc.h>
#include "dac.h"
#include "pwm.h"


/* Declare global, user-defined PWM generator object */
volatile struct P33C_PWM_GENERATOR_s* my_pg;    // user-defined PWM generator 1 object 



volatile uint16_t PWM_Initialize(void) {
    
    volatile uint16_t retval=1;

    // Default PWM Initialization for 500 MHz input clock from AUX PLL
    p33c_PwmModule_Initialize();

    // Capture handle to the desired PWM generator 
    my_pg = p33c_PwmGenerator_GetHandle(MY_PWM_GENERATOR);
   
    // Reset PGx SFRs to RESET conditions
    p33c_PwmGenerator_ConfigWrite(MY_PWM_GENERATOR, pgConfigClear);
  
    
    // Set individual PWM generator configuration for PG1
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // PGxCONL: PWM GENERATOR x CONTROL REGISTER LOW
    my_pg->PGxCONL.bits.CLKSEL = 0b01; // PWM Generator uses Master clock selected by the MCLKSEL[1:0] (PCLKCON[1:0]) control bits
    my_pg->PGxCONL.bits.MODSEL = 0b000; // Independent Edge PWM mode
    my_pg->PGxCONL.bits.HREN = 1; // PWM Generator x operates in High-Resolution mode
    
    // PGxCONH: PWM GENERATOR x CONTROL REGISTER HIGH
    my_pg->PGxCONH.bits.UPDMOD = 0b00; // SOC update: Data registers at start of next PWM cycle if UPDREQ = 1
    my_pg->PGxCONH.bits.TRGMOD = 0b0; // PWM Generator operates in Single Trigger mode
    my_pg->PGxCONH.bits.SOCS = 0b0000; // SOCS[3:0]: Start-of-Cycle Selection: Local EOC ? PWM Generator is self-triggered
    
    // PGxIOCONL: PWM GENERATOR x I/O CONTROL REGISTER LOW
    my_pg->PGxIOCONL.bits.OSYNC = 0b00; // User output overrides via the OVRENH/L and OVRDAT[1:0] 
                                                 // bits are synchronized to the local PWM time base (next Start-of-Cycle)
    my_pg->PGxIOCONL.bits.OVRDAT = 0b00; // Both PWM outputs are LOW in override mode
    my_pg->PGxIOCONL.bits.OVRENL = 1; // OVRDAT0 provides data for output on the PWMxL pin
    my_pg->PGxIOCONL.bits.OVRENH = 1; // OVRDAT1 provides data for output on the PWMxH pin
        
    // PGxIOCONH: PWM GENERATOR x I/O CONTROL REGISTER HIGH
    my_pg->PGxIOCONH.bits.PMOD = 0b00; // PWM Generator outputs operate in Complementary mode
    
    // Set PWM signal generation timing of this generator 
    my_pg->PGxPER.value = PWM_PERIOD;   
    my_pg->PGxDC.value  = PWM_DUTY_CYCLE;   
    my_pg->PGxDTH.value = 0;     
    my_pg->PGxDTL.value = 0;     
    
    
    my_pg->PGxIOCONL.bits.FLTDAT = 0b00;/* Data for PWM1H/PWM1L Pins if FLT Event is Active bits
       If Fault is active, then FLTDAT<1> provides data for PWM1H.
       If Fault is active, then FLTDAT<0> provides data for PWM1L.*/
    
 
    //*PCI Fault ACCEPTANCE MODE*/
    #if defined (__MA330048_dsPIC33CK_DPPIM__)
    my_pg->PGxFPCIH.bits.ACP = 0b011;   //Acceptance Mode LATCHED
    #elif defined (__MA330049_dsPIC33CH_DPPIM__)
    my_pg->PGxFPCIH.bits.ACP = 0b011;   //Acceptance Mode LATCHED       
    #endif

 
    //*Latch Mode 
    my_pg->PGxFPCIH.bits.LATMOD = 0;        //Fault is dominant over Terminate Fault during Latch
    
    //*PCI Fault Source/
    my_pg->PGxFPCIL.bits.PSS = 0b11011;   //PCI Fault from Comparator 1 Output
    my_pg->PGxFPCIL.bits.PPS = 0;         //PCI Fault Source Polarity
    my_pg->PGxFPCIL.bits.AQSS = 0b000;    //PCI Fault Source Qualifier (None = Logic 1))
    my_pg->PGxFPCIL.bits.AQPS = 0;        //PCI Fault Source Qualifier Polarity
    
    //*PCI Fault Terminate
    my_pg->PGxFPCIL.bits.TERM = 0b111;  //PCI Fault Terminate pin PCI9R
    my_pg->PGxFPCIL.bits.SWTERM = 1;    //PCI Fault Terminate Software
    my_pg->PGxFPCIL.bits.TSYNCDIS = 0;  //PCI Fault Terminate Sync Disable from PWM EOC
    my_pg->PGxFPCIH.bits.TQSS = 0b110;  //Termination Qualifier (PCI9R)
    my_pg->PGxFPCIH.bits.TQPS = 1 ;     //PCI Fault Terminate Qualifier Polarity
    
    // Enable PWM generators with outputs DISABLED
    p33c_PwmGenerator_Enable(my_pg); 
   
    
    Nop(); // Place breakpoint to review PWM configuration
    Nop(); // using the Watch Window
    Nop();
    
    // Enable PWM generator outputs (starts signal generation at next period transition)
    p33c_PwmGenerator_Resume(my_pg);
    
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PLEASE NOTE:
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Digital Power Plug-In Modules may have additional circuits dedicated to 
    // specific functions connected to some of their device pins. The following  
    // pre-compiler instructions disabling certain PWM outputs interfering with  
    // analog filter circuits and are therefore not available on the hardware
    // you may be using.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    #if defined (__MA330048_dsPIC33CK_DPPIM__)

    // On dsPIC33CK DP PIM (MA330048) PWM generator output PWM7L is shared
    // with the on-board push button of the Digital Power Development
    // Board (DM330029). The PWM7L output signal will get filtered and
    // distorted by the switch debounce capacitor and is therefore 
    // being turned off here.
    
    if(MY_PWM_GENERATOR == 7) 
    {
        my_pg->PGxIOCONL.bits.OVRENL = 1;
        my_pg->PGxIOCONH.bits.PENL = 0;
    }
    
    
    #elif defined (__MA330049_dsPIC33CH_DPPIM__)

    // On dsPIC33CH DP PIM (MA330049) PWM generator output PWM2L is shared
    // with the on-board push button of the Digital Power Development
    // Board (DM330029). The PWM7L output signal will get filtered and
    // distorted by the switch debounce capacitor and is therefore 
    // being turned off here.

    if(MY_PWM_GENERATOR == 2) 
    {
        my_pg.pgHandle->PGxIOCONL.bits.OVRENL = 1;
        my_pg.pgHandle->PGxIOCONH.bits.PENL = 0;
    }
    
    #endif

    // Check return value
    retval = (bool)(my_pg->PGxCONL.bits.ON) &&         // Check if PWM generator is turned on
                   (!my_pg->PGxIOCONL.bits.OVRENH);  
    
    return(retval); // Return 1=success, 0=failure
}
