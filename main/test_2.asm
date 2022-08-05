.text


main:
	# Print msg1
	addi	$v0,$zero,4		# print_string syscall code = 4
	addi	$a0, $gp,0	
	syscall

	# Get N from user and save
	addi	$v0,$zero,5		# read_int syscall code = 5
	syscall	
	addi	$t0,$v0,0		# syscall results returned in $v0

	# Initialize registers
	addi	$t1, $zero,0		# initialize counter (i)
	addi	$t2, $zero,0		# initialize sum

	# Main loop body
loop:	addi	$t1, $t1, 1	# i = i + 1
	add	$t2, $t2, $t1	# sum = sum + i
	beq	$t0, $t1, exit	# if i = N, continue
	j	loop

	# Exit routine - print msg2
exit:	addi	$v0, $zero,	4		# print_string syscall code = 4
	addi	$a0, $gp, 28	
	syscall

	# Print sum
	addi	$v0,	$zero,1		# print_string syscall code = 4
	addi	$a0, $t2, 0
	syscall

	# Print newline
	addi	$v0,$zero,4		# print_string syscall code = 4
	addi	$a0, $gp, 36
	syscall
	addi	$v0,$zero,10		# exit
	syscall

	# Start .data segment (data!)
	.data
msg1:	.asciiz	"Number of integers (N)?  "
msg2:	.asciiz	"Sum = "
lf:     .asciiz	"\n"