
uint FUN_10018c50(void)

{
  bool bVar1;
  uint uVar2;
  undefined3 extraout_var;
  byte bVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  uVar6 = DAT_1006feb4 & 0x10;
  uVar2 = DAT_1006feb4 & 0x20;
  uVar5 = uVar2;
  if ((0x65 < DAT_1006febc) && (DAT_1006febc < 0x68)) {
    uVar5 = 0x20;
  }
  DAT_1006feb4 = DAT_1006feb4 & 0xffef;
  uVar4 = (uint)DAT_1006feb4;
  if (DAT_1006feb4 < 2) {
    uVar6 = uVar6 + 1;
  }
  if (((DAT_1006feb4 == 0) || (*(char *)(DAT_10072860 + 0xec) != '\0')) || (DAT_1006efb4 == '\x04'))
  goto LAB_10018e4f;
  switch(DAT_10072108) {
  case 0:
  case 1:
  case 3:
  case 6:
  case 8:
  case 0xb:
  case 0xc:
  case 0xe:
  case 0x29:
  case 0x2a:
    if ((uVar6 != 0) || (DAT_1007201c != 0)) goto switchD_10018cd9_caseD_9;
    if ((*(byte *)(DAT_10070f24 + 0x30) & 2) == 0) {
      DAT_1006ef84 = 1;
      bVar3 = DAT_1006feb8 & 7;
      if ((*(byte *)(DAT_10072860 + 0x94) & 0x10) != 0) {
        bVar3 = bVar3 ^ 2;
      }
      if ((bVar3 & 2) == 0) {
        if ((uVar5 == 0) || (bVar1 = FUN_100196f0(), CONCAT31(extraout_var,bVar1) != 1)) {
          DAT_10072108 = 0x11;
          *(undefined **)(DAT_10072860 + 100) = &DAT_100546d8;
        }
        else {
          DAT_10072108 = 0x11;
          *(undefined **)(DAT_10072860 + 100) = &DAT_10054718;
        }
      }
      else {
        DAT_10072108 = 0x11;
        *(undefined **)(DAT_10072860 + 100) = &DAT_10054750;
      }
      *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
      *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
      FUN_10015d00();
      DAT_10072194 = 0;
      goto LAB_10018dda;
    }
    break;
  case 2:
  case 4:
  case 5:
  case 7:
  case 0x13:
    if ((*(byte *)(DAT_10070f24 + 0x30) & 2) != 0) break;
    *(undefined **)(DAT_10072860 + 100) = &DAT_10054798;
    *(undefined2 *)(DAT_10072860 + 0x32) = 0xffff;
    *(undefined2 *)(DAT_10072860 + 0x30) = 0xffff;
    DAT_1006ef7c = 0;
LAB_10018dda:
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
    uVar4 = (uint)DAT_1006feb4;
  default:
switchD_10018cd9_caseD_9:
    break;
  case 0x11:
  case 0x16:
  case 0x19:
  case 0x1b:
    return (uint)*(ushort *)(DAT_10070f24 + 0x24);
  }
  if (((*(byte *)(DAT_10070f24 + 0x30) & 6) == 0) &&
     (*(short *)(DAT_10070f24 + 0x24) =
           *(short *)(DAT_10070f24 + 0x24) -
           (short)*(undefined4 *)
                   (&DAT_10055348 + ((uVar4 - 1 & 7) + (*(byte *)(DAT_10070f24 + 0xe) & 3) * 8) * 4)
     , *(short *)(DAT_10070f24 + 0x24) < 1)) {
    *(undefined2 *)(DAT_10070f24 + 0x24) = 0;
  }
  uVar2 = DAT_10072860;
  DAT_1006feb4 = 0;
  DAT_1006ef94._0_2_ = 0;
  *(undefined1 *)(DAT_10072860 + 0xec) = 0x3c;
  if (uVar6 != 0) {
    *(undefined4 *)(DAT_10072860 + 0xa8) = 0;
  }
LAB_10018e4f:
  return CONCAT22((short)(uVar2 >> 0x10),*(undefined2 *)(DAT_10070f24 + 0x24));
}

