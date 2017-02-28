

	org  07c00h			; Boot 状态, Bios 将把 Boot Sector 加载到 0:7C00 处并开始执行
BaseOfStack		equ	07c00h	; Boot状态下堆栈基地址(栈底, 从这个位置向低地址生长)

;================================================================================================

	jmp short LABEL_START		; Start to boot.
	nop				; 这个 nop 不可少

; 下面是 FAT12 磁盘的头, 之所以包含它是因为下面用到了磁盘的一些信息

LABEL_START:	
	cli
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack

	; 清屏
	mov	ax, 0600h		; AH = 6,  AL = 0h
	mov	bx, 0700h		; 黑底白字(BL = 07h)
	mov	cx, 0			; 左上角: (0, 0)
	mov	dx, 0184fh		; 右下角: (80, 50)
	int	10h			; int 10h

	mov	dh, 0			; "Booting  "
	call	DispStr			; 显示字符串

	
jmp .check_extend
;check exten
.check_extend:
	mov ah, 41h
	mov bx, 55AAH
	mov dl, 80h
	int 13h
	jnc .exten_ok
	jmp $
.exten_ok:
	;cmp byte[boot1.boot], 0x80
	;je .boot_1
	
	xor ax,ax
	xor bx,bx
	xor cx,cx
	xor dx,dx

	mov bx, 0x9000
	mov ah,	2h
	mov al,	3h
	mov ch,	0h
	mov cl,	2h
	mov dh,	0h
	mov dl,	80h
	int 13h
	jc .error_legacy

	jmp 0:0x9000


.error_legacy
	mov ax, 0xffff
	jmp $
	

.boot_1:
	mov si, boot1 
	call read_sectors
	jmp .loader

.boot_2:
	jmp $

.boot_3:
	jmp $

.loader
	jmp 0:0x80100

;ecx number of the sector:w
;eax llba
;edx hlba
;ebx segmet:offset
	
read_sectors:
	push	bp
	mov	bp, sp
	push esi
	
	;add esi , 2
	;mov eax, dword [esi] 
	;mov dword [ap.llba], eax
	;add esi, 4
	;mov edx, dword [esi]
	;mov dword [ap.hlba], edx

;clear eax edx is needed
	xor eax, eax
	xor edx, edx

	mov si, ap
	mov ah, 0x42
	mov dl, 0x0
	int 13
	jc .error

	jmp $
	pop esi
	pop	bp
	ret
	.error
	mov eax, 0xffffffff
	jmp $


LoaderFileName         db      "LOADER  BIN", 0        ; LOADER.BIN 之文件名
MessageLength          equ     9
BootMessage:           db      "Booting  "; 9字节, 不够则用空格补齐. 序号 0
Message1               db      "Ready.   "; 9字节, 不够则用空格补齐. 序号 1
Message2               db      "No LOADER"; 9字节, 不够则用空格补齐. 序号 2

ap:
	.sizeofPacket	db	0x10
	.packed	db	0
	.numSector	dw	3
	.offset	dw	0x9000
	.selector	dw	0
	.llba 	dd 	1
	.hlba	dd	0	 

boot1:
	.boot db 0x80
	.begin_chs db 1
	.end_chs db 59
	.lba dd 2
	.size dd 600000
;----------------------------------------------------------------------------
; 函数名: DispStr
;----------------------------------------------------------------------------
; 作用:
;	显示一个字符串, 函数开始时 dh 中应该是字符串序号(0-based)
DispStr:
	mov	ax, MessageLength
	mul	dh
	add	ax, BootMessage
	mov	bp, ax			; ┓
	mov	ax, ds			; ┣ ES:BP = 串地址
	mov	es, ax			; ┛
	mov	cx, MessageLength	; CX = 串长度
	mov	ax, 01301h		; AH = 13,  AL = 01h
	mov	bx, 0007h		; 页号为0(BH = 0) 黑底白字(BL = 07h)
	mov	dl, 0
	int	10h			; int 10h
	ret


times 	510-($-$$)	db	0	; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 	0xaa55				; 结束标志
