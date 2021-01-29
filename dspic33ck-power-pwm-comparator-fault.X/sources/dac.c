
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
 * File: dac.c 
 * Author: A14426
 * Comments:  source file of dac.h header file
 * Revision history: Initial Release
 */

#include <xc.h>

#include "dac.h"



volatile struct P33C_DAC_MODULE_s* my_dac_module;     // user-defined DAC object 
volatile struct P33C_DAC_INSTANCE_s* my_dac_instance;

volatile uint16_t DAC_Initialize(void){

volatile uint16_t retval=1;

my_dac_module = p33c_DacModule__GetHandle();
p33c_DacModule_ConfigWrite(dacModuleConfigClear);


my_dac_module->DacModuleCtrl1L.bits.CLKSEL =0b10;  // DAC Clock Source: AFPLLO  
my_dac_module->DacModuleCtrl2H.bits.SSTIME = (DAC_SSTIME & 0x0FFF); // Transition Mode Duration (default 0x55 = 340ns @ 500 MHz)     
my_dac_module->DacModuleCtrl2L.bits.TMODTIME = (DAC_TMODTIME  & 0x03FF); // Time from Start of Transition Mode until Steady-State Filter is Enabled (default 0x8A = 552ns @ 500 MHz)


my_dac_instance = p33c_DacInstance__GetHandle(DAC1); //// user-defined DAC 1 object 

p33c_DacInstance_ConfigWrite(DAC1,dacConfigClear);
        
my_dac_instance->DACxDATH.value = (uint16_t)(DACOUT_VALUE_HIGH & 0x0FFF);   // specifies the high DACx data value
    
my_dac_instance->DACxCONL.bits.DACOEN = 1;  // Output DAC voltage to DACOUT1 pin
my_dac_instance->DACxCONL.bits.DACEN = 1;   // enable DAC module
my_dac_module->DacModuleCtrl1L.bits.DACON= 1; // enable DAC

return(retval);

}


