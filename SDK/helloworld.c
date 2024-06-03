/*
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
#include "platform.h"
#include "xbasic_types.h"
#include "xparameters.h"
#include "xuartlite.h"
#include <xil_printf.h>
#include <string.h>
#include "microblaze_sleep.h"
#include "sleep.h"
#include "string.h"
#include "xintc.h"
#include "xil_exception.h"

/************************** Constant Definitions *****************************/

#define UARTLITE_DEVICE_ID      XPAR_UARTLITE_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define UARTLITE_INT_IRQ_ID     XPAR_INTC_0_UARTLITE_0_VEC_ID

XIntc InterruptController;

#define UART_BASE_ADDRESS 0x40600000
#define ID_GET_BASE_ADDRESS 0x44a00000
#define LFSR_BASE_ADDRESS 0x44a10000
#define ENTER_KEY 0x0D
#define BUFFER_SIZE 1024
const int len_pack = 16 + 5;
const int am_stop = 5;

u8 RecvBuffer[BUFFER_SIZE] = { };
int TotalReceivedCount = 0;
u8 command[15] = { };
u8 data_f[1000] = { };
int dataIsReady = 0;

void UartLiteIntrHandler(void *CallBackRef, unsigned int EventData) {
	XUartLite *InstancePtr = (XUartLite *) CallBackRef;

	XUartLite_Recv(InstancePtr, &RecvBuffer[TotalReceivedCount], 1);
	if (RecvBuffer[TotalReceivedCount] == 1) {
		int i = 0;
		while (i < TotalReceivedCount) {
			command[i] = RecvBuffer[i];
//			xil_printf("%c", command[i]);
			i++;
		};
		command[i] = '\0';
		TotalReceivedCount = 0;
		dataIsReady = 1;
	} else if (RecvBuffer[TotalReceivedCount] == 2) {
		int i = 0;
		while (i < TotalReceivedCount) {
			data_f[i] = RecvBuffer[i];
//			xil_printf("%c", data_f[i]);
			i++;
		};
		data_f[i] = '\0';
		TotalReceivedCount = 0;
//		dataIsReady = 1;
	} else {
	TotalReceivedCount++;
	dataIsReady = 0;};
}

int SetupInterruptSystem(XUartLite *UartLitePtr) {
	int Status;

	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_Connect(&InterruptController, UARTLITE_INT_IRQ_ID,
			(XInterruptHandler) XUartLite_InterruptHandler,
			(void *) UartLitePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XIntc_Enable(&InterruptController, UARTLITE_INT_IRQ_ID);
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XIntc_InterruptHandler,
			&InterruptController);
	Xil_ExceptionEnable();
	return XST_SUCCESS;

}

// function, which checks, whether the given number is a power of 2
int is_pow(int a) {
	int met = 0;
	if (!a) {
		return 1;
	}
	while (a) {
		if (a & 1) {
			if (!met) {
				met++;
			} else {
				return 0;
			}
		}
		a = a >> 1;
	}
	return 1;
}

// initializes ID, inserts Hamming bits on corresponding places (1,2,4,8 and so on)
int init_Ham(int val) {
	int curr_bit = 0;
	int ham_val = 0;
	for (int i = 0; i < len_pack; i++) {
		if (!is_pow(i + 1)) {
			ham_val |= ((val >> curr_bit) & 1) << i;
			curr_bit++;
		}
	}
	return ham_val;
}

// encodes the given ID into packet with correct Hamming value
int encode_Ham(int val) {
	int curr_bit = 0;
	// int check_bits=0;
	for (int i = 0; i < am_stop; i++) {
		for (int j = 1; j <= len_pack; j++) {
			if ((j >> i) & 1) {
				curr_bit ^= (val >> (j - 1)) & 1;
			}
		}
		val |= curr_bit << ((1 << i) - 1);
		// check_bits|=curr_bit<<(i);
		curr_bit = 0;
	}

	//general parity bit
	for (int i = 0; i < len_pack; i++) {
		curr_bit ^= (val >> i) & 1;
	}
	// printf("val is %x\n",check_bits);
	val = val << 1;
	val |= curr_bit;
	// check_bits=check_bits<<1;
	// check_bits|=curr_bit;
	return val;
}

// checks the correspondence of Hamming values to
// the stored ID value, corrects it in case with 1
// mistake and detects in case with 2 mistakes
int check_Ham(int val) {
	int res = 0;
	int curr_bit = 0;
	//  printf("val is %x\n",val);
	for (int i = 0; i < am_stop; i++) {
		for (int j = 1; j <= len_pack; j++) {
			if ((j >> i) & 1) {

				curr_bit ^= (val >> (j)) & 1;
				// printf("bit %x, ind %x\n",(val>>(j))&1,j);
			}
		}
		res |= curr_bit << (i);
		// printf("%x\n",res);
		curr_bit = 0;
	}
	//check general parity bit
	for (int i = 0; i <= len_pack; i++) {
		curr_bit ^= (val >> i) & 1;
	}
	// 2 mistakes, can only detect
//    printf("parity %x\n",curr_bit);
	if (res && !curr_bit) {
		return -1;
	}
	//  printf("%x\n",res);
	return res;
}

