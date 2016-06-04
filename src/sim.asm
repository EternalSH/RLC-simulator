section .data
	zero:	dq		 0.0
	ndt:	dq		-0.001
	pdt:	dq		 0.001
	qdef:	dq		 1.0

section .text
	global sim

sim:
	push	RBP
	mov	RBP,		RSP

	;; - TABLE OF CONTENTS -
	;;
	;; - XMMS:
	;;
	;; Uppercase names - constants
	;; Lowercase names - variables
	;;
	;; XMM0  - R 		resistance of RLC circuit
	;; XMM1  - L 		inductance of RLC circuit
	;; XMM2  - C		capacity of RLC circuit
	;; XMM3  - -dt / (L*C)
	;; XMM4	 - q		charge in capacitor
	;; XMM5	 - i		current in circuit
	;; XMM6  - +dt		time step in simulation (positive number)
	;; XMM7	 - -dt		time step in simulation (negative number)
	;; XMM8  - di		
	;; XMM9  - Uc		calculated value of voltage in capacitor
	;; XMM10 - dq	
	;;
	;; XMM11 - R*C
	;; xmm12 - L*C
	
	;; - LOOPS:
	;;
	;;			if N means number of generated numbers for
	;;			  allegro libray to display and dt means +dt:
	;;
	;; main_loop:		iterates through values to calculate
	;;	RAX		  screen resolution is too low to display
	;;			  all calculated values while too big dt
	;;			  causes huge approximation error
	;; skip_result		to match both screen resolution and minimize
	;;	RCX		  approximation error this application performs
	;;			  N/dt approximation to calculate one point 
	;;			  of plot (within skip_result loop)
	;; - TOC -
	
	;; XMM0 - R
	;; XMM1 - L
	;; XMM2 - C
	
	;; XMM4 - q(t)
	movsd	XMM4,		[qdef]

	;; XMM5 - i(t)
	movsd	XMM5,		[zero]

	;; XMM6 - +dt
	movsd	XMM6,		[pdt]

	;; XMM7 - -dt
	movsd	XMM7,		[ndt]

	;; XMM8 - temporary di
	movsd	XMM8,		[zero]
	
	;; XMM10 - temporary dq
	movsd	XMM10,		[zero]	
	
	;; RAX - iterator main_loop 
	xor	RAX,		RAX
	
	;; XMM11 - R*C
	movsd	XMM11,		XMM0
	mulsd	XMM11,		XMM2
	
	;; XMM12 - L*C
	movsd	XMM12,		XMM1
	mulsd	XMM12,		XMM2
	
main_loop:	
	;; XMM9 - Uc(t)
	movsd	XMM9,		XMM4		;; Uc = q
	divsd	XMM9,		XMM2		;; Uc = q/C
	movsd	[RDI+8*RAX],	XMM9		;; result[i] = Uc

	mov	RCX,		20000		;; load loop
	
skip_result:
	movsd	XMM8,		XMM5		;; di = i
	movsd	XMM3,		XMM7		;; -dt
	mulsd	XMM8,		XMM11		;; di = i*R*C
	divsd	XMM3,		XMM12		;; -dt / (L*C)
	addsd	XMM8,		XMM4		;; di = i*R*C+q
	mulsd	XMM8,		XMM3		;; di = -dt * (R*C + q) / (L*C)
	
	addsd	XMM5,		XMM8		;; i += di

	movsd	XMM10,		XMM5		;; dq = i
	mulsd	XMM10,		XMM6		;; dq = i * +dt

	addsd	XMM4,		XMM10		;; q += dq

	loop	skip_result

	add	RAX,		1		;; ++ iterator main_loop	
	cmp	RAX,		600		;; if(iterator > 600)
	jle	main_loop			;;   break;

	pop	RBP
	ret
