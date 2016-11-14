
org  00h

	jmp	LABEL_START

%include       "load.inc"
%include       "pm.inc"

LABEL_GDT:			Descriptor             0,                    0,  0				
LABEL_DESC_FLAT_C:		Descriptor             0,              0fffffh, DA_CR  | DA_32 | DA_LIMIT_4K	
LABEL_DESC_FLAT_RW:		Descriptor             0,              0fffffh, DA_DRW | DA_32 | DA_LIMIT_4K
LABEL_DESC_VIDEO:		Descriptor	 0B8000h,               0ffffh, DA_DRW                         | DA_DPL3

GdtLen		equ	$ - LABEL_GDT
GdtPtr		dw	GdtLen - 1				; 段界限
		dd	loader_phy_addr + LABEL_GDT		; 基地址

ata_identify_structure  times  256 db 0
ata_spt db 0 ; sector per track
ata_head db 0 ; head num
ata_extend db 0
boot_disk db 80h
kernel_size dd  100; per sector
kernel_off dd 10
mcr_number db 0

; GDT 选择子 ----------------------------------------------------------------------------------
SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT + SA_RPL3
; GDT 选择子 ----------------------------------------------------------------------------------


BaseOfStack	equ	9000h

init_ata_param:
	push sp
	mov bp, sp

	xor ax, ax
	xor dx, dx

	mov ah, 0
	mov dl, 80h
	int 13h
	jc .error

	xor ax, ax
	xor dx, dx

	mov ah, 0x08
	mov dl, 80h

	int 13h
	jc .error

	add dh, 1
	mov [loader_phy_addr + ata_head], dh
	
	and cl, 0x3f
	mov [loader_phy_addr + ata_spt], cl

	mov ah, 0x0
	mov dl, [loader_phy_addr + boot_disk] 
	int 13h
	
	mov ah,41h
	mov dl, [loader_phy_addr + boot_disk] 
	mov bx, 55AAh
	int 13h
	jc .no_extend
	mov [loader_phy_addr + ata_extend], cx

.no_extend:
.error:
	pop sp
	ret
	

LABEL_START:			; <--- 从这里开始 *************

	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack



;	mov	bx, 0	
;	mov	di, hw_info_addr
;	push di
;	mov 	al , 1
;	mov     [di], al
;	inc di
;	mov 	al, 0
;	mov     [di], al
;	inc di

;.mem_loop:
;	mov	eax, 0E820h	
;	mov	ecx, 20	
;	mov	edx, 0534D4150h
;	int	15h	
;	jc	.mem_fail
;	add	di, 20
;	inc	byte [loader_phy_addr + mcr_number]	; dwMCRNumber = ARDS 的个数
;	cmp	ebx, 0
;	jne	.mem_loop
;	jmp	.mem_ok

;.mem_fail:
;	jmp $

;.mem_ok:
;	pop di
;	mov al, [loader_phy_addr + mcr_number]
;	mov [di], al
	
	call init_ata_param
	lgdt	[loader_phy_addr + GdtPtr]

	cli

	in	al, 92h
	or	al, 00000010b
	out	92h, al

	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	jmp	dword SelectorFlatC:(loader_phy_addr + LABEL_PM_START)


[SECTION .s32]

ALIGN	32

[BITS	32]

ata_check_address:
	push ebp
	mov ebp, esp

	pop ebp
	ret

ata_read_sectors_lba:
	push ebp
	mov ebp, esp
	pusha
	mov [loader_phy_addr + .sectors], ecx;
	mov [loader_phy_addr + .lba], eax
	mov [loader_phy_addr + .dest], edi

	xor eax, eax
	mov eax, 0x40
	mov edx, 0x1f6
	out dx, al

	
	mov edx, [loader_phy_addr + .sectors]
	mov al, dh
	mov dx, 0x1f2
	out dx, al
	
	mov eax, [loader_phy_addr + .lba]
	shr eax, 24
	mov dx, 0x1f3
	out dx, al

	shr eax, 8
	mov dx, 0x1f4
	out dx, al

	shr eax, 8
	mov dx, 0x1f5
	out dx, al

	mov eax, [loader_phy_addr + .sectors]
	mov dx, 0x1f2
	out dx, al

	mov eax, [loader_phy_addr + .lba]
	mov dx, 0x1f3
	out dx, al

	shr eax, 8
	mov dx, 0x1f4
	out dx, al

	shr eax, 8
	mov dx, 0x1f5
	out dx, al
	

	xor eax, eax
	mov al, 0x24
	mov dx, 0x1f7
	out dx, al

.wait_disk_1:

	xor eax, eax
	in al, dx
	test al, 0x1
	jnz .disk_error
        test al, 0x80
	jz .wait_for_data
	jmp .wait_disk_1

.disk_error:
	jmp $ 

.wait_for_data:

        mov edi, [loader_phy_addr + .dest]
        mov eax, 256
        mov ebx, [loader_phy_addr + .sectors]
        mul ebx
        mov ecx, eax
	mov edx, 0x1F0
	rep insw

	popa
	pop ebp
	ret

.sectors dd 0
.lba dd 0
.dest dd 0
;eax lba for 0 - max-1
;ecx sectors to read
;edi buffer

