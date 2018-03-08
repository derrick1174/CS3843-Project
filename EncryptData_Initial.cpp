// EncryptData.cpp
//
// This file uses the input data and key information to encrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to encrypt the data as specified by the project assignment
int encryptData(char *data, int dataLength)
{
	int resulti = 0;

	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers

	__asm {
		//This segment of code will perform the same as the following in C: 
		//   for(i=0;i<datalength;i++) 
		//	*(data+i) = *(data+i) ^ 1; //The carrot here means XOR
		XOR ecx,ecx 	//sets ecx to zero
		mov edx,data	//moves the file data into edx
		
		start: 
			edx+ecx
			cmp ecx,datalength
			je done
			inc ecx
			jmp start
		done:
			mov data,edx
			mov resulti,ecx
	}

	return resulti;
} // encryptData