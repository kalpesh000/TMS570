/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/********************************************************************
                            Note:
 * This project is a basic Application Program and this generates a
 * "Boot v3 Application Int.hex" file which is used by Bootloader GUI
 * to flash the Application Code.

********************************************************************/

/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

#include "can.h"
#include "rti.h"
#include "het.h"
#include "gio.h"
#include "reg_system.h"

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */

    uint32 sw = 0, read_id = 0;
    unsigned int delay = 0;
    uint8 rx_buffer[8]= {0};

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */

    rtiInit();
    gioInit();
    canInit();

    gioSetDirection(gioPORTB, 0xFFFFFFFF);

    rtiEnableNotification(rtiNOTIFICATION_COMPARE0);

    _enable_IRQ();

    rtiStartCounter(rtiCOUNTER_BLOCK0);

    read_id=canGetID(canREG1,canMESSAGE_BOX16);
    while(1){
        if(canIsRxMessageArrived(canREG1, canMESSAGE_BOX16)){
            canGetData(canREG1, canMESSAGE_BOX16, rx_buffer);

            if(rx_buffer[0] == 0xF0 && rx_buffer[1] == 0xF0 && rx_buffer[2] == 0xF0 && rx_buffer[3] == 0xF0){
                gioSetBit(gioPORTB, 1, 1);

                systemREG1->SYSECR = (0x10) << 14;
                while(1);
            }

        }

        //Using switch to reset the and jump to bootloader
        sw = gioGetBit(hetPORT1, 15);
        for(delay=0; delay<500; delay++);

        if(!sw){
            gioSetBit(gioPORTB, 1, 1);

            systemREG1->SYSECR = (0x10) << 14;
            while(1);
        }

    }

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
void rtiNotification(uint32 notification){

    gioToggleBit(gioPORTB, 1);
    gioToggleBit(gioPORTB, 2);

    /* Toggle */
    //    gioSetPort(gioPORTB, gioGetPort(gioPORTB) ^ 0x00000002);
}

void gioNotification (gioPORT_t *port, uint32 bit){
    return;
}
void esmGroup1Notification(uint32 channel){
    return;
}
void esmGroup2Notification(uint32 channel){
    return;
}
/* USER CODE END */
