####################################################################################
# Created by:  Hu, Robert
#              ryhu
#              9 August 2018
#
# Assignment:  Lab 5: ASCII Binary to Decimal
#              CMPE 012, Computer Systems and Assembly Language
#              UC Santa Cruz, Summer 2018
#
# Description: This program takes in a binary input and converts and prints it to
#              decimal, hex, and perhaps morse code
#
# Notes:       This program is intended to be run from the MARS IDE.
####################################################################################

##################################
# PSEUDO CODE
#
# Store user input and print it out
# Store input into array
# Loop to process array and convert
# Pad value with the most significant bit
# Loop to convert bytes of value to hex and print
# Increment counter and reset at end of loop
# Loop to convert value to decimal and print
# Increment counter and reset at end of loop
# Take decimal digits and convert to morse
# Increment counter
##################################

##################################
# REGISTER USAGE
#
# $t0: address of original argument input
# $t1: byte placement holder or conversion int holder
# $t2: pointer to argument address
# $t3: character holder
# $t4: address of program argument 
# $t5: counter
# $t6: currently working with byte
# $t7: current address
# $s0: sign-extended integer
# $s1: integer no sign extension
# $s2: int used for decimal conversion
# $a0: address of inputs and outputs
# $a1: holds value of argument address
##################################

.data
binaryPrompt:     .asciiz  "You entered the binary number:\n"
hexPrompt:        .asciiz  "\n\nThe hex representation of the sign-extended number is:\n"
decimalPrompt:    .asciiz  "\n\nThe number in decimal is:\n"
new_line:         .asciiz  "\n"

.text
start:    la      $a0   binaryPrompt              # load binary prompt
          li      $v0   4                         # print binary prompt
          syscall
          
          lw      $a0   ($a1)                     # load argument address
          move    $t0   $a0                       # move argument address
          li      $v0   4                         # print argument
          syscall
          
          la      $a0   hexPrompt                 # load hex prompt
          li      $v0   4                         # print hex prompt
          syscall
          
          move    $t7   $t0                       # move argument address
          li      $t5   0                         # initialize counter
          
loop:     lb      $t6   ($t7)                     # load byte
          addi    $t6   $t6           -48         # convert to int
          sb      $t6   ($t7)                     # store byte
          addi    $t7   $t7           1           # increment address
          addi    $t5   $t5           1           # increment counter
          ble     $t5   8             loop        # if counter <= 8, loop
          
          li      $s0   0                         # initialize int holder
          li      $t5   0                         # initialize counter
          move    $t2   $t0                       # move argument address
          lb      $t1   ($t2)                     # load byte
          add     $s0   $s0           $t1         # add byte to int
          
loop2:    addi    $t2   $t2           1           # increment address
          addi    $t5   $t5           1           # increment counter
          lb      $t1   ($t2)                     # load byte
          sll     $s0   $s0           1           # shift int holder left
          add     $s0   $s0           $t1         # add byte to int
          blt     $t5   7             loop2       # if counter < 7, loop
          
          move    $s1   $s0                       # move int
          lb      $t1   ($t0)                     # load byte
          beq     $t1   0x01          sign        # if byte = 01, sign extend
          j       seco                            # jump to second step
          
sign:     ori     $s0   $s0           0xffffff00  # sign extend
          
seco:     move    $t1   $s0                       # move int
          li      $t5   0                         # initialize counter
          
          li      $t3   0                         # initialize char
          add     $t3   $t3           48          # set char to ascii 0
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          li      $t3   0                         # initialize char
          add     $t3   $t3           120         # set char to ascii x
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          
loop3:    addi    $t5   $t5           1           # increment counter
          beq     $t5   9             next        # if counter = 9, go next
          li      $t3   0xf0000000                # initialize mask
          and     $t3   $t3           $t1         # mask msb
          srl     $t3   $t3           28          # shift msb to lsb pos
          sll     $t1   $t1           4           # shift int left
          bge     $t3   0xa           letter      # if msb letter, go letter
          blt     $t3   0xa           number      # if msb number, go number
          
