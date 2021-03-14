####################################################################################
# Created by:  Hu, Robert
#              ryhu
#              7 August 2018
#
# Assignment:  Lab 4: Leap in MIPS
#              CMPE 012, Computer Systems and Assembly Language
#              UC Santa Cruz, Summer 2018
#
# Description: This program takes in two user inputs, a start year and an end year,
#	       and prints leap for leap years and the year for non-leap years
#
# Notes:       This program is intended to be run from the MARS IDE.
####################################################################################

# REGISTER USAGE
# $t0:           start year, entered by user
# $t1:           end year, entered by user
# $t3, $t4, $t5: conditions to determine if leap year or not

.data
prompt1: .asciiz "Please input the start year: "
prompt2: .asciiz "Please input the end year: "

.text
pr:	la   $a0  prompt1       # load address of prompt1 for syscall
	li   $v0  4             # specify Print String service
	syscall                 # print the prompt string
	
	li   $v0  5             # specify Read Int service
	syscall                 # read int
	move $t0  $v0           # move int from $v0 to $t0
	
	la   $a0 prompt2        # load address of prompt2 for syscall
	li   $v0  4             # specify Print String service
	syscall                 # print the prompt string
	
	li   $v0  5             # specify Read Int service
	syscall	                # read int
	move $t1  $v0           # move int from $v0 to $t0

	# Loop to determine if leap or not
loop:	andi  $t3   $t0   3     # look at 2 lower bits of $t0
	sltiu $t3   $t3   1     # if $s0 is zero, then $t0 is divisible by 4
	rem   $t4   $t0   100   # store $t0 mod 100 in $s1
	slt   $t4   $zero $t4   # if $s1 is not zero, then $t0 is not divisible by 100
	rem   $t5   $t0   400   # store $t0 mod 400 in $s2
	sltiu $t5   $t5   1     # if $s2 is zero, then $t0 is divisible by 400
	and   $t3   $t3   $t4   # $t0 is Leap year if divisible by 4 and not 100
	or    $t3   $t3   $t5   # or is divisble by 400
	
	# Branches to determine what to print
	beqz $t3  not_leap      # branch to not_leap if $s0 is 0
	bnez $t3  leap          # branch to leap if $s0 is not 0
	
	# Finish run
exit:	li   $v0  10
	syscall
	
	
###############################################################
# Subroutine to print leap or year if not leap	
.data
new_line: .asciiz "\n"          # New line character to put each print out on a new line
leap_year: .asciiz "leap"       # If leap year, print leap

.text
not_leap:	la   $a0  ($t0)         # set contents of $a0 to $t0
		li   $v0  1             # specify Print Int service
		syscall                 # print int
		
		la   $a0  new_line      # set contents of $a0 to new_line
		li   $v0  4             # specify Print String service
		syscall                 # print new line
		
		addi  $t0  $t0  1       # increment the start year
		bgt   $t0  $t1  exit    # exit if start year is greater than end year
		j loop                  # jump back to loop
		
leap:		la   $a0  leap_year     # set contents of $a0 to leap_year
		li   $v0  4             # specify Print String service
		syscall                 # print int
		
		la   $a0  new_line      # set contents of $a0 to new_line
		li   $v0  4             # specify Print String service
		syscall                 # print new line
		
		addi  $t0  $t0  1       # increment the start year
		bgt   $t0  $t1  exit    # exit if start year is greater than end year
		j loop                  # jump back to loop
# End of subroutine to print leap or year if not leap	
###############################################################
