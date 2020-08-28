.intel_syntax noprefix
.text
.global huffman_encode
.bss
.lcomm length, 5000
.lcomm encoding, 5000
.text
huffman_encode:
    push r12
    push r13
    push r14
    push r15
    push rbx
    xor rax, rax
    mov r12, rdi
    countloop: //rcx[ascii(char)] = frequency of the char
    mov al, byte ptr [r12]
    cmp al, 0
    je createleafs
    inc qword ptr [rcx + 8*rax]
    inc r12
    jmp countloop

    createleafs: //initializes the list of nodes with the leafs of the tree (left == right == 0)
    xor r12, r12 //initialize rcx counter
    xor r13, r13 //initialize nodelist counter
    filterloop: //ignores chars with freq 0, creates leaves for the others
    inc r12b //start with 1, there is no 0 in a string (except at the end)
    cmp r12b, 0xff //so from 1 to 255, only 0 is left out (this is the ascii code)
    je sort //if done, sort the r8
    cmp qword ptr [rcx + 8*r12], 0
    je filterloop //skip if 0 (not present in string, freq = 0)
    shl r13w, 4 //*16, scaling
    mov rax, r13
    add rax, 17
    cmp rax, rdx
    jnc error
    mov dword ptr [r9 + r13 + 2], r12d //char code at the top of a node (0 in inner nodes, dword not byte for optimized alignment)
    mov rax, qword ptr [rcx + 8*r12]
    mov qword ptr [r9 + r13 + 6], rax //frequency at the 8 bytes under
    //left is 0 (because leaf) (already initialized)
    //right is 0 (already initialized)
    shr r13w, 4 //scale back to normal
    mov word ptr [r8 + 2*r13], r13w //this has the index of the node in nodelist, will later be reordered
    inc r13b
    jmp filterloop

    sort:
    push rdx
    xor rdx, rdx //initialize l
    dec r13b //initialize r (from 0 to size - 1)
    xor r15, r15 //initialize pivot
    xor r10, r10 //initialize i
    xor r11, r11 //initialize j
    call quicksort
    pop rdx
    lea r12, [r13 + 1] //initialize nodelist counter

    buildtree: //start making the inner nodes
    cmp r13w, 0
    je buildend
    //new node will be in r15, r10w, r11w (no need for top part because it is always 0)
    mov r10w, word ptr [r8 + 2*r13] //r10 has the address of the actual left node
    shl r10d, 4 //*16, scaling
    mov r15, qword ptr [r9 + r10 + 6]
    shr r10d, 4 //scale back to normal
    dec r13
    mov r11w, word ptr [r8 + 2*r13] //r11 has the address of the actual right node
    shl r11d, 4 //*16, scaling
    add r15, qword ptr [r9 + r11 + 6] //sum left and right freq
    shr r11d, 4 //scale back to normal

    //add the node to node list, index is r12w
    shl r12, 4 //*16, scaling
    mov rax, r12
    add rax, 17
    cmp rax, rdx
    jnc error // if result size surpassed, return -1
    mov dword ptr [r9 + r12 + 2], 0
    mov qword ptr [r9 + r12 + 6], r15
    mov word ptr [r9 + r12 + 14], r10w
    mov word ptr [r9 + r12 + 16], r11w
    shr r12, 4 //scale back to normal

    push rbx
    mov rax, r13 //rax is the counter in r8, moves back untill a bigger freq is rear13ed
    cmp rax, 0
    jnz insert //insert the node address at the right place in r8

    back:
    pop rbx
    inc r12w
    jmp buildtree


    insert: //insert the node address at the right place in r8
    dec rax
    xor rbx, rbx
    mov bx, word ptr [r8 + 2*rax]
    shl rbx, 4 //*16, scaling
    cmp qword ptr [r9 + rbx + 6], r15
    ja insertend
    xor rbx, rbx
    mov bx, word ptr [r8 + 2*rax]
    mov word ptr [r8 + 2*rax + 2], bx
    cmp rax, 0
    jnz insert
    mov word ptr [r8 + 2*rax], r12w //address of the new node
    jmp back

    insertend:
    mov word ptr [r8 + 2*rax + 2], r12w //address of the new node
    jmp back

    buildend: //r8[0] is now the index of the root
    mov word ptr [r9], r12w //r12w was the counter in the list of nodes in result
    xor rax, rax //no error, return 0
   // ret


   //r9 fih l tree
    xor r10,r10
    xor r15,r15
    xor rbx,rbx
    xor rcx,rcx
    mov bl, [r9]
    dec bl  // r15=8
    shl rbx,4

  // mov r13,result+r15+2  //
   mov r13 ,r9
   add r13,rbx
   add r13,2
   xor rbx,rbx
   debug:
     call encode
    after:
    xor rax, rax

