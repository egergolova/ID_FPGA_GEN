/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

//#include <stdio.h>
//#include "platform.h"
//#include "xil_printf.h"
//
//
//int main()
//{
//    init_platform();
//
//    print("Hello World\n\r");
//
//    cleanup_platform();
//    return 0;
//}
#include "platform.h"
#include "xbasic_types.h"
#include "xparameters.h"
#include "xuartlite.h"
#include <xil_printf.h>
#include <string.h>
#include "microblaze_sleep.h"
#include "sleep.h"

#define UART_BASE_ADDRESS 0x40600000 // ������� ����� UARTLite
#define ID_GET_BASE_ADDRESS 0x44a00000 // ������� ����� ������������ IP ����� ID_GET
#define AXI_MEM_BASE_ADDRESS 0x44a00000 // ������� ����� AXI Memory
#define ENTER_KEY 0x0D


const int len_pack=16+5;
const int am_stop=5;

// function, which checks, whether the given number is a power of 2
int is_pow(int a){
    int met=0;
    if(!a){
        return 1;
    }
    while(a){
        if(a&1){
            if(!met){
                met++;
            }
            else{
                return 0;
            }
        }
        a=a>>1;
    }
    return 1;
}

// initializes ID, inserts Hamming bits on corresponding places (1,2,4,8 and so on)
int init_Ham(int val){
        int curr_bit=0;
        int ham_val=0;
        for(int i=0; i<len_pack; i++){
        if(!is_pow(i+1)){
            ham_val|=((val>>curr_bit)&1)<<i;
            curr_bit++;
        }
    }
    return ham_val;
}

// encodes the given ID into packet with correct Hamming value
int encode_Ham(int val){
    int curr_bit=0;
    // int check_bits=0;
    for(int i=0; i<am_stop; i++){
        for(int j=1; j<=len_pack; j++){
            if((j>>i)&1){
                curr_bit^=(val>>(j-1))&1;
            }
        }
        val|=curr_bit<<((1<<i)-1);
        // check_bits|=curr_bit<<(i);
        curr_bit=0;
    }

    //general parity bit
    for(int i=0; i<len_pack; i++){
        curr_bit^=(val>>i)&1;
    }
    // printf("val is %x\n",check_bits);
    val=val<<1;
    val|=curr_bit;
    // check_bits=check_bits<<1;
    // check_bits|=curr_bit;
    return val;
}

// checks the correspondence of Hamming values to
// the stored ID value, corrects it in case with 1
// mistake and detects in case with 2 mistakes
int check_Ham(int val){
    int res=0;
    int curr_bit=0;
  //  printf("val is %x\n",val);
    for(int i=0; i<am_stop; i++){
        for(int j=1; j<=len_pack; j++){
            if((j>>i)&1){
                
                curr_bit^=(val>>(j))&1;
                // printf("bit %x, ind %x\n",(val>>(j))&1,j);
            }
        }
        res|=curr_bit<<(i);
        // printf("%x\n",res);
        curr_bit=0;
   }
   //check general parity bit
   for(int i=0; i<=len_pack;i++){
    curr_bit^=(val>>i)&1;
   }
   // 2 mistakes, can only detect
//    printf("parity %x\n",curr_bit);
   if(res && !curr_bit){
        return -1;
   }
         //  printf("%x\n",res);
    return res;
}

// inserts stored Hamming values into the received ID
int insert_Ham(int val, int ham){
    int curr_ham=0;
    int curr_val=0;
    int val_fin=0;
    for(int i=0; i<len_pack; i++){
        if(is_pow(i)){
            val_fin|=((ham>>curr_ham)&1)<<i;
            curr_ham++;
        }
        else{
            val_fin|=((val>>curr_val)&1)<<i;
            curr_val++;
        }
        return val_fin;
    }
}

// function, which returns the actual value (initial ID) without parity bits
int decode_Ham(int val){
   int res=0;
   int copy_bit=0;
    for(int i=0; i<=len_pack; i++){
        if(!is_pow(i)){
            res|=((val>>i)&1)<<copy_bit;
            copy_bit++;
        }
    }
    return res;
}




void receiveUntilEnter(XUartLite *UartPtr, u8 *buffer, int buffer_size) {
	int bytesReceived = 0;
	int totalBytesReceived = 0;

	while (1) {
		bytesReceived = XUartLite_Recv(UartPtr, &buffer[totalBytesReceived], 1);
		if (bytesReceived > 0) {
			xil_printf("%c", buffer[totalBytesReceived]);
			if (buffer[totalBytesReceived] == ENTER_KEY) { // ���� ������ ��� ENTER
				buffer[totalBytesReceived] = '\0';
				xil_printf("\n\r");
				break;
			}
			totalBytesReceived++;
			if (totalBytesReceived >= buffer_size - 1) { // ���� ����� ��������
				buffer[totalBytesReceived] = '\0';
				xil_printf("\n\r");
				break;
			}
		}
	}
}

int main() {
	// ������������� UARTLite
	XUartLite UartPtr;
	XUartLite_Initialize(&UartPtr, UART_BASE_ADDRESS);
	Xuint32 *baseaddr_p = (Xuint32 *) AXI_MEM_BASE_ADDRESS;
	xil_printf("\rHello! Enter the command:\n\r");
	int data;
	// this hamming distance shall be implemented during initialization somehow
	int ham_dist=0;
	// get ID from initialized microcontroller
	int ID=*(baseaddr_p + 1);
	ID=init_Ham(ID);
	ID=inert_Ham(ID,ham_dist);
	int err_bit=check_Ham(ID);
		if(err_bit){
			if(err_bit==-1){
				//restart
			}
			else{
				ID^=1<<err;
			}
		}

	while (1) {
		u8 command[100]; // ����� ��� �������� �������
		// ����� ������� � UARTLite
//	  int bytesReceived = XUartLite_Recv(&UartPtr, command, sizeof(command));
		receiveUntilEnter(&UartPtr, &command, 100);
		if (strcmp(&command, "GET_ID") == 0) { // ����� ID ����������
			xil_printf("%x\n\r",ID);

//	        xil_printf("ID: 0x12345678\n");
		} else if (strcmp(&command, "GET_RAND") == 0) { // ��������� � ����� ���������� �����
			xil_printf("SLV_REG0 %x\n\r", *(baseaddr_p));
			xil_printf("SLV_REG1 %x\n\r", *(baseaddr_p + 1));
			xil_printf("SLV_REG2 %x\n\r", *(baseaddr_p + 2));
			*(baseaddr_p) = 0xFFFFFFFF;
			//MB_Sleep(50);
			for(int i=0; i<10000; i++){};
			data = *(baseaddr_p + 2);
			*(baseaddr_p) = 0;

			xil_printf("rand num %x\n\r", data);
			xil_printf("SLV_REG0 %x\n\r", *(baseaddr_p));
			xil_printf("SLV_REG1 %x\n\r", *(baseaddr_p + 1));
			xil_printf("SLV_REG2 %x\n\r", *(baseaddr_p + 2));
		} else if (strcmp(&command, "CIPHER") == 0) { // ���������� ������ (�� �����������)
			for(int i=0; i<10000; i++){};
			data = *(baseaddr_p + 2);
			*(baseaddr_p) = 0;
			// need to receive the size of file.

			xil_printf("Encipher function is not ready yet\n");
		} else if (strcmp(&command, "FINISH") == 0) { // ���������� ������

			xil_printf("Finish the programm\n");
			return 0;
			break;
		} else { // ����������� �������

			xil_printf("Unknown command: %s\n", command);
		}

	}

}
