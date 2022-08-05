	.text

main:
	# Register assignments
	# $s0 = x
	# $s1 = y

	# Initialize registers
	lw	$s0, 0($gp)		# Reg $s0 = x
	lw	$s1, 4($gp)		# Reg $s1 = y

	# Call function
	addi	$a0, $s0,0	# Argument 1: x ($s0)
	jal	fun		# Save current PC in $ra, and jump to fun
	addi	$s1,$v0,0	# Return value saved in $v0. This is y ($s1)

	# Print msg1
	addi	$v0, $zero,4		# print_string syscall code = 4
	addi	$a0, $gp, 8
	syscall

	# Print result (y)
	addi	$v0,$zero,1		# print_int syscall code = 1
	addi	$a0, $s1,0	# Load integer to print in $a0
	syscall

	# Print newline
	addi	$v0,$zero,4		# print_string syscall code = 4
	addi	$a0, $gp, 12
	syscall

	# Exit
	addi	$v0,$zero,10		# exit
	syscall

# ------------------------------------------------------------------
	
	# FUNCTION: int fun(int a)
	# Arguments are stored in $a0
	# Return value is stored in $v0
	# Return address is stored in $ra (put there by jal instruction)
	# Typical function operation is:
	
fun:	# Do the function math
	addi $s0, $zero,3
	mul $s1,$s0,$a0		# s1 = 3*$a0  (i.e. 3*a)
	addi $s1,$s1,5		# 3*a+5

	# Save the return value in $v0
	addi $v0,$s1,0

	# Return from function
	jr $ra			# Jump to addr stored in $ra
	
# ------------------------------------------------------------------
	
	# Start .data segment (data!)
	.data
x:	.word 5
y:	.word 0
msg1:	.asciiz	"y="
lf:     .asciiz	"\n"