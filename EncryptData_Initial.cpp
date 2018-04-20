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
		
		xor ecx,ecx 	//sets ecx to zero
		xor eax, eax
		mov eax, dataLength
		xor edx,edx
		mov esi,data	//moves the file data into edx

		push ebp
		mov ebp,esp
		sub esp,16

		mov[ebp-4],eax //datalength
		mov[ebp - 8], ecx//current index
		mov[ebp - 12], ecx//hop count
		mov[ebp - 16], ecx//current round
		
		begin_of_encrypt:
			xor eax,eax
			xor ecx,ecx
			xor ebx,ebx

			mov ecx, [ebp-16]
			mov ah, [gPasswordHash+ecx*4]
			mov al, [gPasswordHash+1+ecx*4]

			mov bh, [gPasswordHash+2+ecx*4]
			mov bl, [gPasswordHash + 3 + ecx * 4]

			cmp bx,0x0
			jne zero_check
			mov bx,0xFFFF
		zero_check:
			mov [ebp-8],eax
			mov [ebp-12],ebx

		hop_step:
			xor eax,eax
			xor ebx,ebx
			xor ecx,ecx
			lea ecx,gkey 
			mov ebx, [ebp-8]

			mov al, [esi+edx]
			xor al,[ecx+ebx]

			mov [esi+edx],al

			xor eax,eax
			xor ecx,ecx

			add ebx,[ebp-12]
			cmp ebx,65537
			jb index_check
			sub ebx,65537

		index_check:
			mov[ebp-8],ebx


		//DBACE
		Dhigh://(first bit)(second bit)(third bit)(fourth bit) to (second bit)(third bit)(fourth bit)(first bit)
			xor al,al
			xor ah, ah
			xor bl,bl
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

		DlowBits://(first bit)(second bit)(third bit)(fourth bit) to (fourth bit)(first bit)(second bit)(third bit)
			xor al,al
			xor ah,ah
			mov ah, [esi+edx]

			RCR ah,1
			RCL al,1//moves the fourth bit (that we want rotated) into the last bit of al

			RCL ah,5//moves the other three bits we want up into queue for rotation
			RCL ah,1//moves the first bit into carry flag
			RCL al,1//moves first bit into al behind fourth bit
			RCL ah, 1//moves second bit into carry flag
			RCL al, 1//moves second bit into al behind first bit
			RCL ah, 1//moves third bit into carry flag
			RCL al, 1//moves third bit behind second bit. al low nibble now holds correctly rotated version.
			RCL al,4//al high nibble should have correctly rotated low bits of original here

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
			
			//need to move al's high nibble into cl's low nibble
			RCL al,1
			RCL cl,1
			RCL al, 1
			RCL cl, 1
			RCL al, 1
			RCL cl, 1
			RCL al, 1
			RCL cl, 1//cl should now have correctly rotated version we are looking for

			mov[esi + edx], cl
			
		B:
			xor al, al
			xor bl, bl
			xor ecx, ecx
			mov ecx, 4
			xor ah, ah
			mov al, [esi + edx]
			mov bl, al

			rol al,4
			mov[esi+edx],al

		A:
			xor al,al
			xor ah,ah
			
			mov al, [esi+edx]
			mov ah,al
			RCL ah,1
			RCL al,1
			mov[esi+edx],al
			
		C:
			xor ecx, ecx
			xor al, al
			xor ah, ah
			mov ecx, 8
			mov al, [esi + edx]

		loop_C :
			RCL al, 1
			RCR ah, 1
			LOOP loop_C
			mov[esi + edx], ah

		E:
			xor bl,bl
			xor eax,eax
			mov eax, [esi+edx]
			and eax, 0xFF

			mov bl, [gEncodeTable+eax]
			mov [esi+edx],bl
			
		begin:
			inc edx
			mov ecx,edx
			cmp ecx,[ebp-4]
			jne hop_step

		begin_newRound:
			xor ecx,ecx
			xor eax,eax
			xor ebx,ebx
			xor edx,edx

			mov ecx, [ebp-16]
			inc ecx
			mov[ebp-16],ecx
			cmp ecx,gNumRounds
			jne begin_of_encrypt

			mov esp,ebp
			pop ebp
	}

	return resulti;
} // encryptData