// inserts stored Hamming values into the received ID
int insert_Ham(int val, int ham) {
	int curr_ham = 0;
	int curr_val = 0;
	int val_fin = 0;
	for (int i = 0; i < len_pack; i++) {
		if (is_pow(i)) {
			val_fin |= ((ham >> curr_ham) & 1) << i;
			curr_ham++;
		} else {
			val_fin |= ((val >> curr_val) & 1) << i;
			curr_val++;
		}
		return val_fin;
	}
}

// function, which returns the actual value (initial ID) without parity bits
int decode_Ham(int val) {
	int res = 0;
	int copy_bit = 0;
	for (int i = 0; i <= len_pack; i++) {
		if (!is_pow(i)) {
			res |= ((val >> i) & 1) << copy_bit;
			copy_bit++;
		}
	}
	return res;
}

void receiveUntil() {
	while (dataIsReady == 0) {
	};
	dataIsReady = 0;
}
const int shift_b[]={0,7,14,15};
const int am_b=4;

int shift_k(int key){
	int last_b=0;
	for(int i=0; i<am_b; i++){
		last_b^=(key>>shift_b[i])&1;
	}
	key<=1;
	key|=last_b;
	key&=0xFFFF;
	return key;
}

int main() {
	// ------ INITIALIZATION OF UARTLITE AND ITS INTER ------
	int Status;
	XUartLite UartPtr;
	XUartLite_Initialize(&UartPtr, XPAR_UARTLITE_0_DEVICE_ID);
	Xuint32 *baseaddr_p = (Xuint32 *) ID_GET_BASE_ADDRESS;
	Xuint32 *baseaddr_LFSR = (Xuint32 *) LFSR_BASE_ADDRESS;
	Status = SetupInterruptSystem(&UartPtr);
	if (Status != XST_SUCCESS) {
		xil_printf("UART initialization failed.\n");
		return XST_FAILURE;
	}
	XUartLite_SetSendHandler(&UartPtr, XUartLite_Send, &UartPtr);
	XUartLite_SetRecvHandler(&UartPtr, UartLiteIntrHandler, &UartPtr);

	XUartLite_EnableInterrupt(&UartPtr);
	int ham_dist = 0;
	// get ID from initialized microcontroller
	int ID = *(baseaddr_p + 1);
			/*		  ID=init_Ham(ID);
			 ID=insert_Ham(ID,ham_dist);
			 int err_bit=check_Ham(ID);
			 if(err_bit){
			 if(err_bit==-1){
			 //restart
			 }
			 else{
			 ID^=1<<err_bit;
			 }
			 }*/
	int data = 0;
	while (1) {
		receiveUntil();
		if (strcmp(command, "GET_ID") == 0) {
			xil_printf("ID: %x", ID);
		} else if (strcmp(command, "GET_RAND") == 0) {
			*(baseaddr_p) =0;
			MB_Sleep(20);
			*(baseaddr_p) = 0xFFFFFFFF;
			for(int i=0; i<1000; i++){};
			data=*(baseaddr_p+2);
			*(baseaddr_p) = 0;
			data = *(baseaddr_p + 2);
			xil_printf("A truly random number: %x", data);
		} else if (strcmp(command, "CIPHER") == 0) {
			*(baseaddr_p) = 1;
			sleep(1);
			data = *(baseaddr_p + 2);
			*(baseaddr_p) = 0;

			int key=0;
			if (ID != data) {
				key= ID ^ data;
			} else {
				 key= ID;
			}
			int curr_key=key;
			xil_printf("%x.",key);
			int size_pack=(int)strchr(data_f,'\0')-(int)data_f;
			for(int i=0; i<size_pack; i+=2){
				int ciph=data_f[i];
				ciph=(ciph<<8)+data_f[i+1];
				ciph^=curr_key;
				curr_key=shift_k(curr_key);
				data_f[i+1]=ciph&0xFF;
				data_f[i]=ciph>>8;
			}

			for (int i = 0; i < size_pack; i++) {
				xil_printf("%c", data_f[i]);
			}
		} else if (strcmp(command, "DECIPHER") == 0) {
			sleep(1);
			int size_key=(int)strchr(data_f, '.')-(int)data_f;
//			xil_printf("%d",size_key);
			char key_s[7]={};
			strncpy(key_s,data_f,size_key);
			int curr_key= strtol(key_s, NULL,16);
//			//xil_printf("%s",key_s);
			int size_pack=(int)strchr(data_f,'\0')-(int)data_f;
		//	xil_printf("%d", size_pack);
			for(int i=size_key+1; i<size_pack; i+=2){
				int ciph=data_f[i];
				ciph=(ciph<<8)+data_f[i+1];
				ciph^=curr_key;
				curr_key=shift_k(curr_key);
				data_f[i+1]=ciph&0xFF;
				data_f[i]=ciph>>8;
			}
//
			for (int i = size_key+1; i < size_pack; i++) {
				xil_printf("%c", data_f[i]);
			}
		} else if (strcmp(command, "FINISH") == 0) {
			xil_printf("Finish the programm");
			return 0;
			break;
		} else {
			xil_printf("Unknown command: %s", command);
		}
		command[0] = '\0';
	}

}
