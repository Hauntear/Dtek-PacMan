  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.


  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	  \reg,0($sp)
.endm

.macro	POP reg
	lw	  \reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
  .data
  .align 2
  .text
  .globl delay
  .globl time2string
  .globl enable_interrupt

hexasc:
	andi	$t0,$a0,0xf
	
	li	$t1,1
	slti	$t2,$t0,10
	 
	beq	$t1,$t2,let
	nop 
	
	addi	$t0,$t0,7
	
	let:
		addi 	$t0,$t0,0x30
		move 	$v0,$t0
	
	jr	$ra
	nop
	
delay:
	li	$t0,6500
	mul	$t0,$t0,$a0
	li	$t1,1
	move	$t2,$zero			
	
	slt	$t7,$t0,$t1
	beq	$t1,$t7,skip
	nop
	
	floop:
		addi	$t2,$t2,1
		slt 	$t3,$t2,$t0
		beq	$t3,$t1,floop
		nop
		
	skip:
		jr	$ra
		nop
	
time2string:
	andi	$t1,$a1,0x00ff
	
	beq	$t1,$zero,yeet
	nop
	
	PUSH $ra
	move $t7,$a0
	PUSH $a0
	
	move	$a0,$a1
	jal	hexasc
	nop
	PUSH $v0
	
	srl	$a0,$a0,4	# make loop mebe
	jal	hexasc
	nop
	PUSH $v0
	
	srl	$a0,$a0,4
	jal	hexasc
	nop
	PUSH $v0
	
	srl	$a0,$a0,4
	jal	hexasc
	nop
	PUSH $v0
	
	move	$t1,$zero
	POP $t0
	or	$t1,$t1,$t0
	
	POP $t0
	sll	$t0,$t0,8
	add	$t1,$t1,$t0
	
	li	$t2,0x3a0000
	or	$t1,$t1,$t2
	
	
	POP $t0
	sll	$t0,$t0,24
	or	$t1,$t1,$t0
	
	sw	$t1,0($t7) #first word
	
	move	$t1,$zero
	POP $t0
	or	$t1,$t0,$zero
	
	ori	$t1,$t1,0x0000
	
	sw	$t0,4($t7)
	
	POP $a0
	
	POP $ra
	jr	$ra
	nop
	
	yeet:
		li	$t0,0x474e4944
		sw	$t0,0($a0)
		sw	$zero,4($a0)
		jr	$ra
		nop

enable_interrupt:
	ei
	jr	$ra
	nop
	