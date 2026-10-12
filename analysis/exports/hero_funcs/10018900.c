
void FUN_10018900(void)

{
  uint uVar1;
  
  if ((char)DAT_10072860[0x25] == '\0') {
    *DAT_10072860 = *DAT_10072860 & 0xffc00000;
    uVar1 = *DAT_10072860 | 0x200000;
  }
  else {
    *DAT_10072860 = *DAT_10072860 & 0xffc00000;
    uVar1 = *DAT_10072860 | 0x100000;
  }
  *DAT_10072860 = uVar1;
  *DAT_10072860 = *DAT_10072860 + 0x80000;
  DAT_10072860[1] = (DAT_10072860[1] - 0x80000 & 0xfc00000) + 0x180000;
  *(uint *)(DAT_1007217c + 0x54) = *(uint *)(DAT_1007217c + 0x54) & 0xff7fffff;
  DAT_10072860[0x31] = 0;
  DAT_10072860[0x30] = 0;
  DAT_10072860[0x2f] = 0;
  DAT_10072860[0x2e] = 0;
  DAT_10072860[0x2b] = 0;
  DAT_10072860[0x2a] = 0;
  DAT_1006ef7c = 0;
  DAT_1007211c = 0;
  DAT_10072108 = 0x10;
  DAT_10072860[0x19] = (uint)&DAT_10054680;
  *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
  *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
  FUN_10015d00();
  return;
}

