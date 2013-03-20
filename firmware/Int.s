;================================================================== 
;	file	: Int.s 
;================================================================== 

	.file	"Int.c"


;	GLOBAL __.INT0				; Watch
;	GLOBAL __.INT3				; Timer3
;	GLOBAL __.INT4				; Timer2
;	GLOBAL __.INT5				; Timer1
;	GLOBAL __.INT6				; Timer0
;	GLOBAL __.INT8				; Uart TX
;	GLOBAL __.INT9				; Uart RX
	GLOBAL __.INT9			; Uart Rx/Tx
;	GLOBAL __.INT13				; Ext0

;;================================================================== 
;;	Watch Interrupt  by 1minute Interval   
;;================================================================== 
;
;	.sect	.text
;	.globl Watch_Int
;	.type	Watch_Int,@function
;	.interrupt Watch_Int
;
;__.INT0:
;
;Watch_Int:					; 
;
;;	clr1	0FBh.4			;
;;	
;;	set1	_rTime_flag.0		; f_Minute set for RTC  
;;	
;;	reti					;
;;
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_WT_Int_Handler		; 4ms interval 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;
;
;;================================================================== 
;;	Timer0 Interrupt  by 4 ms interval 
;;================================================================== 
;
;	.sect	.text
;	.globl T0_Int
;	.type	T0_Int,@function
;	.interrupt T0_Int
;
;__.INT6:
;
;T0_Int:
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_T0_Int_Handler		; 4ms interval 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;
;
;;================================================================== 
;;	Timer1 Interrupt  by 4 ms interval 
;;================================================================== 
;
;	.sect	.text
;	.globl T1_Int
;	.type	T1_Int,@function
;	.interrupt T1_Int
;
;__.INT5:
;
;T1_Int:
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_T1_Int_Handler		; 4ms interval 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;
;;================================================================== 
;;	Timer3 Interrupt  by 4 ms interval 
;;================================================================== 
;
;	.sect	.text
;	.globl T3_Int
;	.type	T3_Int,@function
;	.interrupt T3_Int
;
;__.INT3:
;
;T3_Int:
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_T3_Int_Handler		; 4ms interval 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;			
;;================================================================== 
;;	Timer2 Interrupt  by 25ms Interval   
;;================================================================== 
;
;	.sect	.text
;	.globl T2_Int
;	.type	T2_Int,@function
;	.interrupt T2_Int
;
;__.INT4:
;
;T2_Int:
;	push	A				;
;	push	X				;
;	push	Y				;
;	
;	call	!_T2_Int_Handler		; 25ms interval for Buzzer 
;
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;
;
;;
;================================================================== 
;	Uart  Interrupt  	 
;================================================================== 

	.sect	.text
	.globl Uart_Int
	.type	Uart_Int,@function
	.interrupt Uart_Int

__.INT9:

Uart_Int:
	push	A				;
	push	X				;	
	push	Y				;	

	call	!_uartIntHandler	;
	
	pop	Y				;
	pop	X				;
	pop	A				;
	
	reti					;
;================================================================== 
;	Uart Interrupt  
;================================================================== 
;
;	.sect	.text
;	.globl TX_Int
;	.type	TX_Int,@function
;	.interrupt TX_Int
;
;__.INT8:
;
;TX_Int:
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_TX_Int_Handler		; Uart Transmit 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;
;-------------------------------------------------------------------		
;	.sect	.text
;	.globl RX_Int
;	.type	RX_Int,@function
;	.interrupt RX_Int
;
;__.INT9:
;
;RX_Int:
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_RX_Int_Handler		; Uart Receive 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;

;================================================================== 
;	Ext0 Interrupt for IR receive 
;================================================================== 

;	.sect	.text
;	.globl Ext0_Int
;	.type	Ext0_Int,@function
;	.interrupt Ext0_Int
;
;__.INT13:
;
;Ext0_Int:
;	push	A				;
;	push	X				;	
;	push	Y				;	
;
;	call	!_Ext0_Int_Handler	; Remote Key Receive 
;	
;	pop	Y				;
;	pop	X				;
;	pop	A				;
;	
;	reti					;
		

;================================================================ 
;	Appendix : MC80F7708 Interrupt Vector Table
;================================================================ 
;      org   0FFE0h
;      ;
;	dw    Watch_Int		;INT0	 WT/WTD 	FFE0
;	dw    No_Interrupt	;INT1	 BIT      	FFE2
;	dw    No_Interrupt	;INT2	 ADC/I2C   	FFE4
;	dw    No_Interrupt 	;INT3	    		FFE6
;	dw    No_Interrupt	;INT4	 Timer3 	FFE8
;	dw    T2_Int		;INT5	 Timer2 	FFEA
;	dw    No_Interrupt	;INT6	 Timer1 	FFEC
;	dw    T0_Int		;INT7	 Timer0 	FFEE
;	dw    No_Interrupt	;INT8	 		FFF0
;	dw    Uart_Int      	;INT9	 Uart  	FFF2
;	dw    No_Interrupt      ;INT10 Ext3		FFF4
;	dw    No_Interrupt	;INT11 Ext2 	FFF6
;	dw    No_Interrupt	;INT12 Ext1		FFF8
;	dw    Ext0_Int		;INT13 Ext0 	FFFA
;	dw    No_Interrupt  	;INT14 		FFFC
;	dw    Program_start 	;INT15 Reset  	FFFE
;
;==================================================================*/ 
;/*	the end of Int.s								 */			

