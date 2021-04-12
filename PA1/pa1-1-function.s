GCD: #a0 = n, a1 = m
addi $sp, $sp, -12

sw $ra 0($sp)

sw $a0 4($sp)

sw $a1 8($sp)

slt $t0, $a0, $a1

beq $t0, $zero, GL1

add $t1, $a1, $zero #t1 = j

j GL2

GL1:

add $t1, $a0, $zero
 

GL2:

add $t2, $t1, $zero #t2 = i

slt $t0, $zero, $t2

beq $t0, $zero, GED

GLOOP:

div $a0, $t2

mfhi $t3

bne $t3, $zero, GL3

div $a1, $t2

mfhi $t4

bne $t4, $zero, GL3

add $v0, $t2, $zero

j GED

 

GL3:

addi $t2, $t2, -1 

slt $t0, $zero, $t2

bne $t0, $zero, GLOOP

GED:

lw $ra 0($sp)

lw $a0 4($sp)

lw $a1 8($sp)

addi $sp, $sp, 12

jr  $ra

###################

LCM:

addi $sp, $sp, -12

sw $ra 0($sp)

sw $a0 4($sp)

sw $a1 8($sp)

slt $t0, $a1, $a0

beq $t0, $zero, LL1

add $t1, $a0, $zero #t1 = j

j LL2

LL1:

add $t1, $a1, $zero

add $t2, $t1, $zero	#t2 = i

LLOOP:

div $t2, $a0

mfhi $t3

bne $t3, $zero, LL2

div $t2, $a1

mfhi $t3

beq $t3, $zero, LED

LL2:

add $t2, $t2, $t1

j LLOOP;

LED:

add $v0, $t2, $zero

lw $ra 0($sp)

lw $a0 4($sp)

lw $a1 8($sp)

addi $sp, $sp, 12

jr  $ra