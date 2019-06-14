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
 * This project is a basic Bootloader and is programmed according to "Bootloader Flow.pdf"
 * document.The GUI version used for the test is v0.1.

                      Used Parameters:
 * Starting Address of Application is 0x14000.

 * canMESSAGE_BOX1 ID = 0x402; //Tx for Bootmode.
 * canMESSAGE_BOX2 ID = 0x403; //Rx for Mode Select [Erase/Program/ASC(All Send Complete)].
 * canMESSAGE_BOX3 ID = 0x403; //Tx for Mode Select Ack.
 * canMESSAGE_BOX4 ID = 0x405; //Rx for Receive Add.
 * canMESSAGE_BOX5 ID = 0x405; //Tx for Receive Add Ack.
 * canMESSAGE_BOX6 ID = 0x406; //Rx for Data 1.
 * canMESSAGE_BOX7 ID = 0x406; //Tx for Data 1 Ack.
 * canMESSAGE_BOX8 ID = 0x407; //Rx for Data 2.
 * canMESSAGE_BOX9 ID = 0x407; //Tx for Data 2 Ack.
 * canMESSAGE_BOX10 ID = 0x408; //Rx for Data 3.
 * canMESSAGE_BOX11 ID = 0x408; //Tx for Data 3 Ack.
 * canMESSAGE_BOX12 ID = 0x409; //Rx for Data 4.
 * canMESSAGE_BOX13 ID = 0x409; //Tx for Data 4 Ack.

 * At ID 0x403 :
 *          D0 = 0x01 for Erase,
 *          D0 = 0x02 for Program,
 *          D0 = 0x03 for ASC,
 *          D1 = 0x01 for Erase Ack,
 *          D2 = 0x02 for Program Ack,
 *          D3 = 0x03 for ASC Ack.
 *          Else = 0x00.

                      Messages for the Tx GUI :
 * tx_data_ack[7] = 0x10;

 * rx_ack[7] is 0x10 then enter modes loop.
 ********************************************************************/

/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

#include "gio.h"
#include "can.h"
#include "bl_flash.h"
#include "rti.h"

uint32_t Jump_address = 0x14000;

#define erase_mode    0x01
#define program_mode  0x02
#define appjump_mode  0x03
#define progTest_mode 0x04
#define ack_bit       0x55

