
void FUN_004070c0(char param_1)

{
  uint uVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  
  if (param_1 != -1) {
    DAT_00436a0c = CONCAT31(DAT_00436a0c._1_3_,param_1);
    return;
  }
  if ((((char)DAT_00436a0c != -0x80) && (DAT_00ba0210 != 0)) && (DAT_00ba01f0 != '\0')) {
    iVar3 = 0;
    iVar4 = 0xf0;
    do {
      uVar1 = 0;
      do {
        pbVar2 = (byte *)(iVar3 + uVar1 + DAT_00ba0210);
        uVar1 = uVar1 + 1;
        *pbVar2 = (&DAT_0091c488)[(uint)*pbVar2 + (DAT_00436a0c & 0xff) * 0x100];
      } while (uVar1 < 0x140);
      iVar3 = iVar3 + DAT_00ba0214;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  return;
}

