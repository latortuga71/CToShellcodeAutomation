# CtoShellcodeAutomation

## Example

```
cl /c /FA /GS- src\Main.c && python tools\CtoShellCodeTool.py x64 Main.asm cleaned64.asm && ml64 cleaned64.asm /link /entry:AlignRSP && python tools\CtoShellCodeTool.py extract  cleaned64.exe raw_shellcode64_output.bin
```

## Why
Wanted to automate the task, and was having issues using @hasherezade tool to automate the string inlining for some reason it was not working.
Also wanted to incorporate into azure devops. To make it as simple as writing the code in C then just pushing and having it automatically built.

## Credits
 * https://vxug.fakedoma.in/papers/VXUG/Exclusive/FromaCprojectthroughassemblytoshellcodeHasherezade.pdf
 * https://github.com/hasherezade/masm_shc