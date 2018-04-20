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
			xor ecx, ecx 	//sets ecx to zero
			xor eax, eax
			mov eax, dataLength
			xor edx, edx
			xor ebx,ebx
			mov esi, data	//moves the file data into edx
			mov ebx,gNumRounds
			dec ebx

			push ebp
			mov ebp, esp
			sub esp, 16

			mov[ebp - 4], eax //datalength
			mov[ebp - 8], ecx//current index
			mov[ebp - 12], ecx//hop count
			mov[ebp - 16], ebx//current round

		begin_of_decrypt :
			xor eax, eax
			xor ecx, ecx
			xor ebx, ebx

			mov ecx, [ebp - 16]
			mov ah, [gPasswordHash + ecx * 4]
			mov al, [gPasswordHash + 1 + ecx * 4]

			mov bh, [gPasswordHash + 2 + ecx * 4]
			mov bl, [gPasswordHash + 3 + ecx * 4]

			cmp bx, 0x0
			jne zero_check
			mov bx, 0xFFFF
		zero_check:
			mov[ebp - 8], eax
			mov[ebp - 12], ebx

		

		E :
			xor bl, bl
			xor eax, eax
			mov eax, [esi + edx]
			and eax, 0xFF

			mov bl, [gDecodeTable + eax]
			mov[esi + edx], bl

		C :
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

		A :
			xor al, al
			xor ah, ah

			mov al, [esi + edx]
			mov ah, al
			RCR ah, 1
			RCR al, 1
			mov[esi + edx], al
			
		B :
		xor al, al
			xor bl, bl
			xor ecx, ecx
			mov ecx, 4
			xor ah, ah
			mov al, [esi + edx]
			mov bl, al

			rol al, 4
			mov[esi + edx], al
			
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
			
		hop_step :
				xor eax, eax
				xor ebx, ebx
				xor ecx, ecx
				lea ecx, gkey
				mov ebx, [ebp - 8]

				mov al, [esi + edx]
				xor al, [ecx + ebx]

				mov[esi + edx], al

				xor eax, eax
				xor ecx, ecx

				add ebx, [ebp - 12]
				cmp ebx, 65537
				jb index_check
				sub ebx, 65537

			index_check:
				mov[ebp - 8], ebx

		begin :
			inc edx
			mov ecx, edx
			cmp ecx, [ebp - 4]
			jne E

		begin_newRound :
			xor ecx, ecx
			xor eax, eax
			xor ebx, ebx
			xor edx, edx

			mov ecx, [ebp - 16]
			dec ecx
			mov[ebp - 16], ecx
			cmp ecx, 0x0
			jge begin_of_decrypt

			mov esp, ebp
			pop ebp

	}

	return resulti;
} // decryptData