letter:   add     $t3   $t3           55          # set char to ascii letter
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          j       loop3                           # loop
          
number:   add     $t3   $t3           48          # set char to ascii number
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          j       loop3                           # loop
          
next:     la      $a0   decimalPrompt             # load decimal prompt
          li      $v0   4                         # print decimal prompt
          syscall
          
          lb      $t1   ($t0)                     # load msb
          beq     $t1   0x01          negnum      # if msb = 01, go negnum
          beq     $t1   0x00          posnum      # if msb = 00, go posnum
          
negnum:   li      $t3   0                         # initialize char
          add     $t3   $t3           45          # set char to ascii -
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          move    $t7   $t0                       # move argument address
          li      $t5   0                         # initialize counter
          
nloop:    beq     $t5   8             nprint      # if counter = 8, go nprint
          lb      $t6   ($t7)                     # load msb
          beq     $t6   0x01          isOne       # if msb = 01, go isOne
          beq     $t6   0x00          isZero      # if msb = 00, go isZero
          
isOne:    li      $t6   0x00                      # initialize flip
          sb      $t6   ($t7)                     # store flip
          addi    $t7   $t7           1           # increment address
          addi    $t5   $t5           1           # increment counter
          j       nloop                           # loop
          
isZero:   li      $t6   0x01                      # initialize flip
          sb      $t6   ($t7)                     # store flip
          addi    $t7   $t7           1           # increment address
          addi    $t5   $t5           1           # increment counter
          j       nloop                           # loop
          
nprint:   li      $s2   0                         # initialize int
          li      $t5   0                         # initialize counter
          move    $t2   $t0                       # move argument address
          lb      $t1   ($t2)                     # load byte
          add     $s2   $s2           $t1         # add byte to int
          
nloop2:   addi    $t2   $t2           1           # increment address
          addi    $t5   $t5           1           # increment counter
          lb      $t1   ($t2)                     # load byte
          sll     $s2   $s2           1           # shift left 1
          add     $s2   $s2           $t1         # add byte to int
          blt     $t5   7             nloop2      # if counter < 7, loop
          
print:    move    $t1   $s2                       # move int
          li      $t5   0                         # initialize counter
          div     $t3   $t1           100         # div by 100, store in char
          beqz    $t3   mid                       # if char 0, mid
          bnez    $t3   big                       # else, big
          
          
          
big:      add     $t3   $t3           48          # set char to ascii
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          rem     $t1   $t1           100         # set rem
          div     $t3   $t1           10          # div by 10, store in char
          add     $t3   $t3           48          # set char to ascii
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          rem     $t3   $t1           10          # set rem to char
          add     $t3   $t3           48          # set char to ascii
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          j       exit                            # exit
          
mid:      div     $t3   $t1           10          # div by 10, store in char
          beqz    $t3   small                     # if char 0, small
          add     $t3   $t3           48          # set char to ascii
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          rem     $t3   $t1           10          # set rem to char
          add     $t3   $t3           48          # set char to ascii
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          j       exit                            # exit

small:    rem     $t3   $t1           10          # set rem to char
          add     $t3   $t3           48          # set char to ascii
          move    $a0   $t3                       # move char
          li      $v0   11                        # print char
          syscall
          j       exit                            # exit
          
posnum:   li      $s2   0                         # initialize int
          li      $t5   0                         # initialize counter
          move    $t2   $t0                       # move argument address
          lb      $t1   ($t2)                     # load byte
          add     $s2   $s2           $t1         # add byte to int
          
ploop:    addi    $t2   $t2           1           # increment address
          addi    $t5   $t5           1           # increment counter
          lb      $t1   ($t2)                     # load byte
          sll     $s2   $s2           1           # shift left 1
          add     $s2   $s2           $t1         # add byte to int
          blt     $t5   7             ploop       # if counter < 7, loop
          j       print                           # print
          
exit:     la      $a0   new_line                  # load new line
          li      $v0   4                         # print new line
          syscall
          
          li      $v0   10                        # exit
          syscall
          