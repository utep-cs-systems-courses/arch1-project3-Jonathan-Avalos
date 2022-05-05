	.balign 2

	.text
	.global song
	.extern buzzer_set_period
	.extern second

song:
	sub #2, r1
	mov &second, 0(r1)
top:
	cmp #6, 0(r1)
	jl next
	sub #6, 0(r1)
	jmp top
next:
	cmp #1, 0(r1)
	jne next1
	mov #1516, r12
	jmp end
next1:
	cmp #2, 0(r1)
	jne next2
	mov #1911, r12
	jmp end
next2:
	cmp #3, 0(r1)
	jne next3
	mov #1275, r12
	jmp end
next3:
	cmp #4, 0(r1)
	jne next4
	mov #2551, r12
	jmp end
next4:
	cmp #5, 0(r1)
	jne skip
	mov #3033, r12
	jmp end
skip:
	mov #2272, r12
end:
	call #buzzer_set_period
	add #2, r1
	pop r0
