int i1=-3;
unsigned char b2=5;
unsigned int i2=72;
void main()
{
  _asm
  {
    movzx eax, i2
    push eax
    push 3
    pop ebx
    pop eax
    add eax, ebx
    push eax
    movsx eax, i1
    push eax
    pop ebx
    pop eax
    imul eax, ebx
    push eax
    pop eax
    mov b2, al
    movzx eax, b2
    push eax
    push i2
    pop ebx
    pop eax
    imul eax, ebx
    push eax
    push 2
    pop ebx
    pop eax
    cdq
    idiv ebx
    push eax
    pop eax
    mov i2, eax
  }
}
