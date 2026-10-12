
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_1001f750(void)

{
  int iVar1;
  undefined1 local_8 [4];
  uint local_4;
  
  FUN_1002c630(0);
  FUN_1002c290(0);
  FUN_1002c390((&PTR_s_O__PLAYROOM__1005a690)[*DAT_10070f24 * 6],s_MAP_PSX_BIN_10058a68,local_8,8);
  _DAT_10070d20 = local_4;
  iVar1 = FUN_1002c3c0(&DAT_10070ed4,(local_4 & 0xfffff800) + 0x1000,0);
  if (iVar1 == 0) {
    return 0xffffffff;
  }
  DAT_10070f24[0x128] = DAT_10070f24[0x128] | 2;
  FUN_1002c390((&PTR_s_O__PLAYROOM__1005a690)[*DAT_10070f24 * 6],s_MAP_PSX_BIN_10058a68,DAT_10070ed4
               ,local_4);
  *(undefined4 *)(DAT_10070f24 + 0x90) = *DAT_10070ed4;
  if (*(int *)(DAT_10070f24 + 0x90) != *(int *)(DAT_10070f24 + 0x8c)) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  FUN_1002c5a0(2);
  iVar1 = FUN_1001fc90();
  FUN_1002c630(0);
  FUN_1002c4a0(DAT_10070ed4,iVar1,0,0);
  return 0;
}

