
/* one game tick, called from PC_DLLEngineMain */

void MOD_Tick(void)

{
  char *pcVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 extraout_ECX;
  bool bVar4;
  
  if (DAT_1003fb84 == 0) {
    FUN_1001fea0(8,8);
    DAT_100407a4[1] = '\0';
    DAT_100407a4[2] = '\0';
    DAT_100407a4[10] = '\x03';
    *(undefined4 *)(DAT_100407a4 + 0x8c) = DAT_10038518;
    *(undefined4 *)(DAT_100407a4 + 0x94) = DAT_10038510;
    builtin_strncpy(DAT_100407a4 + 0x48,"GeT\x13",4);
    pcVar1 = DAT_100407a4;
    pcVar1[0x4c] = -0x48;
    pcVar1[0x4d] = -0x66;
    pcVar1[0x4e] = -0x55;
    pcVar1[0x4f] = -0x14;
    FUN_1001f9e0();
    FUN_1000d27a();
    FUN_1000d15e(0);
    FUN_1001fa40(*(undefined4 *)(&DAT_1003853c + *DAT_100407a4 * 0x18));
    FUN_10020170(1);
    FUN_1000d126(1);
    DAT_1003fb80 = '\0';
    FUN_1001a330();
    FUN_1001a930();
    FUN_10025060();
    FUN_1001b8f0();
    FUN_1000ae00();
    FUN_1001fe80((int)*DAT_100407a4,0);
    goto LAB_1001acae;
  }
  if (DAT_1003fb84 == 1) {
    while( true ) {
      uVar2 = FUN_10020030(DAT_100407b0 + 0x400);
      *(undefined4 *)(DAT_100407a4 + 0x34) = uVar2;
      FUN_1001fa70();
      FUN_1000af65();
      FUN_10007570();
      FUN_1001f920();
      FUN_1001fa90();
      FUN_10020180(&LAB_1001a320);
      FUN_1000acb0();
      FUN_1000a700();
      FUN_10008540();
      if (DAT_100407a4[4] == '\0') {
        FUN_100222c0(extraout_ECX);
        FUN_10022070();
        FUN_10020190();
        FUN_10008b20();
        FUN_1001b9f0();
        FUN_100085a0();
        FUN_100174e0();
        FUN_10001880();
        FUN_1001bd40();
      }
      else {
        FUN_10020190();
        FUN_10008b20();
        FUN_100085a0();
      }
      FUN_100250f0();
      FUN_1001fc40(0);
      if (DAT_1003fc70 < *(int *)(DAT_100407a4 + 0x3c) + 1U) {
        FUN_10020170(0);
      }
      DAT_10041444 = 0;
      FUN_10020170(0);
      DAT_10041444 = 1;
      *(uint *)(DAT_100407a4 + 0x3c) = DAT_1003fc70;
      uVar2 = FUN_10020170(1);
      *(undefined4 *)(DAT_100407a4 + 0x54) = uVar2;
      FUN_1001fb80(DAT_10041448);
      if (DAT_100407a4[4] == '\0') {
        FUN_10008050();
      }
      FUN_1000a1d0();
      FUN_1000aa70();
      FUN_1001fc30(DAT_10041448 + 0x88);
      FUN_1000b512();
      FUN_10020030(*(undefined4 *)(DAT_100407a4 + 0x34));
      bVar4 = DAT_10041448 == DAT_1004144c;
      DAT_10041448 = DAT_1004144c;
      if (bVar4) {
        DAT_10041448 = DAT_1004144c + 0x2088;
      }
      *(int *)(DAT_100407a4 + 0x38) = *(int *)(DAT_100407a4 + 0x38) + 1;
      iVar3 = FUN_1000b059();
      if (iVar3 == 0) break;
LAB_1001acae:
      FUN_1001a560();
      FUN_1001fbb0(4);
      FUN_1001f9f0();
      if (DAT_1003fb80 == '\0') {
        FUN_1001fb60(0,0);
      }
      DAT_1003fb80 = '\x01';
      DAT_10041444 = 0;
      FUN_10020180(&LAB_1001a320);
      DAT_1004254c = 8;
      DAT_10035168 = 0x80;
      DAT_1003fa40 = 0;
      FUN_1001fb10(0);
      pcVar1 = DAT_100407a4;
      pcVar1[0x1b30] = '\0';
      pcVar1[0x1b31] = '\x01';
      pcVar1[0x1b32] = '\0';
      pcVar1[0x1b33] = '\0';
      FUN_1001fb40(1);
      *(uint *)(DAT_100407a4 + 0x3c) = DAT_1003fc70;
      pcVar1 = DAT_100407a4;
      pcVar1[0x40] = '\0';
      pcVar1[0x41] = '\0';
      pcVar1[0x42] = '\0';
      pcVar1[0x43] = '\0';
      DAT_100407a4[4] = '\0';
      DAT_100407a4[5] = '\x01';
      DAT_100407a4[3] = '\x05';
      uVar2 = FUN_10020170(1);
      *(undefined4 *)(DAT_100407a4 + 0x54) = uVar2;
      FUN_10001000();
      DAT_1003fb84 = 1;
    }
    return;
  }
  return;
}

