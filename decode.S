.intel_syntax noprefix
.global huffman_decode

huffman_decode:
//rcx contains encoding string length
push r12
push r13
push r14
push r15
push rbp
push rbx
add rdx,rsi
xor r15,r15
xor r10,r10
xor r11,r11
xor rax,rax
xor rbx,rbx
lea r15,[rdi]
mov r14,[r15]
lea r10,[rsi]
mov bl, [r8]
dec bl  // bl=8 nb of letttres
shl rbx,4

  decoding:
       // mov r13,result+r15+2  //
        mov r13 ,r8
        add r13,rbx
        add r13,2


    traverse_tree:
    cmp rax,rcx // length of string
    jg fin
    cmp r11,64
    je load_register
  return:
    cmp byte ptr [r13], 0
    jne leaff

    xor rbp,rbp
    xor r12, r12
    mov bp, [r13 + 14]// r8=rightchild
    mov r12w, [r13 + 12]// r12 = left child
    inc rax //counter
    inc r11
    shl r14,1
    jc leftChild

    mov r13, r12
    shl r13, 4
    lea r13, [r13 + r8 + 2]
    jmp traverse_tree




    leftChild:
    mov r13, rbp
    shl r13, 4
    lea r13, [r13 + r8 + 2]
    jmp traverse_tree

leaff:
xor r9,r9
mov r9b, [r13]
cmp rcx,1
je ende

cmp r10, rdx
je error
mov  [r10] , r9b
inc r10
jmp decoding

load_register:
xor r11,r11
add r15,8
mov r14,[r15]
jmp return

error:
mov rax,-1
jmp ende
fin:
mov rax ,r10
sub rax, rsi
ende:
pop rbx
pop rbp
pop r15
pop r14
pop r13
pop r12
ret
