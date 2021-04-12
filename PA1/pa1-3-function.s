swap:
addi $sp, $sp, -8
sw $s0, 0($sp)
sw $s1, 4($sp)
lw $s0, 0($a0)
lw $s1, 0($a1)
sw $s0, 0($a1)
sw $s1, 0($a0)
lw $s0, 0($sp)
lw $s1, 4($sp)
addi $sp, $sp, 8
jr  $ra

partition:
addi $sp, $sp, -32
sw $ra, 0($sp)
sw $s0,	4($sp)
sw $s1, 8($sp)
sw $s2, 12($sp)
sw $s3, 16($sp)
sw $s4, 20($sp)
sw $s5, 24($sp)
sw $s6, 28($sp)

addi $s0, $zero, 1
add $s1, $zero, $a0 #low = right
addi $s2, $a1, 1 #high = right + 1
la $s3, list
addi $s4, $zero, 4
mul $s4, $s4, $a0
add $s3, $s3, $s4
lw $s4, 0($s3) #pivot = list[left]
DO1: #do
DO2: #do
addi $s1, $s1, 1 #low++
slt $s5, $a1, $s1
beq $s5, $s0, DO3
la $s3, list
addi $s5, $zero, 4
mul $s5, $s5, $s1
add $s3, $s3, $s5
lw $s5, 0($s3)
slt $s6, $s5, $s4
beq $s6, $s0, DO2 #while(low<=right && list[low]<pivot)
DO3:
addi $s2, $s2, -1 #high--
slt $s5, $s2, $a0
beq $s5, $s0, ENDW
la $s3, list
addi $s5, $zero, 4
mul $s5, $s5, $s2
add $s3, $s3, $s5
lw $s5, 0($s3)
slt $s6, $s4, $s5
beq $s6, $s0, DO3 #while(high>=left && list[high]>pivot)
ENDW:
slt $s5, $s1, $s2
bne $s5, $s0, ENDIF #if(low<high)

addi $sp, $sp, -8
sw $a0 0($sp)
sw $a1 4($sp)#inside if
la $s3, list
addi $s5, $zero, 4
mul $s5, $s5, $s1
add $a0, $s3, $s5
la $s3, list
addi $s5, $zero, 4
mul $s5, $s5, $s2
add $a1, $s3, $s5
jal swap #swap(&list[low], &list[high])

lw $a0, 0($sp) #end if
lw $a1, 4($sp)
addi $sp, $sp, 8 

ENDIF:
slt $s5, $s1, $s2
beq $s5, $s0, DO1 #while(low<high)

la $s3, list
addi $s5, $zero, 4
mul $s5, $s5, $a0
add $a0, $s3, $s5
addi $s5, $zero, 4
mul $s5, $s5, $s2
add $a1, $s3, $s5
jal swap #swap(&list[left], &list[high])
add $v0, $zero, $s2 #return high
lw $ra 0($sp)
lw $s0,	4($sp)
lw $s1, 8($sp)
lw $s2, 12($sp)
lw $s3, 16($sp)
lw $s4, 20($sp)
lw $s5, 24($sp)
lw $s6, 28($sp)
addi $sp, $sp, 32
jr  $ra

Quick_sort:
addi $sp, $sp, -28
sw $a0, 0($sp)
sw $a1, 4($sp)
sw $ra, 8($sp)
sw $v0, 12($sp)
sw $s0,	16($sp)
sw $s1, 20($sp)
sw $s2, 24($sp)


addi $s0, $zero, 1
slt $s2, $a0, $a1
bne $s0, $s2, ENDQ
jal partition
lw $a0, 0($sp)
addi $a1, $v0, -1
jal Quick_sort
lw $a1, 4($sp)
addi $a0, $v0, 1
jal Quick_sort
ENDQ:
lw $a0, 0($sp)
lw $a1, 4($sp)
lw $ra, 8($sp)
lw $v0, 12($sp)
lw $s0,	16($sp)
lw $s1, 20($sp)
lw $s2, 24($sp)
addi $sp, $sp, 28
jr  $ra