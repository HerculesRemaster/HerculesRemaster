
void FUN_00401610(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  FUN_004063c0(s_UnLoadAllSfxMusic_00435d2c);
  FUN_004015d0();
  iVar1 = 0;
  iVar3 = 0x2b0;
  iVar2 = 0x13c;
  do {
    if (*(short *)(PTR_DAT_004361bc + iVar3) != -1) {
      FUN_0040bfe0(iVar1);
      FUN_00404750(*(undefined4 *)(PTR_DAT_004361bc + iVar2));
      *(undefined2 *)(PTR_DAT_004361bc + iVar3) = 0xffff;
    }
    iVar2 = iVar2 + 4;
    iVar1 = iVar1 + 1;
    iVar3 = iVar3 + 2;
  } while (iVar2 < 0x1bc);
  iVar1 = 0;
  do {
    FUN_0040c140(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x18);
  FUN_0040be30();
  FUN_0040c2d0(0);
  FUN_0040be30();
  FUN_0040be30();
  FUN_0040ac10(3);
  return;
}

