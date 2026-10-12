
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10015780(void)

{
  int iVar1;
  char *pcVar2;
  char cVar3;
  ushort uVar4;
  uint uVar5;
  int iVar6;
  
  if (*DAT_10070f24 != '\t') {
    if ((DAT_10072110 != 0) || (DAT_10072170 != 0)) {
      *(ushort *)(DAT_10070f24 + 0x5c) =
           *(ushort *)(DAT_10070f24 + 0x5c) &
           ~(*(short *)(DAT_10070f24 + 0x72) + *(short *)(DAT_10070f24 + 0x70) +
             *(short *)(DAT_10070f24 + 0x6e) + *(short *)(DAT_10070f24 + 0x6c) + 0x40U);
      *(ushort *)(DAT_10070f24 + 0x5e) =
           *(ushort *)(DAT_10070f24 + 0x5e) &
           ~(*(short *)(DAT_10070f24 + 0x72) + *(short *)(DAT_10070f24 + 0x70) +
             *(short *)(DAT_10070f24 + 0x6e) + *(short *)(DAT_10070f24 + 0x6c) + 0x40U);
    }
    DAT_1006efa0 = (DAT_1006efa0 & 7) - 1;
    if ((int)DAT_1006efa0 < 0) {
      *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
      DAT_1006efa0 = 0;
    }
  }
  if (*(char *)(DAT_1007217c + 0xec) == '\0') {
    *(undefined1 *)(DAT_1007217c + 0xed) = 0;
  }
  else {
    *(char *)(DAT_1007217c + 0xec) = *(char *)(DAT_1007217c + 0xec) + -1;
  }
  uVar5 = FUN_1002a170(*DAT_10072860,DAT_10072860[1],*(int *)(DAT_1007217c + 8));
  FUN_1002ff50(uVar5);
  if (DAT_1006ef8c != 0) {
    DAT_1006ef8c = DAT_1006ef8c + -1;
  }
  if (DAT_1006ef88 == 0) {
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) | 0x80000000;
  }
  else {
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0x7fffffff;
    DAT_1006feb4 = 0;
  }
  *(char *)(DAT_1007217c + 0x21) = (char)((DAT_10072860[2] >> 0x10) + 0x40 >> 7);
  cVar3 = DAT_1006efb4;
  DAT_10072140 = 5;
  if ((DAT_10070f24[0x30] & 0x20U) != 0) {
    DAT_10072144 = 5;
  }
  if ((DAT_10070f24[0x30] & 0x40U) != 0) {
    _DAT_10072148 = 5;
  }
  if ((DAT_10070f24[0x30] & 0x80U) != 0) {
    _DAT_1007214c = 5;
  }
  if ((*(uint *)(DAT_10070f24 + 0x30) & 0x100) != 0) {
    DAT_10072150 = 5;
  }
  iVar6 = (int)DAT_1006efb4;
  iVar1 = (&DAT_10072140)[iVar6];
  (&DAT_10072140)[iVar6] = iVar1 + -1;
  if (iVar1 + -1 < 0) {
    (&DAT_10072140)[iVar6] = (int)*(short *)(&DAT_10055320 + iVar6 * 2);
  }
  DAT_10072008 = 1;
  if (cVar3 == '\x04') {
    DAT_10072008 = 0;
    DAT_1006ef64 = DAT_1006ef64 - 1;
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xfffffffc;
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) | 4;
    *(undefined2 *)(DAT_1007217c + 0x4a) = *(undefined2 *)(&DAT_100552a8 + (DAT_1006ef64 & 0xf) * 2)
    ;
    *(undefined1 *)(DAT_10072860 + 0x3b) = 0;
    if (DAT_10072150 != 0) goto LAB_100159a1;
    *(short *)(DAT_10070f24 + 0x2c) = *(short *)(DAT_10070f24 + 0x2c) + -1;
    if (0 < *(short *)(DAT_10070f24 + 0x2c)) goto LAB_100159a1;
    pcVar2 = DAT_10070f24;
    pcVar2[0x2c] = '\0';
    pcVar2[0x2d] = '\0';
    DAT_1006efb4 = '\0';
  }
  else if ((char)DAT_10072860[0x3b] != '\0') {
    DAT_10072008 = 0;
    *(char *)(DAT_10072860 + 0x3b) = (char)DAT_10072860[0x3b] + -1;
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xfffffffc;
    *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) | 5;
    *(undefined2 *)(DAT_1007217c + 0x4a) =
         *(undefined2 *)(&DAT_100552d0 + (DAT_1006ef94 & 0xffff) * 2);
    DAT_1006ef94._0_2_ = (short)DAT_1006ef94 + 1;
    if (*(short *)(DAT_1007217c + 0x4a) == 0) {
      FUN_10015670();
    }
    if (*(short *)(DAT_1007217c + 0x4a) == 0xfe) {
      *(undefined2 *)(DAT_1007217c + 0x4a) =
           *(undefined2 *)(&DAT_100552d0 + (DAT_1006ef94 & 0xffff) * 2);
      DAT_1006ef94._0_2_ = (short)DAT_1006ef94 + 1;
      *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xfffffffc;
      *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) | 4;
    }
    goto LAB_100159a1;
  }
  FUN_10015670();
