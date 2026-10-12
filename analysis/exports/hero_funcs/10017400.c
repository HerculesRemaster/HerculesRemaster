
void FUN_10017400(void)

{
  bool bVar1;
  int iVar2;
  undefined3 extraout_var;
  short local_c [6];
  
  FUN_10019d70();
  if (DAT_10072110 == 0) {
    if (DAT_1006ef70 != 0x12) {
      iVar2 = FUN_100188a0();
      if (iVar2 != 0) {
        return;
      }
      iVar2 = FUN_10017810();
      if (iVar2 != 0) {
        return;
      }
    }
    FUN_10017290();
    if ((((*DAT_10070f24 == '\x03') && (DAT_10072118 != (undefined4 *)0x0)) && (DAT_10072120 == 1))
       && ((iVar2 = FUN_1002a880(DAT_10072118,local_c,3), iVar2 != 0 &&
           (bVar1 = FUN_1002a520(*DAT_10072860,DAT_10072860[1],local_c),
           CONCAT31(extraout_var,bVar1) != 0)))) {
      DAT_10072120 = 2;
      DAT_10072108 = 0xd;
      return;
    }
  }
  iVar2 = FUN_10029e20(DAT_1007217c);
  if (iVar2 == 0) {
    FUN_100174e0();
    FUN_10015d00();
    return;
  }
  FUN_100176e0();
  return;
}