#define boot_mode_msgbox    canMESSAGE_BOX1
#define mode_select_msgbox  canMESSAGE_BOX2
#define amode_select_msgbox canMESSAGE_BOX3
#define receive_add_msgbox  canMESSAGE_BOX4
#define areceive_add_msgbox canMESSAGE_BOX5
#define data1_msgbox        canMESSAGE_BOX6
#define adata1_msgbox       canMESSAGE_BOX7
#define data2_msgbox        canMESSAGE_BOX8
#define adata2_msgbox       canMESSAGE_BOX9
#define data3_msgbox        canMESSAGE_BOX10
#define adata3_msgbox       canMESSAGE_BOX11
#define data4_msgbox        canMESSAGE_BOX12
#define adata4_msgbox       canMESSAGE_BOX13

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
  //Change as per the start address
  uint32_t jumpAdd = 0;

  void canrxMessageArrived(canBASE_t *node, uint32 messageBox);
  void gui_basictest(void);
  void led_blink(void);

  uint32_t GotoAddress = 0;

  uint8_t rx_ack[8]     = {0};
  uint8_t rx_mode[8]    = {0};
  uint8_t rx_add[8]     = {0};
  uint8_t rx_data1[8]   = {0};
  uint8_t rx_data2[8]   = {0};
  uint8_t rx_data3[8]   = {0};
  uint8_t rx_data4[8]   = {0};
  uint8_t tx_data_ack[8]    = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t tx_bootmode_ack[8]= {0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0};


  uint8_t bufferdata0[4]   = {0};
  uint8_t bufferdata1[8]   = {0};
  uint8_t bufferdata2[8]   = {0};
  uint8_t bufferdata3[8]   = {0};
  uint8_t bufferdata4[4]   = {0};

  uint8_t mode = 0;
  boolean  bootflag = false;

  uint32_t read_id = 0x00;
  uint32_t progAdd = 0x00;
  uint8_t progAdd8 = 0;

  uint32 currentCount = 0;
  uint8 jumpCount = 0;

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */

    uint64_t i = 0;
    uint8_t pm_flag = 1;

    gioInit();
    gioSetDirection(gioPORTB,0xFFFFFFFF);

    canInit();
    rtiInit();

    while(1){

        //Start the Timer
        rtiStartCounter(rtiCOUNTER_BLOCK0);
        currentCount = rtiGetCurrentTick(rtiCOMPARE0) ;

        //Send the Boot Mode Command on interval of 100ms
        if (currentCount >= 1000000){
            // Sends Boot Mode acknowledgment to GUI
            canTransmit(canREG1, boot_mode_msgbox, tx_bootmode_ack);

            jumpCount++;

            //Reset the Timer
            rtiStopCounter(rtiCOUNTER_BLOCK0);
            rtiResetCounter(rtiCOUNTER_BLOCK0);
            rtiStartCounter(rtiCOUNTER_BLOCK0);
        }

        //Check for Bootmode Ack
        if(canIsRxMessageArrived(canREG1, mode_select_msgbox)){
            canGetData(canREG1, mode_select_msgbox, rx_mode);
        }

        //Check if Mode selection Command is received
        if(rx_mode[0]!=0x00 && jumpCount <=10){
            //Set the bootflag which will allow program to enter Boot Mode Selection State
            bootflag = true;
        }

        //After 1sec if the Mode Selection Command is not received them jump to application
        else if(rx_mode[0]==0x00 && jumpCount >=10){
            jumpCount = 0;

        //Application Jump Instruction
        GotoAddress = (uint32_t)(Jump_address);
        ((void (*)(void))GotoAddress)();
        while(1);
        }


        /************************* Start of Boot Mode Selection State ***************************************/

        while(bootflag){

            // Receives Mode data from GUI
            if(canIsRxMessageArrived(canREG1, mode_select_msgbox)){
                canGetData(canREG1, mode_select_msgbox, rx_mode);
            }

            //So, when rx_mode[0] = 0 the the bootloader resets and keeps sending Boot cmd
            if(rx_mode[0] == 3){
                pm_flag = 1;
            }

            // Switch case for different modes
            switch(rx_mode[0]){

                case erase_mode:{ // Erase
                    rx_mode[0] = 0;

                    //Erase Sector
                    Fapi_BlockErase(0, Jump_address, 0x2C000);

                    //Send Ack for Erase completion
                    tx_data_ack[0] = erase_mode;
                    tx_data_ack[1] = ack_bit;
                    canTransmit(canREG1, amode_select_msgbox, tx_data_ack);
                    for(i=0;i<=7;i++){
                        tx_data_ack[i]=0;
                    }

                    break;
                }

                case program_mode:{  //Program
                    rx_mode[0] = 0;

                    //Send Ack for Program Mode Entry
                    tx_data_ack[0] = program_mode;
                    tx_data_ack[1] = ack_bit;
                    canTransmit(canREG1, amode_select_msgbox, tx_data_ack);
                    for(i=0;i<=7;i++){
                        tx_data_ack[i]=0;
                    }

                    //Loop until Jump to Application message does not arrive
                    while(rx_mode[0] != 0x03){
                        //Address read
                        if(pm_flag == 1){
                            if(canIsRxMessageArrived(canREG1, receive_add_msgbox)){
                                canGetData(canREG1, receive_add_msgbox, rx_add);

                                //Send Ack
                                tx_data_ack[0] = 0xF1;
                                tx_data_ack[1] = ack_bit;
                                canTransmit(canREG1, areceive_add_msgbox, tx_data_ack);
                                for(i=0;i<=7;i++){
                                    tx_data_ack[i]=0;
                                }

                                //Set Flag
                                pm_flag = 2;
                            }
                        }

                        //Data 1 read
                        if(pm_flag == 2){
                            if(canIsRxMessageArrived(canREG1, data1_msgbox)){
                                canGetData(canREG1, data1_msgbox, rx_data1);

                                //Send Ack
                                tx_data_ack[0] = 0xF2;
                                tx_data_ack[1] = ack_bit;
                                canTransmit(canREG1, adata1_msgbox, tx_data_ack);
                                for(i=0;i<=7;i++){
                                    tx_data_ack[i]=0;
                                }

                                //Set Flag
                                pm_flag = 3;
                            }

                        }

                        //Data 2 read
                        if(pm_flag == 3){
                            if(canIsRxMessageArrived(canREG1, data2_msgbox)){
                                canGetData(canREG1, data2_msgbox, rx_data2);

                                //Send Ack
                                tx_data_ack[0] = 0xF3;
                                tx_data_ack[1] = ack_bit;
                                canTransmit(canREG1, adata2_msgbox, tx_data_ack);
                                for(i=0;i<=7;i++){
                                    tx_data_ack[i]=0;
                                }

                                //Set Flag
                                pm_flag = 4;
                            }
                        }

                        //Data 3 read
                        if(pm_flag == 4){
                            if(canIsRxMessageArrived(canREG1, data3_msgbox)){
                                canGetData(canREG1, data3_msgbox, rx_data3);

                                //Send Ack
                                tx_data_ack[0] = 0xF4;
                                tx_data_ack[1] = ack_bit;
                                canTransmit(canREG1, adata3_msgbox, tx_data_ack);
                                for(i=0;i<=7;i++){
                                    tx_data_ack[i]=0;
                                }

                                //Set Flag
                                pm_flag = 5;
                            }
                        }

                        //Data 4 read
                        if(pm_flag == 5){
                            if(canIsRxMessageArrived(canREG1, data4_msgbox)){
                                canGetData(canREG1, data4_msgbox, rx_data4);
                                pm_flag = 6;
                            }
                        }

                        //Program the data at the address
                        if(pm_flag == 6){
                            //Merge the jump address
                            progAdd = rx_add[0] + (rx_add[1] << 8) + (rx_add[2] << 16) + (rx_add[3] << 24);

                            progAdd8 = progAdd % 8;

                            if (progAdd8 == 4){
                                for(i=0; i<4; i++){
                                    bufferdata0[i] = rx_data1[i];
                                    bufferdata1[i] = rx_data1[i+4];

                                    bufferdata1[i+4] = rx_data2[i];
                                    bufferdata2[i] = rx_data2[i+4];

                                    bufferdata2[i+4] = rx_data3[i];
                                    bufferdata3[i] = rx_data3[i+4];

                                    bufferdata3[i+4] = rx_data4[i];
                                    bufferdata4[i] = rx_data4[i+4];
                                }
                                Fapi_BlockProgram( 0, progAdd     ,  (uint32_t)&bufferdata0[0],4);
                                Fapi_BlockProgram( 0, (progAdd+4 ),  (uint32_t)&bufferdata1[0],8);
                                Fapi_BlockProgram( 0, (progAdd+12),  (uint32_t)&bufferdata2[0],8);
                                Fapi_BlockProgram( 0, (progAdd+20),  (uint32_t)&bufferdata3[0],8);
                                Fapi_BlockProgram( 0, (progAdd+28),  (uint32_t)&bufferdata4[0],4);
                            }
                            else{
                                Fapi_BlockProgram(0,(progAdd   ),(uint32_t)&rx_data1[0],8);
                                Fapi_BlockProgram(0,(progAdd+8 ),(uint32_t)&rx_data2[0],8);
                                Fapi_BlockProgram(0,(progAdd+16),(uint32_t)&rx_data3[0],8);
                                Fapi_BlockProgram(0,(progAdd+24),(uint32_t)&rx_data4[0],8);
                            }

                            //Reset the Address & Data Arrays
                            for(i=0;i<=7;i++){
                                rx_add[i]  = 0;
                                rx_data1[i]= 0;
                                rx_data2[i]= 0;
                                rx_data3[i]= 0;
                                rx_data4[i]= 0;
                                bufferdata1[i]= 0;
                                bufferdata2[i]= 0;
                                bufferdata3[i]= 0;
                            }

                            for(i=0;i<=3;i++){
                                bufferdata0[i]= 0;
                                bufferdata4[i]= 0;
                            }

                            //Send Ack
                            tx_data_ack[0] = 0xF5;
                            tx_data_ack[1] = ack_bit;
                            canTransmit(canREG1, adata4_msgbox, tx_data_ack);
                            for(i=0;i<=7;i++){
                                tx_data_ack[i]=0;
                            }
                            pm_flag = 1;
                        }

                        //Mode check to get out of while loop
                        if(canIsRxMessageArrived(canREG1, mode_select_msgbox)){
                            canGetData(canREG1, mode_select_msgbox, rx_mode);
                        }

                      //So, when rx_mode[0] = 0 the the bootloader resets and keeps sending Boot cmd
                        if(rx_mode[0] == 3){
                            pm_flag = 1;
                        }
                    }
                break;
                }

                case appjump_mode:{  // Jump to application code
                    rx_mode[0] = 0;

                    //All receive is completed and now PC is jumping to Application code
                    tx_data_ack[0] = appjump_mode;
                    tx_data_ack[1] = ack_bit;
                    canTransmit(canREG1, amode_select_msgbox, tx_data_ack);
                    for(i=0;i<=7;i++){
                        tx_data_ack[i]=0;
                    }

                    //Jump Instruction
                    GotoAddress = (uint32_t)(Jump_address);
                    ((void (*)(void))GotoAddress)();
                       while(1);
                     break;
                    }

                    case progTest_mode:{  //Program
                        rx_mode[0] = 0;

                        //Send Ack for Program completion
                        tx_data_ack[0] = progTest_mode;
                        tx_data_ack[1] = ack_bit;
                        canTransmit(canREG1, amode_select_msgbox, tx_data_ack);
                        for(i=0;i<=7;i++){
                            tx_data_ack[i]=0;
                        }

                        break;
                }

                default:{  //Default

                    //Mode check to get out of while loop
                    if(canIsRxMessageArrived(canREG1, mode_select_msgbox)){
                        canGetData(canREG1, data1_msgbox, rx_mode);
                    }

                    //So, when rx_mode[0] = 0 the the bootloader resets and keeps sending Boot cmd
                    if(rx_mode[0] == 3){
                        pm_flag = 1;
                    }

                    break;
                }
            }
        }

        /************************* End of Boot Mode Selection State ***************************************/
    }

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