LAB_100159a1:
  FUN_10012040();
  uVar4 = DAT_10072168;
  if ((ushort)DAT_1006ef90 != 0) {
    DAT_1006ef90._0_2_ = (ushort)DAT_1006ef90 + 1;
    DAT_100717b4 = *(int *)(&DAT_100551e8 + (uint)(ushort)DAT_1006ef90 * 4);
    if (DAT_100717b4 == 0) {
      DAT_1006ef90 = (uint)DAT_1006ef90._2_2_ << 0x10;
    }
  }
  DAT_10072168 = 0;
  _DAT_10072012 = _DAT_10072012 & ~uVar4;
  if ((DAT_1007211c != 0) &&
     ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0)) {
    DAT_1007211c = 0;
  }
  if (DAT_1006ef80 != 0) {
    if ((DAT_10070f24[0x5c] & 0x40U) == 0) {
      DAT_1006ef80 = 0;
    }
    else {
      DAT_1006ef80 = DAT_1006ef80 + -1;
    }
  }
  if ((DAT_1007216c != 0) && (DAT_10072194 < 0x81)) {
    DAT_1007216c = 0;
  }
  DAT_1007215c = DAT_1007215c + -1;
  if (DAT_1007215c < '\0') {
    DAT_1007215c = '\0';
  }
  if (DAT_1006efa4 != 0) {
    DAT_1006efa4 = DAT_1006efa4 + -1;
  }
  if (((((*(uint *)(DAT_10070f24 + 0x30) & 0x8000) != 0) && (DAT_1007201c == 0)) &&
      (DAT_10072108 != 0x28)) && (DAT_10072108 != 0x1b)) {
    if (DAT_10072158 < 0) {
      DAT_10072158 = DAT_10072158 + 1;
    }
    else if ((DAT_10070f24[0x5d] & 3U) != 0) {
      *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
      DAT_10072108 = 0x1b;
      *(undefined2 *)(DAT_10072860 + 0xd) = 1;
      *(undefined1 *)((int)DAT_10072860 + 0x22) = 1;
      DAT_1007201c = 0;
      DAT_10072158 = 0;
      DAT_10072160 = (-(uint)((*(ushort *)(DAT_10070f24 + 0x5c) & 0x100) != 0) & 0xffe80000) +
                     0x200000;
      DAT_10072860[0x2f] = DAT_10072160;
      DAT_10072860[0x2b] = DAT_10072160;
      DAT_10072860[0x2e] = 0;
      DAT_10072860[0x2a] = 0;
      DAT_10072860[0x30] = 0x10000;
      DAT_10072860[0x2c] = 0x10000;
      FUN_10029620((int)DAT_10072860);
      FUN_1001a410();
      return;
    }
  }
  if ((((DAT_10070f24[0x30] & 1U) == 0) && (*(short *)(DAT_10070f24 + 0x24) == 0)) &&
     (DAT_10072110 == 0)) {
    DAT_10072110 = 1;
  }
  if ((DAT_10072014 != 0) && (DAT_10072110 = 0, *(short *)(DAT_10070f24 + 0x24) < 0xb)) {
    pcVar2 = DAT_10070f24;
    pcVar2[0x24] = '\n';
    pcVar2[0x25] = '\0';
  }
  if (DAT_10072110 == 0) {
    uVar5 = FUN_10018c50();
    if (((short)uVar5 == 0) && ((DAT_10070f24[0x30] & 1U) == 0)) {
      DAT_10072110 = 1;
    }
  }
  DAT_10072860[0x27] = *DAT_10072860;
  DAT_10072860[0x28] = DAT_10072860[1];
  DAT_10072860[0x29] = DAT_10072860[2];
  _DAT_10072180 = DAT_10072860[0x2b];
  (**(code **)(&DAT_10072040 + (DAT_10072108 & 0x7fff) * 4))();
  return;
}


