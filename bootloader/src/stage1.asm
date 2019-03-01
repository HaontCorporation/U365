org 0x7C00 ; BIOS Starting Address
jmp startup
nop

; FAT16 BPB (BIOS Parameter Block)
OEMName: db 'HAONTCOR'
BytesPerSector: dw 512
SectorsPerCluster: db 1 
ReservedSectorsCount: dw 1
FATCount: db 2
RootDirEntries: dw 224
TotalSectors: dw 2880
MediaType: db 0xF0
SectorsPerFAT: dw 12
SectorsPerTrack: dw 18
HeadsCount: dw 2
HiddenSectorsCount: dw 0
DriveIndex: db 0
Reserved: db 0
BootSignature: db 0x29
VolumeLabel: db 'HAONT_U365 '
Filesystem: db 'FAT16   '

RootDirBase: dw 0x500
Stage2LBA: dq 0
Stage2SectorsCount: dw 0

startup:
    mov [DriveIndex], dl
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov si, message
    call puts

    call load_stage2
    mov dl, [DriveIndex]
    jmp 0x7E00 ; stage2

    cli
    hlt

load_stage2:
.load_rootdir:
    xor dx, dx
    mov ax, [RootDirEntries]
    mov bl, 32
    mul bl
    div word [BytesPerSector]
    mov [dap.SectorsCount], ax

    mov bp, [RootDirBase]
    mov [dap.BufferOffset], bp

    mov al, [FATCount]
    mul word [SectorsPerFAT]
    add al, [ReservedSectorsCount]
    mov [dap.LBA], al

    mov ah, 42h
    mov dl, [DriveIndex]
    mov si, dap
    int 0x13

    jc error

.load_stage2info:
    .loop:
        cld
        mov di, [RootDirBase]
        mov si, Stage2Name
        mov cx, 11
        repe cmpsb
        je .found
        add byte [RootDirBase], 0x20
        jmp .loop
    .found:
        mov di, [RootDirBase]

        xor dx, dx
        mov ax, word [di+0x1A]
        add ax, 37 ; (1 + 24 + 14 - 2)
        mov word [Stage2LBA], ax

        mov eax, dword [di+0x1C]
        div word [BytesPerSector]
        inc eax 
        mov [Stage2SectorsCount], ax

.load_stage2:
    mov ax, [Stage2SectorsCount]
    mov [dap.SectorsCount], ax

    mov bp, 0x7E00
    mov [dap.BufferOffset], bp

    mov ax, [Stage2LBA]
    mov word [dap.LBA], ax

    mov ah, 42h
    mov dl, [DriveIndex]
    mov si, dap
    int 0x13

    jc error
    
    ret

puts:
.loop:
    mov ah, 0Eh
    mov al, [si]
    int 0x10
    inc si
    cmp byte [si], 0
    jnz .loop
    
    ret

error:
    xor ax, ax
    int 0x16

    db 0xEA
    dd 0xFFFF0000

dap: ; Disk Address Packet
.DAPSize:       dw 0x10 ; Always 0x10
.SectorsCount:  dw 0 ; (some Phoenix BIOSes are limited to a maximum of 127 sectors)
.BufferOffset:  dw 0
.BufferSegment: dw 0
.LBA:           dq 0

message: db 'Bootloader: Stage1', 0Dh, 0Ah, 0
Stage2Name: db 'STAGE2  BIN'

times 510-($-$$) db 0
dw 0xAA55 ; Bootable Signature