// DecryptData.cpp
//
// THis file uses the input data and key information to decrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to decrypt the data as specified by the project assignment
int decryptData(char *data, int dataLength)
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

		xor ecx, ecx 	//sets ecx to zero
		xor edx, edx
		mov esi, data	//moves the file data into edx

		Dhigh: //(2)(3)(4)(1) back to (1)(2)(3)(4)
			xor ah, ah
			xor bl, bl
			mov ah, [esi + edx]

			RCR ah,4//moves ah's high nibble to low nibble
			RCR ah,1//gets (1) into carry flag
			RCL bl,1//moves (1) into last bit of bl
			RCL ah,5//moves the other three bits we want up into queue for rotation
			RCL ah,1//moves (2) into carry flag
			RCL bl,1//moves (2) into bl
			RCL ah,1//moves (3) into carry flag
			RCL bl,1//moves (3) into bl
			RCL ah,1//moves (4) into carry flag
			RCL bl,1//moves (4) into bl
			RCL bl,4//bl high nibble now has decrypted high nibble of original

		Dlow: //(4)(1)(2)(3) back into (1)(2)(3)(4)
			xor al, al
			xor ah, ah
			xor bh,bh
			mov ah, [esi + edx]

			RCL ah,5//moves bits 1,2,3 into position (bit 4 is in carry flag)
			RCL bh,1//moves (4) into bh
			RCL ah,1//moves (1) into carry flag
			RCL al,1//moves (1) into al
			RCL ah,1//moves (2) into carry flag
			RCL al,1//moves (2) into al
			RCL ah,1//moves (3) into carry flag
			RCL al,1//moves (3) into al
			RCR bh,1//moves (4) into carry flag
			RCL al,1//moves (4) into al
			RCL al,4//shifts al's low nibble (contains decrypted low nibble of original) into al's high nibble

		Dsave:
			xor cl,cl//will contain decrypted version by the end

			RCL bl,1
			RCL cl,1
			RCL bl, 1
			RCL cl, 1
			RCL bl, 1
			RCL cl, 1
			RCL bl, 1
			RCL cl, 1//cl low nibble now contains high nibble of decrypted version

			RCL al,1
			RCL cl,1
			RCL al, 1
			RCL cl, 1
			RCL al, 1
			RCL cl, 1
			RCL al, 1
			RCL cl, 1//low nibble of decrypted is now in low nibble of cl with high nibble of decrypted shifted into high nibble of cl

			mov[esi + edx], cl

		B:
			xor al, al
			xor ecx, ecx
			mov ecx, 4
			xor ah, ah
			mov al, [esi + edx]
			mov ah, al
			RCL al, 1

		loop_B :
			RCL ah, 1
			LOOP loop_B
			inc ah
			mov[esi + edx], ah

		A :
			xor al, al
			xor ah, ah

			mov al, [esi + edx]
			mov ah, al
			RCR ah, 1
			RCR al, 1
			mov[esi + edx], al

		C :
			xor ecx,ecx
			xor al,al
			xor ah,ah
			mov ecx,8
			mov al, [esi+edx]
			

		loop_C:
			  RCL al,1
			  RCR ah,1
			  LOOP loop_C
			  mov[esi+edx],ah
		E :
			xor bl, bl
			xor eax, eax
			mov eax, [esi + edx]
			and eax, 0xFF

			mov bl, [gDecodeTable + eax]
			mov[esi + edx], bl

		begin :
			inc edx
			mov ecx, edx
			cmp ecx, dataLength
			jne Dhigh
	}

	return resulti;
} // decryptData

