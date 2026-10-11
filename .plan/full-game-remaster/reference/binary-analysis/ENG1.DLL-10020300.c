
/* one game tick, called from PC_DLLEngineMain */

void MOD_Tick(void)

{
  char *pcVar1;
  undefined4 uVar2;
  int iVar3;
  bool bVar4;
  
  if (DAT_1006fcec == 0) {
    FUN_1002c860(8,8);
    DAT_10070f24[1] = '\0';
    DAT_10070f24[2] = '\0';
    DAT_10070f24[10] = '\x01';
    *(undefined4 *)(DAT_10070f24 + 0x8c) = DAT_10058b00;
    *(undefined4 *)(DAT_10070f24 + 0x94) = DAT_10058af8;
    builtin_strncpy(DAT_10070f24 + 0x48,"GeT\x13",4);
    pcVar1 = DAT_10070f24;
    pcVar1[0x4c] = -0x48;
    pcVar1[0x4d] = -0x66;
    pcVar1[0x4e] = -0x55;
    pcVar1[0x4f] = -0x14;
    FUN_1002c350();
    FUN_100083d3();
    FUN_100082b7(0);
    FUN_1002c3b0((&PTR_DAT_1005a6a4)[*DAT_10070f24 * 6]);
    FUN_1002cc30(1);
    FUN_1000827f(1);
    DAT_1006fce8 = '\0';
    FUN_1001f0f0();
    FUN_1001f750();
    FUN_10032b10();
    FUN_100255f0();
    FUN_10005ea0();
    FUN_1002c840((int)*DAT_10070f24,0);
    goto LAB_100203fe;
  }
  if (DAT_1006fcec == 1) {
    while( true ) {
      FUN_1002c270();
      if (*DAT_10070f24 == '\x03') {
        FUN_1000e670();
      }
      uVar2 = FUN_1002ca40(DAT_10070f30 + 0x400);
      *(undefined4 *)(DAT_10070f24 + 0x34) = uVar2;
      FUN_1002c420();
      FUN_10006005();
      FUN_10002180();
      FUN_1001f870(1);
      FUN_1002c440();
      FUN_10005600();
      if (DAT_10070f24[4] == '\0') {
        FUN_1002fb70();
        FUN_1002f840();
        FUN_100258e0();
        FUN_100034d0();
        FUN_100156a0();
        FUN_1001fd90();
        FUN_10025d00();
      }
      else {
        FUN_100034d0();
        FUN_100256e0();
      }
      FUN_10003470();
      FUN_10005d50();
      FUN_10003a50();
      if (*(int *)(DAT_10070f24 + 0x40) == 0) {
        FUN_10032bd0();
      }
      if (*(int *)(DAT_10070f24 + 0x40) == 0) {
        FUN_1002da90();
      }
      FUN_1001f6f0();
      FUN_1002c630(0);
      if (DAT_1006ff88 < *(int *)(DAT_10070f24 + 0x3c) + 1U) {
        FUN_1002cc30(0);
      }
      DAT_10071ec0 = 0;
      uVar2 = FUN_1002cc30(0);
      *(undefined4 *)(DAT_10070f24 + 0x54) = uVar2;
      DAT_10071ec0 = 1;
      *(uint *)(DAT_10070f24 + 0x3c) = DAT_1006ff88;
      FUN_1002c570(DAT_10071ec4);
      if (DAT_10070f24[4] == '\0') {
        FUN_10002cb0();
      }
      FUN_100050d0();
      FUN_100058b0();
      FUN_1002c620(DAT_10071ec4 + 0x88);
      FUN_1000666b();
      FUN_1002ca40(*(undefined4 *)(DAT_10070f24 + 0x34));
      bVar4 = DAT_10071ec4 == DAT_10071ec8;
      DAT_10071ec4 = DAT_10071ec8;
      if (bVar4) {
        DAT_10071ec4 = DAT_10071ec8 + 0x1488;
      }
      *(int *)(DAT_10070f24 + 0x38) = *(int *)(DAT_10070f24 + 0x38) + 1;
      iVar3 = FUN_100060f9();
      if (iVar3 == 0) break;
LAB_100203fe:
      FUN_1001f320();
      FUN_1002c5a0(4);
      FUN_1002c360();
      if (DAT_1006fce8 == '\0') {
        FUN_1002c540(0,0);
      }
      DAT_1006fce8 = '\x01';
      DAT_10071ec0 = 0;
      FUN_1002cc40(&LAB_1001f0e0);
      DAT_1007286c = 8;
      DAT_1004d858._0_2_ = 0x80;
      DAT_1006ee80 = 0;
      FUN_1002c4e0(0);
      pcVar1 = DAT_10070f24;
      pcVar1[0x1b30] = '\0';
      pcVar1[0x1b31] = '\x01';
      pcVar1[0x1b32] = '\0';
      pcVar1[0x1b33] = '\0';
      FUN_1002c520(1);
      *(uint *)(DAT_10070f24 + 0x3c) = DAT_1006ff88;
      DAT_10070f24[4] = '\0';
      DAT_10070f24[5] = '\x01';
      pcVar1 = DAT_10070f24;
      pcVar1[0x40] = '\0';
      pcVar1[0x41] = '\0';
      pcVar1[0x42] = '\0';
      pcVar1[0x43] = '\0';
      DAT_10070f24[3] = '\x05';
      uVar2 = FUN_1002cc30(1);
      *(undefined4 *)(DAT_10070f24 + 0x54) = uVar2;
      FUN_10001000();
      DAT_1006fcec = 1;
    }
    return;
  }
  return;
}

