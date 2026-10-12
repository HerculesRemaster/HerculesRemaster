
void FUN_0040cab0(void)

{
  char cVar1;
  bool bVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  if (((((DAT_00ba01f0 != '\0') && (g_framePtr != 0)) && (DAT_00a909b8 < 0x140)) &&
      ((DAT_00a909c8 < DAT_0043a48c && (iVar5 = DAT_00a909b8 + (short)DAT_00a90a0c, -1 < iVar5))))
     && (iVar6 = DAT_00a909c8 + DAT_00a90a10, -1 < iVar6)) {
    if (DAT_00a909b8 < 0) {
      DAT_00a90a0c = DAT_00a90a0c + (short)DAT_00a909b8;
      DAT_00a909d8 = DAT_00a909d8 - DAT_00a909b8;
      DAT_00a909b8 = 0;
    }
    if (DAT_00a909c8 < 0) {
      DAT_00a90a10 = DAT_00a90a10 + (short)DAT_00a909c8;
      DAT_00a909e8 = DAT_00a909e8 - DAT_00a909c8;
      DAT_00a909c8 = 0;
    }
    if (0x13f < iVar5) {
      DAT_00a90a0c = DAT_00a90a0c + (0x140 - (short)iVar5);
    }
    if (DAT_0043a48c <= iVar6) {
      DAT_00a90a10 = DAT_00a90a10 + ((short)DAT_0043a48c - (short)iVar6);
    }
    if ((0 < (short)DAT_00a90a0c) && (0 < DAT_00a90a10)) {
      DAT_00a90944 = DAT_00a909c8 * g_framePitch + DAT_00a909b8 + g_framePtr;
      DAT_00a909fc = DAT_00a909fc + DAT_00a909d8;
      uVar3 = DAT_00a909e8 << 8;
      do {
        iVar6 = DAT_00a909fc + uVar3;
        uVar4 = (uint)DAT_00a90a0c;
        iVar7 = DAT_00a90944 + uVar4;
        iVar5 = -uVar4;
        do {
          cVar1 = *(char *)(iVar6 + uVar4 + iVar5);
          if (cVar1 != '\0') {
            *(char *)(iVar7 + iVar5) = cVar1;
          }
          bVar2 = iVar5 < -1;
          iVar5 = iVar5 + 1;
        } while (bVar2);
        DAT_00a90a10 = DAT_00a90a10 + -1;
        DAT_00a90944 = DAT_00a90944 + g_framePitch;
        uVar3 = uVar3 + 0x100 & 0xffff;
      } while (0 < DAT_00a90a10);
      DAT_0043a590 = 1;
    }
  }
  return;
}


