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

	__asm {//DBACE
		// for each file[x]:
		// (#A) rotate 1 bit to left 0xA5  0x4B
		// (#B) swap nibbles 0xD2  0x2D
		// (#C) reverse bit order 0x2D  0xB4
		// (#D) rotate hi nibble left, lo nibble right 0xB4  0x72
		// (#E) code table swap 0xE1  CodeTable[0xE1] 

		xor ecx,ecx 	//sets ecx to zero
		xor edx,edx
		mov esi,data	//moves the file data into edx
		
		Dhigh://not quite correct yet
			xor al,al
			xor ah, ah
			xor bl,bl
			xor bh,bh
			mov bh, [esi+edx]//save an extra copy of original
			mov ah, [esi + edx]

			RCL ah,1
			RCL al,1
			RCL ah, 1
			RCL bl, 1
			RCL ah, 1
			RCL bl, 1
			RCL ah, 1
			RCL bl, 1
			RCR al,1
			RCL bl,1//bl low bits should have correctly rotated high bits of original here

		DlowBits:
			xor al,al
			xor ah,ah
			xor cl,cl
			mov ah,bh

			RCR ah,1
			RCL cl,1
			RCR ah,1
			RCL al,1
			RCR ah, 1
			RCL al, 1
			RCR ah, 1
			RCL al, 1//al low bits should have correctly rotated low bits of original here

		Dsave:
			xor cl,cl
			RCR bl,1
			RCR cl,1
			RCR bl, 1
			RCR cl, 1
			RCR bl, 1
			RCR cl, 1
			RCR bl, 1
			RCR cl, 1//rotated high bits of original are now in cl's high bits 

			RCR cl,4 //rotated high bits of original are now in cl's low bits

			RCR al,1
			RCL cl,1
			RCR al, 1
			RCL cl, 1
			RCR al, 1
			RCL cl, 1
			RCR al, 1
			RCL cl, 1//cl should now have correctly rotated version we are looking for

			mov[esi+edx],cl
		/*B:
			xor al,al
			xor ecx,ecx
			mov ecx,4
			xor ah,ah
			mov al, [esi+edx]
			mov ah,al
			RCL al,1

		loop_B :
			RCL ah, 1
			LOOP loop_B
			dec ah
			mov[esi + edx], ah*/

		//A:
		//	xor al,al
		//	xor ah,ah
		//	
		//	mov al, [esi+edx]
		//	mov ah,al
		//	RCL ah,1
		//	RCL al,1
		//	mov[esi+edx],al
		//	
		//C:
		//	xor ecx, ecx
		//	xor al, al
		//	xor ah, ah
		//	mov ecx, 8
		//	mov al, [esi + edx]

		//loop_C :
		//	RCL al, 1
		//	RCR ah, 1
		//	LOOP loop_C
		//	mov[esi + edx], ah

		//E:
		//	xor bl,bl
		//	xor eax,eax
		//	mov eax, [esi+edx]
		//	and eax, 0xFF

		//	mov bl, [gEncodeTable+eax]
		//	mov [esi+edx],bl

		begin:
			inc edx
			mov ecx,edx
			cmp ecx,dataLength
			jne Dhigh
	}

	return resulti;
} // encryptData