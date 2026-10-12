
void FUN_100171b0(void)

{
  int iVar1;
  
  FUN_100174e0();
  if (DAT_10072110 != 0) {
LAB_10017241:
    FUN_10029da0(DAT_1007217c);
    if ((*(short *)(DAT_10072860 + 0x34) != 0x1c7) && (*(short *)(DAT_10072860 + 0x34) != 0x1c8)) {
      FUN_10019d70();
    }
    if (*(short *)(DAT_10072860 + 0x34) == 0x1c9) {
      FUN_10029620(DAT_10072860);
    }
    FUN_10015d00();
    return;
  }
  iVar1 = FUN_10017810();
  if (iVar1 == 0) {
    iVar1 = FUN_100188a0();
    if (iVar1 == 0) {
      FUN_10017290();
      if ((DAT_1006ef70 < 0x2000) &&
         (((DAT_10072108 == 4 || (DAT_10072108 == 2)) &&
          ((*(ushort *)(DAT_10070f24 + 0x6e) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0)))) {
        *(int *)(DAT_10072860 + 0xb8) = *(int *)(DAT_10072860 + 0xb8) + -0xc000;
        if (*(int *)(DAT_10072860 + 0xb8) < 0) {
          *(undefined4 *)(DAT_10072860 + 0xb8) = 0;
        }
      }
      goto LAB_10017241;
    }
  }
  return;
}

