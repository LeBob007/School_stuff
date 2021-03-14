####################################################################################
# Created by:  Hu, Robert
#              ryhu
#              29 August 2018
#
# Assignment:  Lab 6: Musical Subroutines
#              CMPE 012, Computer Systems and Assembly Language
#              UC Santa Cruz, Summer 2018
# 
# Description: This program writes the subroutines required to play certain
#              song combinations
# 
# Notes:       This program is intended to be run from the MARS IDE.
####################################################################################

#---------------
# REGISTER USAGE
#
# $t0: rhythm byte holder, pitch holder
# $t1: pitch byte holder, length byte holder
# $t2: hold return address
# $t3: temp rhythm holder, read note holder
# $t4: temp pitch holder, num of millisecond in minute
# $t5: hold return address
# $t6: tempo of song in millisecond per beat holder
# $t7: byte holder, check end of song
# $s0: rhythm holder
# $s1: previous/current rhythm holder

#--------------------------------------------------------------------
# play_song
#
# input:  $a0 - address of first character in string containing song
#         $a1 - tempo of song in beats per minute
#--------------------------------------------------------------------
play_song:         move    $t2         $ra                         # save return address
                   li      $t4         60000                       # load in num of ms in minute
                   div     $a1         $t4             $a1         # div ms by tempo
                   
contPlay:          move    $t6         $a1                         # save ms per beat
                   jal     read_note                               # jump to read note
                   andi    $t0         $v0             0x0000FFFF  # mask pitch
                   andi    $s0         $v0             0xFFFF0000  # mask rhythm
                   srl     $s0         $s0             16          # shift rhythm
                   jal     play_note                               # jump to play note
                   move    $a1         $t6                         # reset tempo
                   move    $a0         $v1                         # update address
                   lb      $t7         ($a0)                       # check first byte at address
                   bne     $t7         0x00            contPlay    # compare first byte with \0 char
                   jr      $t2                                     # return
                   
#--------------------------------------------------------------------
# get_song_length
#
# input:  $a0 - address of first character in string containing song
#
# output: $v0 - number of notes in song
#--------------------------------------------------------------------
get_song_length:   li      $v0         1                           # set num to 1

loopLength:        lb      $t1         ($a0)                       # load first byte
                   beq     $t1         0x20            incrLength  # if byte is space, increment length

contLength:        addi    $a0         $a0             1           # increment address
                   beq     $t1         0x00            retLength   # if byte is \0, return
                   j       loopLength                              # loop
                   
incrLength:        addiu   $v0         $v0             1           # increment length counter
                   addi    $a0         $a0             1           # increment address
                   j       loopLength                              # loop
                   
retLength:         jr      $ra                                     # return
                   
#--------------------------------------------------------------------
# play_note
#
# input:  $a0 - pitch
#         $a1 - note duration in milliseconds
#
# output: no output arguments, plays note using syscall system service 33
#--------------------------------------------------------------------
play_note:         move    $a0         $t0                         # load pitch
                   mul     $a1         $a1             4           # multiply duration by 4
                   div     $a1         $a1             $s0         # divide duration by rhythm
                   li      $a2         25                          # set instrument
                   beq     $a0         0               noPitch     # check pitch for rest note
                   li      $a3         127                         # set volume
                   li      $v0         33                          # call note
                   syscall                                         # play note
                   jr      $ra                                     # return
                   
noPitch:           li      $a3         0                           # set volume
                   li      $v0         33                          # call note
                   syscall                                         # play note
                   jr      $ra                                     # return
                   
#--------------------------------------------------------------------
# read_note
#
# input:  $a0 - address of first character in string containing note encoding
#         $a1 - rhythm of previous note
#
# output: $v0 - note rhythm in bits [31:16], note pitch in bits [15:0]
#               note rhythm: (1 = 4 beats, 2 = 2 beats, 4 = 1 beat,
#                             8 = 1/2 beat, 16 = 1/4 beat)
#               note pitch:  (MIDI value, 0-127)
#         $v1 - address of first character of what would be next note
#--------------------------------------------------------------------
read_note:         move    $t5         $ra                         # save return address
                   jal     get_pitch                               # jump to get pitch
                   li      $t4         0                           # set to 0
                   add     $t4         $t4             $v0         # add pitch to reg
                   jal     get_rhythm                              # jump to get rhythm
                   li      $t3         0                           # set to 0
                   add     $t3         $t3             $v0         # add rhythm to reg
                   sll     $t3         $t3             16          # shift reg left
                   add     $t3         $t3             $t4         # add pitch to reg
                   move    $v0         $t3                         # move to output reg
                   jr      $t5                                     # return
                   
#--------------------------------------------------------------------
# get_pitch
#
# input:  $a0 - address of first character in string containing note encoding
#
# output: $v0 - MIDI pitch value
#         $v1 - address of character after pitch is determined
#               e.g. if portion of song looks like: c'' f4
#               $v1 will point to ---------------------^
#--------------------------------------------------------------------
get_pitch:         li      $v0         0                           # initialize output pitch
                   lb      $t1         ($a0)                       # load first byte
                   
