
undefined4 FUN_1001a930(void)

{
  uint uVar1;
  int iVar2;
  
  FUN_1001fc40(0);
  FUN_1001f940(0);
  uVar1 = FUN_1001f980(*(undefined4 *)(&DAT_10038528 + *DAT_100407a4 * 0x18),s_MAP_PSX_BIN_100381c8)
  ;
  iVar2 = FUN_1001fa50(&DAT_10040790,(uVar1 & 0xfffff800) + 0x1000,0);
  if (iVar2 == 0) {
    return 0xffffffff;
  }
  DAT_100407a4[0x128] = DAT_100407a4[0x128] | 2;
  FUN_1001fa20(*(undefined4 *)(&DAT_10038528 + *DAT_100407a4 * 0x18),s_MAP_PSX_BIN_100381c8,
               DAT_10040790,uVar1);
  *(undefined4 *)(DAT_100407a4 + 0x90) = *DAT_10040790;
  if (*(int *)(DAT_100407a4 + 0x90) != *(int *)(DAT_100407a4 + 0x8c)) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  FUN_1001fbb0(2);
  iVar2 = FUN_1001aa30();
  FUN_1001fc40(0);
  FUN_1001fad0(DAT_10040790,iVar2,0,0);
  return 0;
}