ata_read_sectors_legacy:
	push ebp
	mov ebp, esp
	pusha
	mov [loader_phy_addr + .sectors], ecx;
	mov [loader_phy_addr + .lba], eax
	mov [loader_phy_addr + .dest], edi

	call ata_lba_to_chs
	mov [loader_phy_addr + .chs], ebx
	mov [loader_phy_addr + .sectors], ebx

	xor eax, eax
	mov al, bh
	or al, 0xA0
	mov dx, 0x1f6
	out dx, al

	mov edx, 0x1f2
	mov eax, [loader_phy_addr + .sectors]
	out dx, al
	
	mov edx, 0x1f3
	mov eax, [loader_phy_addr + .chs]
	out dx, al

	mov edx, 0x1f4
	mov eax, [loader_phy_addr + .chs]
	shr eax, 16
	out dx, al

	mov edx, 0x1f5
	mov eax, [loader_phy_addr + .chs]
	shr eax, 24
	out dx, al
	
	mov edx,0x1f7
	mov eax, 0x20
	out dx, al
	
;
	;xor ebx, ebx
        ;add ebx, 0xff                    ; give the disk 1 second to get ready

.wait_for_disk:
	;dec ebx
        ;cmp ebx, 0
	;jnz .wait_for_disk
        mov edx, 0x1F7
        in al, dx
	test al, 0x1
	jnz .disk_error
        test al, 0x80
	jz .wait_for_data
        jmp .wait_for_disk
	
.disk_error:
	jmp $

.wait_for_data:

        mov edi, [loader_phy_addr + .dest]
        mov eax, 256
        mov ebx, [loader_phy_addr + .sectors]
        mul ebx
        mov ecx, eax
        mov edx, 0x1F0
	rep insw

        popa
        mov eax, 0
        ret

.sectors db 0
.chs dd 0
.lba dd 0
.dest dd 0

;eax lab
ata_lba_to_chs:
	push ebp
	mov ebp, esp

	push edx
	push ebx
	push eax

        movzx ebx, byte[loader_phy_addr + ata_spt]
        mov edx, 0
        div ebx
        mov [loader_phy_addr + .tmp], eax

        mov eax, [loader_phy_addr + .lba]
        movzx ebx, byte[loader_phy_addr + ata_spt]
        mov edx, 0
        div ebx
        add edx, 1
        mov [loader_phy_addr + .sector], dl

        mov eax, [loader_phy_addr + .tmp]
        movzx ebx, byte[loader_phy_addr + ata_head]
        mov edx, 0
        div ebx
        mov [loader_phy_addr + .head], dl

        mov eax, [loader_phy_addr + .tmp]
        movzx ebx, byte[loader_phy_addr + ata_head]
        mov edx, 0
        div ebx
        mov [loader_phy_addr + .cylinder], ax

        mov ebx, 0
        mov bx, [loader_phy_addr + .cylinder]
        shl ebx, 16
        mov bh, [loader_phy_addr + .head]
        mov bl, [loader_phy_addr + .sector]
	mov [loader_phy_addr + .res], ebx
	
	pop eax
	pop ebx
	pop edx
	mov ebx, [loader_phy_addr + .res]

	pop ebp
	ret 

.cylinder                       dw 0
.head                           db 0
.sector                         db 0
.lba                            dd 0
.tmp                            dd 0
.res 				dd 0

ata_reset:
	push ebp
	mov ebp, esp
	
	pop ebp
	ret

ata_indentity:

	push ebp
	mov ebp, esp

	mov dx, 0x1f6
	out dx, al

        mov eax, 0
        mov dx, 0x1F2
        out dx, al

        mov eax, 0
        mov dx, 0x1F3
        out dx, al

        mov eax, 0
        mov dx, 0x1F4
        out dx, al

        mov eax, 0
        mov dx, 0x1F5
        out dx, al

	mov al, 0xEC            ; ATA identify command
        mov dx, 0x1F7
        out dx, al

        mov dx, 0x1F7
        in al, dx
        cmp al, 0
	je .no_disk

;0x1000
.wait_disk:
	mov dx, 0x1F7

	in al, dx

        test al, 0x80
        jz .cleared
        jmp .wait_disk

.cleared:	

        mov dx, 0x1F0
        mov edi, ata_identify_structure
        mov ecx, 256                            ; get 256 words from the disk
        rep insw
	jmp .out_check

.no_disk:
	jmp $

.out_check:
	pop ebp
	ret 

LABEL_PM_START:
	mov eax, SelectorFlatRW
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	xor eax, eax

	mov ah, 0xa0
	call ata_indentity
	call ata_reset
	jmp load_kernel
	
load_kernel:
	mov ecx, [loader_phy_addr + kernel_size]
	mov edi, kernel_load_addr

	mov eax, [loader_phy_addr + kernel_off]
	; we need to caculate the offset of sector from 0 
	dec eax

	call ata_read_sectors_lba

	jmp parse_kernel

parse_kernel:

	mov cx, [kernel_load_addr + elf_phnum]
	mov esi, [kernel_load_addr + elf_phoff]
	add esi, kernel_load_addr
	
.begin:
	mov eax, [esi]
	cmp eax,0
	jz .next

	mov eax, [esi + ph_size]
	push eax

	mov eax, [esi + ph_file_off]
	add eax, kernel_load_addr
	push eax

	mov eax, [esi + ph_vaddr]
	sub eax, kernel_offset
	push eax
	
	call mem_cpy

.next:
	add esi, elf_ph_size
	dec ecx
	jnz .begin

	jmp .kernel_begin


.kernel_begin:

	jmp SelectorFlatC:0x100000

	
	
;edi
;esi
;size
mem_cpy:
	push ebp
	mov ebp, esp

	push edi
	push esi
	push ecx

	mov edi, [ebp + 8]   
	mov esi, [ebp + 12]   
	mov ecx, [ebp + 16]   
	
.m1:
	cmp ecx, 0
	je .m2

	mov al,[ds:esi]
	inc esi 

	mov [es:edi], al
	inc edi
	
	dec ecx
	jnz  .m1

.m2:
	pop ecx
	pop esi
	pop edi

	pop ebp

	ret
	
