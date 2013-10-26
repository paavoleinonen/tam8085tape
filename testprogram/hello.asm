;
; Display test program for TAM8085
; Outputs "HELLO " to 7-segment displays.
;
; Paavo Leinonen <paavo.leinonen@iki.fi>, 2012
;

; Expects already initialized 8279

; Intel 8279 keyboard/display controller
DSPDATA: equ 5000h
DSPCTL: equ 5001h

	LXI H, DSPCTL
	MVI M, 090h	; Start from 0 with autoincrement
	DCX H		; select DSPDATA
	MVI M, 067h	; 'H'
	MVI M, 097h	; 'E'
	MVI M, 083h	; 'L'
	MVI M, 083h	; 'L'
	MVI M, 0F3h	; 'O'
	MOV M, L	; ' '

	HLT
