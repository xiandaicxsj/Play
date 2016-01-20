;used to memcpy
global memcpy;[ds:si,es:di,int n]
section .text
memcpy:
	push ebp
	mov ebp,esp
	push esi
	push edi
	push ecx
	mov esi,[ebp+8]
	mov edi,[ebp+12]
	mov ecx,[ebp+16]
mem.1:
	cmp ecx,0
	jz mem.2	
	mov al,[ds:esi]
	mov [es:edi],al
	dec ecx
	inc esi
	inc edi
	jmp mem.1
	pop ecx
	pop edi
	pop esi
	leave
	ret

