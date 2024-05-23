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
#include "xbasic_types.h"
#include "xparameters.h"
#include "xuartlite.h"
#include "xil_printf.h"
#include <string.h>
#include "microblaze_sleep.h"
#define UART_BASE_ADDRESS 0x40600000 // Базовый адрес UARTLite
#define ID_GET_BASE_ADDRESS 0x44a00000 // Базовый адрес собственного IP блока ID_GET
#define AXI_MEM_BASE_ADDRESS 0x60000000 // Базовый адрес AXI Memory



int main() {
  // Инициализация UARTLite
  XUartLite UartPtr;
  XUartLite_Initialize(&UartPtr, UART_BASE_ADDRESS);
  Xuint32 *baseaddr_p = (Xuint32 *)AXI_MEM_BASE_ADDRESS;
  int data;
  while (1){
	  u8 command[100]; // Буфер для хранения команды
	  // Прием команды с UARTLite
	  int bytesReceived = XUartLite_Recv(&UartPtr, command, sizeof(command));
	  if (bytesReceived > 0) {
	      // Добавление символа конца строки к команде
		  command[bytesReceived - 1] = '\0';

	      if (strcmp(&command, "GET_ID") == 0) { // Вывод ID устройства
				    

//	        xil_printf("ID: 0x12345678\n");
	      } else if (strcmp(&command, "GET_RND") == 0) { // Генерация и вывод случайного числа
			*(baseaddr_p)=1;
			MB_Sleep(2);
			data=*(baseaddr_p+2);
			*(baseaddr_p)=0;
			xil_printf("rand num %x\n", data);
	      } else if (strcmp(&command, "CIPHER") == 0) { // Шифрование данных (не реализовано)

	        xil_printf("Функция шифрования еще не реализована\n");
	      } else if (strcmp(&command, "FINISH") == 0) { // Завершение работы

	        xil_printf("Завершение работы...\n");
	        return 0;
	        break;
	      } else { // Неизвестная команда

	        xil_printf("Неизвестная команда: %s\n", command);
	      }
	  }
  }

}