void canrxMessageArrived(canBASE_t *node, uint32 messageBox){
    uint64_t wcounter = 1;
    uint8_t b1while = 1;

//    b1while = 1;
    while((!canIsRxMessageArrived(node, messageBox)) & b1while){
        wcounter++;
        if (wcounter >= 200000){
            b1while = 0;
        }
    }
    wcounter = 0;
//    canGetData(node, messageBox, rx_ack);
}

void led_blink(void){
    unsigned int delay = 0;

    gioSetBit (gioPORTB, 1, 1);
    for(delay=0;delay<5000000;delay++);
    gioSetBit (gioPORTB, 1, 0);
    for(delay=0;delay<5000000;delay++);
}

void gui_basictest(void){
    while(!canIsRxMessageArrived(canREG1, receive_add_msgbox));
    canGetData(canREG1, receive_add_msgbox, rx_add);

    while(!canIsRxMessageArrived(canREG1, data1_msgbox));
    canGetData(canREG1, data1_msgbox, rx_data1);

    while(!canIsRxMessageArrived(canREG1, data2_msgbox));
    canGetData(canREG1, data2_msgbox, rx_data2);

    while(!canIsRxMessageArrived(canREG1, data3_msgbox));
    canGetData(canREG1, data3_msgbox, rx_data3);

    while(!canIsRxMessageArrived(canREG1, data4_msgbox));
    canGetData(canREG1, data4_msgbox, rx_data4);

    progAdd = rx_add[0] + (rx_add[1] << 8) + (rx_add[2] << 16) + (rx_add[3] << 24);
    Fapi_BlockErase(0, progAdd, 0x2000);
    Fapi_BlockProgram( 0, progAdd,  (uint32_t)&rx_data1[0],8);
    Fapi_BlockProgram( 0, (progAdd+8),  (uint32_t)&rx_data2[0],8);
    Fapi_BlockProgram( 0, (progAdd+16),  (uint32_t)&rx_data3[0],8);
    Fapi_BlockProgram( 0, (progAdd+24),  (uint32_t)&rx_data4[0],8);
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
