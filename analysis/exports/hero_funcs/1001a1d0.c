
void __cdecl FUN_1001a1d0(int param_1)

{
  short sVar1;
  
  FUN_10029260(param_1);
  *(uint *)(param_1 + 0x54) = *(uint *)(param_1 + 0x54) | 0x2005;
  sVar1 = *(short *)(DAT_1007217c + 0x10);
  *(undefined1 *)(param_1 + 0x23) = 5;
  *(undefined2 *)(param_1 + 0x4a) = 0xa0;
  *(short *)(param_1 + 0x3e) = sVar1 + -0x10;
  *(ushort *)(param_1 + 0x74) = (*(byte *)(DAT_10070f24 + 0x38) & 7) << 7;
  return;
}

