
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl FUN_10019b40(ushort param_1)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  uVar1 = param_1 & _DAT_10072012;
  if ((uVar1 & 2) == 0) {
    if ((uVar1 & 1) != 0) {
      DAT_10072168._0_1_ = (byte)DAT_10072168 | 1;
      return *(int *)(&DAT_100553e8 + *(char *)(DAT_10070f24 + 0xe) * 4);
    }
    if ((uVar1 & 4) != 0) {
      DAT_10072168._0_1_ = (byte)DAT_10072168 | 4;
      return *(int *)(&DAT_100553f8 + *(char *)(DAT_10070f24 + 0xe) * 4);
    }
    if ((uVar1 & 8) != 0) {
      DAT_10072168._0_1_ = (byte)DAT_10072168 | 8;
      return *(int *)(&DAT_10055408 + *(char *)(DAT_10070f24 + 0xe) * 4);
    }
    if ((uVar1 & 0x10) != 0) {
      DAT_10072168._0_1_ = (byte)DAT_10072168 | 0x10;
      return *(int *)(&DAT_10055418 + *(char *)(DAT_10070f24 + 0xe) * 4);
    }
    if ((uVar1 & 0x40) != 0) {
      DAT_10072168._0_1_ = (byte)DAT_10072168 | 0x40;
      return *(int *)(&DAT_10055428 + *(char *)(DAT_10070f24 + 0xe) * 4);
    }
    iVar2 = 0;
  }
  else {
    DAT_10072168._0_1_ = (byte)DAT_10072168 | 2;
    iVar4 = (int)DAT_10072194;
    iVar3 = *(char *)(DAT_10070f24 + 0xe) * 4;
    iVar2 = *(int *)(&DAT_100553f8 + iVar3);
    if ((iVar2 <= iVar4) && (iVar2 = *(int *)(&DAT_10055438 + iVar3), iVar4 <= iVar2)) {
      return iVar4;
    }
  }
  return iVar2;
}

