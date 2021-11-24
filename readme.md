cl /c /GS- /FA <file>.cpp
masm_shc.exe <file>.asm <cleaned_file>.asm
ml <cleaned_file>.asm /link /entry:<my_entry_func>
dump text section
done.


# 32 bit

# add assume 
add assume fs:nothing
# comment out includes
;INCLUDELIB LIBCMT
;INCLUDELIB OLDNAMES
;include listing.inc
# fix strings 

# fix all jmps from jmp SHORT to jmp


# 64 bit

comment out includes

# replace FLAT: with empty string

# remove all pdata and xdata segments

# fix reference to gs register to gs:[96]

# add stub before first text section
_TEXT SEGMENT
AlignRSP PROC
    push rsi
    mov rsi,rsp
    and rsp, 0FFFFFFFFFFFFFFF0h
    sub rsp, 020h
    call main
    mov rsp, rsi
    pop rsi
    ret
AlignRSP ENDP
_TEXT ENDS

## inline strings different method since x64


# compile with align entry point