xor rcx,rcx
xor rbx,rbx
xor r8,r8
xor r10,r10
xor r11,r11
xor r12,r12
xor r13,r13
xor r14,r14
xor rdx,rdx
xor rcx,rcx
xor r13,r13
//lea r14,[enc]
mov r15,rdi

traverse_string:
xor rbx,rbx
mov bl,[r15]
inc r15
CMP bl,0
je end
shl rbx,4
mov r12,[encoding+rbx] //    r9=encoding
mov cl,[length+rbx] // cl=length

add dl,cl
add rax,rcx
cmp dl , 64
jg greater
sub dl,cl
label:
shl r14,cl
or r14,r12
add dl,cl
jmp traverse_string

end:
cmp dl,0
je endee
mov cl,dl
ror r14,cl
mov [rsi+r13],r14
endee:
pop rbx
pop r15
pop r14
pop r13
pop r12
ret

greater:
mov r10,r12
mov r8b,dl
sub r8,64
Xchg r8,rcx
shr r10,cl
sub rcx, r8
neg rcx
shl r14,cl
or r14,r12
mov [rsi+r13],r14
add r13,8
xor r14,r14
xor rdx,rdx
sub rcx, r8
neg rcx
mov r11,1
shl r11,cl
dec r11
and r12,r11
jmp label


   encode:

   push rcx
   push r15
   cmp byte ptr [r13], 0
   jne leaf
    xor r8,r8
    xor r15, r15
    mov r8w, [r13 + 14]// r8=rightchild
    mov r15w, [r13 + 12]// r15 = left child
    mov r13, r8
    shl r13, 4

    //add r13,r14 //r13+

    lea r13, [r13 + r9 + 2]
    shl rcx, 1
    inc rcx
    inc r10
    call encode
    shr rcx, 1
    dec r10
    mov r13, r15
    shl r13, 4

    lea r13, [r13 + r9 + 2]
    shl rcx, 1
    inc r10
    call encode
    shl rcx,1
    dec r10

     pop r15
     pop rcx

    ret

    leaf:
    xor rbx,rbx
    mov bl, [r13]
    shl rbx,4

    mov [encoding+rbx],rcx
    mov [length+rbx],r10


    //mov [encoding+r10], bl
   // mov [encoding+r10+1], cl
    //lea r10, [r10 + 9]

    pop r15
    pop rcx
    ret



//label0:
//inc byte ptr [length+rbx]




    error:
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    mov rax, -1 //error, return -1
    ret



quicksort:
    push dx //max 255 leaves, so 16 bits are enough for the counters
    push r10w

    cmp dx,r13w
    jge quicksortend
    xor rax, rax
    mov ax, word ptr [r8 + 2*r13]
    shl rax, 4 //*16, scaling
    mov r15, qword ptr [r9 + rax + 6] //order according to frequency
    mov r10w, dx
    dec r10w
    mov r11w, r13w

    bigdo:

    firstdo:
    inc r10w
    xor rax, rax
    mov ax, word ptr [r8 + 2*r10]
    shl rax, 4 //*16, scaling
    cmp qword ptr [r9 + rax + 6], r15
    jg firstdo

    seconddo:
    dec r11w
    cmp r11w, dx
    jl escape
    xor rax, rax
    mov ax, word ptr [r8 + 2*r11]
    shl rax, 4 //*16, scaling
    cmp qword ptr [r9 + rax + 6], r15
    jl seconddo

    escape:
    cmp r10w, r11w
    jge bigdoend

    //swap(address at i, address at j)
    mov r12w, word ptr [r8 + 2*r10]
    mov ax, word ptr [r8 + 2*r11]
    mov word ptr [r8 + 2*r10], ax
    mov word ptr [r8 + 2*r11], r12w

    jmp bigdo

    bigdoend:

    //swap(address at i, address at r)
    mov r12w, word ptr [r8 + 2*r10]
    mov ax, word ptr [r8 + 2*r13]
    mov word ptr [r8 + 2*r10], ax
    mov word ptr [r8 + 2*r13], r12w

    push r13w
    mov r13w, r10w
    dec r13w
    call quicksort
    pop r13w
    mov dx, r10w
    inc dx
    call quicksort

    quicksortend:
    pop r10w
    pop dx
    ret
