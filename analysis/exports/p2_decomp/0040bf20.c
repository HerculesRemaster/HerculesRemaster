
uint FUN_0040bf20(short param_1,short param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  short in_stack_00000018;
  short in_stack_0000001c;
  
  iVar1 = (&DAT_00a8e790)[param_1];
  if (iVar1 != 0) {
    if ((-1 < param_2) && (param_2 < 0x80)) {
      iVar4 = (int)*(char *)(param_2 + 0x144 + iVar1);
      iVar2 = in_stack_00000018 * iVar4;
      iVar2 = iVar2 * iVar2;
      iVar2 = ((int)(iVar2 + (iVar2 >> 0x1f & 0x3fffU)) >> 0xe) * 0x81;
      iVar4 = in_stack_0000001c * iVar4;
      iVar4 = iVar4 * iVar4;
      iVar4 = ((int)(iVar4 + (iVar4 >> 0x1f & 0x3fffU)) >> 0xe) * 0x81;
      uVar3 = FUN_0040f940((int)*(short *)(iVar1 + 0x44 + param_2 * 2));
      FUN_0040c160(uVar3,(int)(iVar2 + (iVar2 >> 0x1f & 0x7fU)) >> 7,
                   (int)(iVar4 + (iVar4 >> 0x1f & 0x7fU)) >> 7);
      return uVar3 & 0xffff;
    }
  }
  return 0xffff;
}

