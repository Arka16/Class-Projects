##########################################################################
# Created by:  Pal, Arka
#              arpal
#              20 February 2020
#
# Assignment:  Lab 4: Syntax Checker
#              CSE 12, Computer Systems and Assembly Language
#              UC Santa Cruz, Winter 2020
# 
# Description: This program prints ‘Hello world.’ to the screen.
# 
# Notes:       This program is intended to be run from the MARS IDE.
##########################################################################
#RESGISTER USAGE
#$t0:address of file name
#$t1:actual the file name 
#$t2:first byte of file name/each byte of file name
#$t3:number of opening braces/brace matches
#$t4:length of the file name
#$t5:contents of buffer
#$t6:Each byte of contents in buffer
#$t7:index of opening brace
#$t8: copy of buffer for clearing
#$t9:index of closing brace
#$s0:file descriptor
#$s1:Initial address of the stack pointer
#$s3:Closing brace
#$s4:Character after $t5
#s5:number of bytes in buffer. Resets to 0 when it equals to 128
#s6: counter for loop when buffer is being emptied
#s7: used to reinitialize $t7 and $t9 when file greater than 128 bytes are read
#$a1: address of file name/flag mode/address of buffer
#$a2:mode/maximum characters read from buffer
##########################################################################
#Pseudocode
#move $a1, the address of the file name to $t0
#Set $t1 to the file name and $t2 to the first byte of the file name
#if the first character is not a letter
#	print invalid input
#       end program
#For each character of file name
#	if the character is not a letter, number, underscore, or period
#		print invalid input
#               end program
#       increment $t4 by 1
#if the length of the input($t4) is greater than 20
#	print invalid input
#	end program
#Else
#	open file
#	read file
#	for each character in string inside the file
#		if the character is an opening brace
#                       decrement $sp by 4
#			push character index to stack
#			decrement $sp by 4
#                       continue loop
#			push character to stack
#		if the character is a closing brace
#                      pop opening brace index from stack
#                      increment $sp by 4
#		       pop opening from stack
#		       increment #sp by 4
#			if character after the closing brace is an opening brace
#				make $t7 equal to $t9
#               if character after the closing brace is an opening brace
#				make $t7 equal to $t9
#               if the popped element doesn't correspond to the closing brace(ex if closing brace is curly and opening brace is parenthesis)
#			print brace mismatch error
#			close program
#       if 128 characters are read
#		empty out buffer
#		reinitialize $t7 and $t9
#		read next 128 bytes
#               continue loop until $s5 is less than 128
#       Close file
#       if stack is not empty
#		print stack remains error
#       else
#       	print success message
#      End program
##########################################################################
#Assembly Code
.data
buffer:.space 128
invalid: .asciiz "ERROR: Invalid program argument."
mismatch: .asciiz "ERROR - There is a brace mismatch: "
index: .asciiz " at index "
success: .asciiz "SUCCESS: There are "
success2:.asciiz  " pairs of braces." 
newline: .asciiz "\n"
Enter: .asciiz "You entered the file: "
Stack: .asciiz "ERROR - Brace(s) still on stack: "
space: .asciiz " "

.text
li $v0, 4
la $a0, Enter
syscall

li $v0, 4
la $a0, newline
syscall

#Prints file name
move $t0, $a1#$t0 has the address of the file name
li $v0,4
lw $a0, 0($t0)
syscall


li $v0, 4
la $a0, newline
syscall


lw $t1, 0($t0)#$t1 is the actual the file name 
lb $t2, 0($t1)#t2 stores the first character of the filename

#Checks for Invalid file names by first checking if the first character of the filename starts by a letter
blt $t2,65,Invalid_1
NOP
ble $t2,90, Loop1
NOP
j Next

Next:
         NOP
         blt, $t2, 97, Invalid_1
         NOP
         bgt, $t2, 123, Invalid_1
         NOP
         j Loop1
         

Invalid_1:
	NOP
	li $v0, 4
        la $a0, newline
        syscall
	li $v0, 4
        la $a0, invalid
        syscall
        j Exit
        addi $t4, $t4, 0
Loop1:
	NOP
	lb $t2, ($t1)#loops through file name
	beq  $t2, $zero, stop #if $t2 is null all characters of the file name have been checked
	NOP
        #Checks if the file name contains invalid characters. If the character is Valid, the next character is observed
	beq $t2, 46, Inc
	NOP
	blt $t2, 48,Invalid_2
	NOP
	ble $t2, 57, Inc
	blt $t2,65,Invalid_2
        NOP
        ble $t2,90, Inc
        NOP
        beq $t2, 95, Inc
        NOP
        blt, $t2, 97, Invalid_2
        NOP
        bgt, $t2, 122, Invalid_2
        NOP
Inc:    
        NOP
	addi $t1, $t1, 1
	addi $t4, $t4, 1
	j Loop1

Invalid_2:
	NOP
	li $v0, 4
        la $a0, newline
        syscall
	li $v0, 4
        la $a0, invalid
        syscall
        j Exit	

stop:   NOP
	ble $t4, 20, skip
	NOP
	li $v0, 4
	la $a0, newline
	syscall 
	li $v0, 4
	la $a0, invalid
	syscall 
	j Exit

skip:   NOP
        li $v0, 4
        la $a0, newline
        syscall


#Opens file
Open:   NOP
        li $v0, 13
        lw $a0, 0($t0)#address of input
        li $a1, 0#flags
        li $a2, 0
        syscall
        move $s0, $v0
#Reads file
Set:    NOP
        addi $t3, $t3,0
        addi $t7, $t7,0
        addi $t9, $t9, 0
        move $s1, $sp #saves the initial address of the stack pointer to $s1

