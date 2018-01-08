/*
 * QuickSort.S
 *
 *      Author : Cho Youngwoo
 *  Student ID : 2014210085
 *
 */

/*
from Introduction to Algorithm 3rd Edition 171p

void QuickSort(int * A, int p int r){

     int q;

          if (p < r){
               q = Partition(A, p, r);
               QuickSort(A, p, q - 1);
               QuickSort(A, q + 1, r);
          }
}

int Partition(int * A, int p, int r){

     int pivot = A[r];
     int i = p - 1;
     int j;

          for (j = p; j < r; j++){
               if (A[j] <= pivot){
                    i++;
                    exchange(A[i], A[j]);    // no implement
               }
          }
          exchange(A[i + 1], A[r]);          // no implement

     return i + 1;
}
*/


swap:     sll $t1, $a1, 2   # $t1 = k * 4
          add $t1, $a0, $t1 # $t1 = v+(k*4)
                            #   (address of v[k])
          lw $t0, 0($t1)    # $t0 (temp) = v[k]
          lw $t2, 4($t1)    # $t2 = v[k+1]
          sw $t2, 0($t1)    # v[k] = $t2 (v[k+1])
          sw $t0, 4($t1)    # v[k+1] = $t0 (temp)
          jr $ra            # return to calling routine


Partition:
          move $t1, $a2            # copy r == $a2 into $t1
          sll $t1, $t1, 2          # $t1 = 4 * $t1
          add $t1, $a0, $t1        # $t1 = &A[r]
          addi $t2, $a1, -1        # $t2 = $a1 - 1 (i == $t2)
          move $t3, $a1            # copy p == $a1 into $t3 == j
          lw $t4, 0($t1)           # $t4 = A[r] == pivot
forP:     slt $t0, $t3, $a2        # $t0 = (j == $t3 < $a2 == r)
          beq $t0, $zero, endP     # if $t0 == 0 then goto endP
          move $t5, $t3            # copy j == $t3 into $t5
          sll $t5, $t5, 2          # $t5 = 4 * $t5
          add $t5, $a0, $t5        # $t5 = &A[j]
          lw $t6, 0($t5)           # $t6 = A[j]
          slt $t0, $t4, $t6        # $t0 = (pivot == $t4 < $t6 == A[j])
          bne $t0, $zero, noSwap   # if pivot < A[j] then goto noSwap
          addi $t2, $t2, 1         # $t2 = $t2 + 1 (i++)
          move $t7, $t2            # copy i == $t2 into $t7
          sll $t7, $t7, 2          # $t7 = 4 * $t7
          add $t7, $a0, $t7        # $t7 = &A[i]
          lw $t8, 0($t7)           # $t8 = A[i]
          sw $t6, 0($t7)           # exchange A[i] and A[j]
          sw $t8, 0($t5)           # exchange A[i] and A[j]
noSwap:   addi $t3, $t3, 1         # $t3 = $t3 + 1 (j++)
          j forP                   # goto forP
endP:     addi $t2, $t2, 1         # $t2 = $t2 + 1 (i++)
          move $t5, $t2            # copy i == $2 into $t5
          sll $t5, $t5, 2          # $t5 = 4 * $t5
          add $t5, $a0, $t5        # $t5 = &A[i + 1]
          lw $t7, 0($t1)           # $t7 = A[r]
          lw $t6, 0($t5)           # $t6 = A[i + 1]
          sw $t6, 0($t1)           # exchange A[r] and A[i + 1]
          sw $t7, 0($t5)           # exchange A[r] and A[i + 1]
          move $v0, $t2            # copy i == $2 into $v0 for return value
          jr $ra
     

.globl QuickSort
QuickSort:
          slt $t0, $a1, $a2        # $t0 = ($a1 == p < r == $a2)
          beq $t0, $zero, endQ     # if $t0 == 0 then goto endQ
          addi $sp, $sp, -12       # make room in stack for args
          sw $ra, 8($sp)           # save $ra into stack
          sw $a1, 4($sp)           # save $a1 into stack
          sw $a2, 0($sp)           # save $a2 into stack
          jal Partition            # call Partition
          addi $sp, $sp, -4        # make room in stack for v0
          sw $v0, 0($sp)           # save $v0 into stack
          move $t1, $v0            # copy $v0 into $t1
          addi $t1, $t1, -1        # $t1 = $t1 - 1 (q -= 1)
          move $a2, $t1            # move $t1 into $a2 (3rd arg == q - 1)
          jal QuickSort            # recursive call
          lw $v0, 0($sp)           # restore $v0
          lw $a2, 4($sp)           # restore $a2
          move $t1, $v0            # copy $v0 into $t1
          addi $t1, $t1, 1         # $t1 = $t1 + 1 (q += 1)
          move $a1, $t1            # move $t1 into $a1 (2nd arg == q + 1)
          jal QuickSort            # recursive call
          lw $v0, 0($sp)           # restore $v0
          lw $a2, 4($sp)           # restore $a2
          lw $a1, 8($sp)           # restore $a1
          lw $ra, 12($sp)          # restore $ra
          addi $sp, $sp, 16        # restore stack pointer
endQ:     jr $ra                   # return to calling routine


.globl BubbleSort
BubbleSort:
          addi $sp,$sp,-20       # make room on stack for 5 registers
          sw $ra, 16($sp)        # save $ra on stack
          sw $s3,12($sp)         # save $s3 on stack
          sw $s2, 8($sp)         # save $s2 on stack
          sw $s1, 4($sp)         # save $s1 on stack
          sw $s0, 0($sp)         # save $s0 on stack
          move $s2, $a0           # save $a0 into $s2
          move $s3, $a1           # save $a1 into $s3
          move $s0, $zero         # i = 0
for1tst:  slt  $t0, $s0, $s3      # $t0 = 0 if $s0 ≥ $s3 (i ≥ n)
          beq  $t0, $zero, exit1  # go to exit1 if $s0 ≥ $s3 (i ≥ n)
          addi $s1, $s0,-1        # j = i - 1
for2tst:  slti $t0, $s1, 0        # $t0 = 1 if $s1 < 0 (j < 0)
          bne  $t0, $zero, exit2  # go to exit2 if $s1 < 0 (j < 0)
          sll  $t1, $s1, 2        # $t1 = j * 4
          add  $t2, $s2, $t1      # $t2 = v + (j * 4)
          lw   $t3, 0($t2)        # $t3 = v[j]
          lw   $t4, 4($t2)        # $t4 = v[j + 1]
          slt  $t0, $t4, $t3      # $t0 = 0 if $t4 ≥ $t3
          beq  $t0, $zero, exit2  # go to exit2 if $t4 ≥ $t3
          move $a0, $s2           # 1st param of swap is v (old $a0)
          move $a1, $s1           # 2nd param of swap is j
          jal  swap               # call swap procedure
          addi $s1, $s1, -1       # j -= 1
          j    for2tst            # jump to test of inner loop
exit2:    addi $s0, $s0, 1        # i += 1
          j    for1tst            # jump to test of outer loop
exit1:    lw $s0, 0($sp)  # restore $s0 from stack
          lw $s1, 4($sp)         # restore $s1 from stack
          lw $s2, 8($sp)         # restore $s2 from stack
          lw $s3,12($sp)         # restore $s3 from stack
          lw $ra,16($sp)         # restore $ra from stack
          addi $sp,$sp, 20       # restore stack pointer
          jr $ra                 # return to calling routine

