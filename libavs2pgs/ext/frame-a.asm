;*****************************************************************************
;* frame-a.asm: x86 SSE2 frame operations
;*****************************************************************************
;* This program is free software: you can redistribute it and/or modify
;* it under the terms of the GNU General Public License as published by
;* the Free Software Foundation, either version 3 of the License, or
;* (at your option) any later version.
;*
;* This program is distributed in the hope that it will be useful,
;* but WITHOUT ANY WARRANTY; without even the implied warranty of
;* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;* GNU General Public License for more details.
;*
;* You should have received a copy of the GNU General Public License
;* along with this program.  If not, see <http://www.gnu.org/licenses/>.
;*****************************************************************************
;* Imported from avs2bdnxml 2.08 (under GPLv3)
;* Copyright (C) 2008-2013 Arne Bochem <avs2bdnxml at ps-auxw de>
;* and last modified by Giton<adm@subelf.net>
;*****************************************************************************

%include "x86inc.asm"

SECTION_RODATA

zero:    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
a_mask:  db 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff
full:    db 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
ag_mask: db 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff
rb_mask: db 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00


SECTION .text

INIT_XMM
cglobal is_identical_sse2, 3,5,6
	lea r3, [r1+r0]
	mov r4, 0
	mova m0, [zero]
	mova m1, [a_mask]
	mova m2, [full]
.loop:
	test r4, r4
	jnz .diff
	cmp r1, r3
	jge .ident

	mova m3, [r1]
	mova m4, m3
	pand m4, m1
	mova m5, [r2]
	pcmpeqd m4, m0
	pxor m4, m2
	pand m3, m4
	;mova [r1], m3
	psadbw m3, m5
	movd r4, m3

	add r1, 16
	add r2, 16
	jmp .loop
.diff:
	mov eax, 0
	jmp .end
.ident:
	mov eax, 1
.end:
	RET

INIT_XMM
cglobal is_empty_sse2, 2,4,3
	lea r2, [r1+r0]
	mov r3, 0
	mova m0, [zero]
	mova m1, [a_mask]
.loop:
	test r3, r3
	jnz .not
	cmp r1, r2
	jge .empty

	mova m2, [r1]
	pand m2, m1
	psadbw m2, m0
	movd r3, m2

	add r1, 16
	jmp .loop
.not:
	mov eax, 0
	jmp .end
.empty:
	mov eax, 1
.end:
	RET

INIT_XMM
cglobal zero_transparent_sse2, 2,3,5
	lea r2, [r1+r0]
	mova m0, [zero]
	mova m1, [a_mask]
	mova m2, [full]
.loop:
	cmp r1, r2
	jge .end

	mova m3, [r1]
	mova m4, m3
	pand m4, m1
	pcmpeqd m4, m0
	pxor m4, m2
	pand m3, m4
	mova [r1], m3

	add r1, 16
	jmp .loop
.end:
	RET

INIT_XMM
cglobal swap_rb_sse2, 3,4,5
	lea r3, [r1+r0]
	mova m0, [ag_mask]
	mova m1, [rb_mask]
.loop:
	cmp r1, r3
	jge .end

	mova m2, [r1]
	mova m3, m2
	mova m4, m2
	psrld m3, 16
	pslld m2, 16
	por m2, m3
	pand m4, m0
	pand m2, m1
	por m2, m4
	mova [r2], m2

	add r1, 16
	add r2, 16
	jmp .loop
.end:
	RET