startPitch:        beq     $t1         0x61            pitchA      # check note letter
                   beq     $t1         0x62            pitchB      # check note letter
                   beq     $t1         0x63            pitchC      # check note letter
                   beq     $t1         0x64            pitchD      # check note letter
                   beq     $t1         0x65            pitchE      # check note letter
                   beq     $t1         0x66            pitchF      # check note letter
                   beq     $t1         0x67            pitchG      # check note letter
                   beq     $t1         0x72            donePitch   # check for rest note
                   addi    $a0         $a0             1           # increment address
                   j       get_pitch                               # loop
                   
pitchA:            addi    $v0         $v0             57          # set pitch to 57
                   j       contPitch                               # continue
                   
pitchB:            addi    $v0         $v0             59          # set pitch to 59
                   j       contPitch                               # continue
                   
pitchC:            addi    $v0         $v0             60          # set pitch to 60
                   j       contPitch                               # continue
                   
pitchD:            addi    $v0         $v0             62          # set pitch to 62
                   j       contPitch                               # continue
                   
pitchE:            addi    $v0         $v0             64          # set pitch to 64
                   j       contPitch                               # continue
                   
pitchF:            addi    $v0         $v0             65          # set pitch to 65
                   j       contPitch                               # continue
                   
pitchG:            addi    $v0         $v0             67          # set pitch to 67
                   j       contPitch                               # continue
                   
contPitch:         addi    $a0         $a0             1           # increment address
                   lb      $t1         ($a0)                       # load first byte
                   beq     $t1         0x65            flatPitch   # check for pitch modifiers
                   beq     $t1         0x69            sharpPitch  # check for pitch modifiers
                   beq     $t1         0x27            inPitch     # check for pitch modifiers
                   beq     $t1         0x2c            dePitch     # check for pitch modifiers
                   beq     $t1         0x20            donePitch   # check for pitch modifiers
                   bge     $t1         0x31            checkPitch  # check if pitch is num
                   beq     $t1         0x00            donePitch   # check for \0 char
                   j       contPitch                               # loop
                   
flatPitch:         subi    $v0         $v0             1           # decrease pitch by 1
                   addi    $a0         $a0             1           # increment address
                   j       contPitch                               # loop
                   
sharpPitch:        addi    $v0         $v0             1           # increase pitch by 1
                   addi    $a0         $a0             1           # increment address
                   j       contPitch                               # loop
                   
inPitch:           addi    $v0         $v0             12          # increase pitch by octave
                   j       contPitch                               # loop
                   
dePitch:           subi    $v0         $v0             12          # decrease pitch by octave
                   j       contPitch                               # loop
                   
checkPitch:        ble     $t1         0x38            checkPitch2 # check if byte is num
                   
checkPitch2:       j       donePitch                               # done
                   
donePitch:         move    $v1         $a0                         # update to new address
                   jr      $ra                                     # return
                   
#-------------------------------------------------------------
# get_rhythm
#
# input:  $a0 - address of character in string containing song encoding
#               after pitch is determined
#               --> e.g. if song portion looks like: c'' f4
#                   $a0 will point to ------------------^
#               --> e.g. if song portion looks like: aes,8 f16
#                   $a0 will point to -------------------^
#         $a1 - previous note rhythm
#
# output: $v0 - note rhythm, default to previous note rhythm if no number
#               is present in note encoding
#               (1 = 4 beats, 2 = 2 beats, 4 = 1 beat, 8 = 1/2 beat, 16 = 1/4 beat)
#         $v1 - address of first character of next note
#-------------------------------------------------------------
get_rhythm:        li      $v0         0                           # initialize rhythm
                   lb      $t0         ($a0)                       # load first byte
                   beq     $t0         0x31            rhythm1     # check rhythm value
                   beq     $t0         0x32            rhythm2     # check rhythm value
                   beq     $t0         0x34            rhythm4     # check rhythm value
                   beq     $t0         0x38            rhythm8     # check rhythm value
                   j       noRhythm                                # no rhythm
                   
rhythm1:           lb      $t0         1($a0)                      # load next byte
                   beq     $t0         0x36            rhythm16    # check rhythm value
                   addi    $v0         $v0             1           # increase rhythm by 1
                   move    $s1         $v0                         # save rhythm value
                   j       doneRhythm                              # done
                   
rhythm2:           addi    $v0         $v0             2           # increase rhythm by 2
                   move    $s1         $v0                         # save rhythm value
                   j       doneRhythm                              # done
                   
rhythm4:           addi    $v0         $v0             4           # increase rhythm by 4
                   move    $s1         $v0                         # save rhythm value
                   j       doneRhythm                              # done
                   
rhythm8:           addi    $v0         $v0             8           # increase rhythm by 8
                   move    $s1         $v0                         # save rhythm value
                   j       doneRhythm                              # done
                   
rhythm16:          addi    $v0         $v0             16          # increase rhythm by 16
                   move    $s1         $v0                         # save rhythm value
                   j       doneRhythm                              # done
                   
noRhythm:          move    $v0         $s1                         # load previous rhythm value
                   j       doneRhythm                              # done
                   
doneRhythm:        addi    $a0         $a0             1           # increment address
                   move    $v1         $a0                         # update to new address
                   jr      $ra                                     # return
                   