Read:   NOP
        li $v0,14
        move $a0, $s0
        la $a1, buffer
        li $a2, 128
        syscall
        
        la $t5, buffer
        la $t8, buffer

        
#Loop to check for brace mismatches. Pushes opening braces to stack when $t6 is an opening brace and pops from stack
#when $t6 is a closing brace
loop2:   
	NOP
	lb $t6, ($t5)
	beq $t6, $zero, Check2
	NOP
	#Pushes opening brace and its index to stack if opening brace is read, Pops opening brace and its index if closing brace is read. 
	#If the character read is not a brace, doesn't push or pop
	beq $t6, 40, Push
	NOP
	beq $t6, 91, Push
	NOP
	beq $t6, 123, Push
	NOP
	beq $t6, 41, Pop1
	NOP
	beq $t6, 93, Pop2
	NOP
	beq $t6, 125, Pop3
	NOP
	j Check
#Pushes character to Stack
Push:
        NOP
        subi $sp, $sp, 4#stack pointer is subtracted by 4 so it can make space to push element
	sw $t7, ($sp)#pushes index of opening brace to stack
	subi $sp, $sp, 4
	sw $t6, ($sp)#pushes opening brace to stack
	addi $t3, $t3, 1#$t3 is increment by one to keep track of the number of pairs
	j Increment
#Pops character from Stack Different Pops for Different closing braces
Pop1:
        NOP
        addi $s3, $zero, 41#stores closing parenthesis to $s3
        lw $t6, ($sp)#gets opening parenthesis from stack 
        addi $sp, $sp, 4 #increments stack pointer by 4 so it can get the next stack element
        lw $t7, ($sp)#gets index of opening parenthesis
        bne $t6, 40, error1#if opening brace popped is not an opening parenthesis, print mismatch error
        NOP
        addi $sp, $sp, 4
        j Check#Check if next element is an opening brace
      
        
        
Pop2:
        NOP
        addi $s3, $zero, 93
        lw $t6, ($sp)
        addi $sp, $sp, 4
        lw $t7, ($sp)
        bne $t6, 91, error1
        NOP
        addi $sp, $sp, 4
        j Check
Pop3:
        NOP
        addi $s3, $zero, 125
        lw $t6, ($sp)
        addi $sp, $sp, 4
        lw $t7, ($sp)
        bne $t6, 123, error1
        NOP
        addi $sp, $sp, 4
        j Check
#Increases the index of the loop and gets the next character      	
Check:	
	NOP
	lb $s4, 1($t5)
	beq $s4, 40, ReInit
	NOP
	beq, $s4, 91, ReInit
	NOP
	beq, $s4, 123, ReInit
	NOP 
	j Increment

ReInit:	NOP
        #Initializes $t7 to $t9 so the correct index of the next opening brace can be stored
        addi $t7, $t9, 0

Increment:
        NOP
	addi $t5,$t5,1#goes to the next byte of the buffer
	addi $t7,$t7,1#increments the index of the opening brace by 1
	addi $t9, $t9, 1#increments the index of the closing brace by 1
	addi $s5, $s5, 1#$s5 is the number of bytes of each file read. Is incrementing by one to count the number of bytes
	j loop2#continues loop
#Closes file once all characters in the file are examined	
Check2:
        NOP
        #checks if more than 128 bytes are read. If there are less than 128 bytes, the file is closed
        ble $v0, 0, CloseFile
        NOP
        addi $s7, $s7, 1
        #$t7 and $t9 are reinitialized
        mul $t7, $s7, 128
        mul $t9, $s7, 128
        
        addi $s5, $zero, 0
        addi $s6, $zero, 0

Lo:     NOP
        bgt $s6, 127, Read
        NOP
        #buffer is emptied
        sb $zero, ($t8)
        addi $t8, $t8,1
        addi $s6, $s6, 1
        j Lo
                   

#Closes the file once all characters in the file are read
CloseFile:
        NOP
        li $v0, 16
        syscall
        bne $s1, $sp, error3
        NOP
        j Success
#Prints mismatch error
error1:
	NOP
	li $v0, 4
	la $a0, mismatch
	syscall
	bne $t6, 40, A
	NOP
	j D
A:      NOP
	bne $t6, 91, B
	NOP
	j D
B:      NOP
	bne $t6, 123, skip2
	NOP
	j D
	#prints opening brace	
D:      NOP
        li $v0, 11
	move $a0,$t6
	syscall
	li $v0, 4
	la $a0, index
	syscall
	#prints index of opening brace
	li $v0, 1
	lw $a0, ($sp)
	#move $a0, $t7
	syscall
	li $v0, 4
	la $a0, space
	syscall
skip2:  NOP
        #prints closing brace
	li $v0, 11
	move $a0, $s3
	syscall
	li $v0, 4
	la $a0, index
	syscall
	#prints index of closing brace
	li $v0, 1
	move $a0, $t9
	syscall
	j Exit
error3:
	NOP
	li $v0, 4
	la $a0, Stack
	syscall
S:      NOP
        #Loop for stack stops when the $sp equals as it's initial state
        beq $sp, $s1, Exit
        NOP
        #prints the stack of braces in reverse order by popping opening brace from stack
	li $v0, 11
	lw $a0, ($sp)
	syscall 
	#$sp is added by 8 inorder to skip popping the index of the brace 
	addi $sp, $sp, 8
	j S
#Prints Success message
Success:
        NOP
	li $v0, 4
	la $a0, success
	syscall
	#Prints number of brace matches
        li $v0, 1
	move $a0, $t3
	syscall
	li $v0, 4
	la $a0, success2
	syscall
        
#Finishes program
Exit:
        NOP
        li $v0, 4
	la $a0, newline
	syscall
        li $v0 10
        syscall








