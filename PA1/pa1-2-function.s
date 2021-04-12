product: #i = t0, j = t1, k = t3 1 = s0
addi $sp, $sp, -20
sw $s0 0($sp)
sw $s1 4($sp)
sw $s2 8($sp)
sw $ra 12($sp)
sw $a0 16($sp)
lw $t2, DIM
addi $s0, $zero, 1
beq $s0, $a0, END
add $t0, $zero, $zero #initial i
LOOPI1:
add $t1, $zero, $zero #initial j
LOOPJ1:
add $t3, $zero, $zero #initial k
LOOPK:
###########
addi $t4, $zero, 40
addi $t5, $zero, 4
la $t6, A
la $t7, B
la $t8, C
mul $s1, $t4, $t0 #A[i][k]
mul $s2, $t5, $t3
add $s1, $s1, $s2
add $t6, $t6, $s1
lw $t6, 0($t6)
mul $s1, $t4, $t3 #B[k][j]
mul $s2, $t5, $t1
add $s1, $s1, $s2
add $t7, $t7, $s1
lw $t7, 0($t7)
mul $t6, $t6, $t7 #a*b = t6
mul $s1, $t4, $t0 #C[i][j]
mul $s2, $t5, $t1
add $s1, $s1, $s2
add $t8, $t8, $s1
lw $t9, 0($t8)
add $t9, $t9, $t6 #c+a*b
sw $t9, 0($t8)


##########
addi $t3, $t3, 1
slt $s1, $t3, $t2
beq $s0, $s1, LOOPK #end of loop k


addi $t1, $t1, 1
slt $s1, $t1, $t2
beq $s0, $s1, LOOPJ1 #end of loop j1

addi $t0, $t0, 1
slt $s1, $t0, $t2
beq $s0, $s1, LOOPI1 #end of loop i1


add $t0, $zero, $zero #initial i
LOOPI2:
add $t1, $zero, $zero #initial j
LOOPJ2:
###########
la $t7, B
la $t8, C
mul $s1, $t4, $t0 #C[i][j]
mul $s2, $t5, $t1
add $s1, $s1, $s2
add $t8, $t8, $s1
lw $t8, 0($t8)
mul $s1, $t4, $t0 #B[i][j]
mul $s2, $t5, $t1
add $s1, $s1, $s2
add $t7, $t7, $s1
sw $t8, 0($t7)
la $t8, C
mul $s1, $t4, $t0 #C[i][j]
mul $s2, $t5, $t1
add $s1, $s1, $s2
add $t8, $t8, $s1
sw $zero, 0($t8)


##########
addi $t1, $t1, 1
slt $s1, $t1, $t2
beq $s0, $s1, LOOPJ2 #end of loop j2

addi $t0, $t0, 1
slt $s1, $t0, $t2
beq $s0, $s1, LOOPI2 #end of loop i2



addi $a0, $a0, -1 # call product
jal product

END:
lw $s0 0($sp)
lw $s1 4($sp)
lw $s2 8($sp)
lw $ra 12($sp)
lw $a0 16($sp)
addi $sp, $sp, 20
jr  $ra

printResult:
lw $t2, DIM
add $t1, $zero $zero #t1 = i
PLOOP1:
add $t3, $zero $zero #t3 = j
PLOOP2:

la $t4, B
addi $t5, $zero, 40
addi $t6, $zero, 4
mul $t5, $t5, $t1
mul $t6, $t6, $t3
add $t5, $t5, $t6
add $t4, $t5, $t4
li	$v0, 1
lw	$a0, 0($t4)
syscall		                  # printf("%d",B[i][j]);
li	$v0, 4
la	$a0, SPACE
syscall		                  # printf(" \t ")

addi $t3, $t3, 1
slt $t0, $t3, $t2 #t2 = DIM
beq $t0, $zero, PBREAK
j PLOOP2
PBREAK:
li	$v0, 4
la	$a0, ENTER
syscall		                  # printf("\n")
addi $t1, $t1, 1
slt $t0, $t1, $t2 #t2 = DIM
bne $t0, $zero, PLOOP1
PED:
jr  $ra