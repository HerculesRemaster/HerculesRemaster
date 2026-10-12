
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0040ac60(int *param_1)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  byte bVar6;
  byte bVar7;
  short sVar8;
  short sVar9;
  short sVar10;
  short sVar11;
  short sVar12;
  byte bVar16;
  byte bVar17;
  int *piVar18;
  short sVar13;
  short sVar14;
  short sVar15;
  
  if (DAT_00ba01fc == '\0') {
    if (DAT_0096e698 != '\0') {
      FUN_00406ee0(DAT_0096e699,DAT_0096e69a,DAT_0096e69b);
    }
    FUN_00406ff0();
  }
  piVar18 = param_1 + 1;
LAB_0040aca2:
  do {
    param_1 = (int *)*param_1;
    if (param_1 == piVar18) {
      do {
        param_1 = (int *)*param_1;
        piVar18 = piVar18 + 1;
      } while (param_1 == piVar18);
    }
    if (param_1 == (int *)0x0) {
      if (DAT_00ba01fc == '\0') {
        FUN_004070c0(0xff);
        FUN_00407140();
        FUN_00406620();
      }
      FUN_0040bd80();
      return;
    }
    bVar16 = *(byte *)((int)param_1 + 7);
    bVar17 = bVar16 & 0xe0;
    if (bVar17 != 0x20) {
      if (bVar17 != 0x60) {
        if ((bVar17 != 0x40) && ((bVar17 == 0xe0 || ((bVar16 & 0xe0) == 0)))) {
          if (bVar16 == 0xe1) {
            DAT_00a8e6fc = (uint)((param_1[1] & 0x180U) == 0x180);
            DAT_00a8e6f4 = param_1[1] & 0x1f;
            DAT_00a8e6f8 = (uint)param_1[1] >> 5 & 3;
          }
          else if (bVar16 == 1) {
            FUN_0040a8f0(param_1 + 4,(int)(short)param_1[3],(int)*(short *)((int)param_1 + 0xe));
          }
          else if (bVar16 == 0xf3) {
            _DAT_00a909b0 = (int)(short)param_1[1];
            DAT_00a909b4 = (uint)*(byte *)((int)param_1 + 6);
            if (0xef < DAT_00a909b4) {
              DAT_00a909b4 = DAT_00a909b4 - 0xf0;
            }
            _DAT_0043a488 = (short)param_1[2] + _DAT_00a909b0;
            DAT_0043a48c = (int)*(short *)((int)param_1 + 10) + DAT_00a909b4;
            if (_DAT_00a909b0 < 0) {
              _DAT_00a909b0 = 0;
            }
            if (0x140 < _DAT_00a909b0) {
              _DAT_00a909b0 = 0x140;
            }
            if (_DAT_0043a488 < 0) {
              _DAT_0043a488 = 0;
            }
            if (0x140 < _DAT_0043a488) {
              _DAT_0043a488 = 0x140;
            }
            if ((int)DAT_00a909b4 < 0) {
              DAT_00a909b4 = 0;
            }
            if (0xf0 < (int)DAT_00a909b4) {
              DAT_00a909b4 = 0xf0;
            }
            if (DAT_0043a48c < 0) {
              DAT_0043a48c = 0;
            }
            if (0xf0 < DAT_0043a48c) {
              DAT_0043a48c = 0xf0;
            }
          }
        }
        goto LAB_0040aca2;
      }
      if (DAT_00ba01fc != '\0') goto LAB_0040aca2;
      DAT_00a909fc = &DAT_0096e6e0 + (DAT_00a8e6f4 >> 1) * 0x10000;
      if (DAT_00a8e6fc != 0) {
        DAT_00a909fc = &DAT_00a6e6e0;
      }
      bVar16 = bVar16 & 0xfd;
      if (bVar16 == 0x7c) {
        DAT_00a909b8 = (int)(short)param_1[2];
        DAT_00a909c8 = (int)*(short *)((int)param_1 + 10);
        DAT_00a909d8 = (uint)*(byte *)(param_1 + 3);
        DAT_00a909e8 = (uint)*(byte *)((int)param_1 + 0xd);
        DAT_00a90a10 = 0x10;
        DAT_00a90a0c = 0x10;
      }
      else if (bVar16 == 0x74) {
        DAT_00a909b8 = (int)(short)param_1[2];
        DAT_00a909c8 = (int)*(short *)((int)param_1 + 10);
        DAT_00a909d8 = (uint)*(byte *)(param_1 + 3);
        DAT_00a909e8 = (uint)*(byte *)((int)param_1 + 0xd);
        DAT_00a90a10 = 8;
        DAT_00a90a0c = 8;
      }
      else {
        if (bVar16 != 100) goto LAB_0040aca2;
        DAT_00a909b8 = (int)(short)param_1[2];
        DAT_00a909c8 = (int)*(short *)((int)param_1 + 10);
        DAT_00a909d8 = (uint)*(byte *)(param_1 + 3);
        DAT_00a909e8 = (uint)*(byte *)((int)param_1 + 0xd);
        DAT_00a90a0c = (undefined2)param_1[4];
        DAT_00a90a10 = *(undefined2 *)((int)param_1 + 0x12);
      }
      if ((*(byte *)((int)param_1 + 7) & 2) == 0) {
        FUN_0040cab0();
      }
      else {
        FUN_0040c8f0();
      }
      goto LAB_0040aca2;
    }
  } while (DAT_00ba01fc != '\0');
  bVar16 = bVar16 & 0xfd;
  if (bVar16 == 0x2c) {
    sVar12 = (short)param_1[2];
    sVar8 = *(short *)((int)param_1 + 10);
    sVar13 = (short)param_1[4];
    sVar9 = *(short *)((int)param_1 + 0x12);
    sVar15 = (short)param_1[6];
    sVar10 = *(short *)((int)param_1 + 0x1a);
    sVar14 = (short)param_1[8];
    sVar11 = *(short *)((int)param_1 + 0x22);
    bVar1 = *(byte *)((int)param_1 + 0xd);
    bVar2 = *(byte *)(param_1 + 3);
    bVar3 = *(byte *)((int)param_1 + 0x15);
    bVar4 = *(byte *)(param_1 + 5);
    bVar16 = *(byte *)((int)param_1 + 0x1d);
    bVar17 = *(byte *)(param_1 + 7);
    bVar5 = *(byte *)((int)param_1 + 0x25);
    bVar6 = *(byte *)(param_1 + 9);
    DAT_00a8e6fc = (uint)((*(ushort *)((int)param_1 + 0x16) & 0x180) == 0x180);
    DAT_00a8e6f4 = *(byte *)((int)param_1 + 0x16) & 0x1f;
    DAT_00a909fc = &DAT_0096e6e0 + (DAT_00a8e6f4 >> 1) * 0x10000;
    if (DAT_00a8e6fc != 0) {
      DAT_00a909fc = &DAT_00a6e6e0;
    }
    bVar7 = *(byte *)((int)param_1 + 0x16);
LAB_0040aef6:
    DAT_00a909f4 = (uint)bVar16 << 0x10;
    DAT_00a909e4 = (uint)bVar17 << 0x10;
    DAT_00a909e0 = (uint)bVar6 << 0x10;
    DAT_00a909d4 = (int)sVar10;
    DAT_00a909d0 = (int)sVar11;
    DAT_00a909c4 = (int)sVar15;
    DAT_00a909c0 = (int)sVar14;
    DAT_00a8e6f8 = bVar7 >> 5 & 3;
    bVar16 = *(byte *)(param_1 + 1);
    DAT_00a909f8 = 4;
  }
  else {
    if (bVar16 == 0x3c) {
      sVar12 = (short)param_1[2];
      sVar8 = *(short *)((int)param_1 + 10);
      sVar13 = (short)param_1[5];
      sVar9 = *(short *)((int)param_1 + 0x16);
      sVar15 = (short)param_1[8];
      sVar10 = *(short *)((int)param_1 + 0x22);
      sVar14 = (short)param_1[0xb];
      sVar11 = *(short *)((int)param_1 + 0x2e);
      bVar1 = *(byte *)((int)param_1 + 0xd);
      bVar2 = *(byte *)(param_1 + 3);
      bVar3 = *(byte *)((int)param_1 + 0x19);
      bVar4 = *(byte *)(param_1 + 6);
      bVar16 = *(byte *)((int)param_1 + 0x25);
      bVar17 = *(byte *)(param_1 + 9);
      bVar5 = *(byte *)((int)param_1 + 0x31);
      bVar6 = *(byte *)(param_1 + 0xc);
      DAT_00a8e6fc = (uint)((*(ushort *)((int)param_1 + 0x1a) & 0x180) == 0x180);
      DAT_00a8e6f4 = *(byte *)((int)param_1 + 0x1a) & 0x1f;
      DAT_00a909fc = &DAT_0096e6e0 + (DAT_00a8e6f4 >> 1) * 0x10000;
      if (DAT_00a8e6fc != 0) {
        DAT_00a909fc = &DAT_00a6e6e0;
      }
      bVar7 = *(byte *)((int)param_1 + 0x1a);
      goto LAB_0040aef6;
    }
    if (bVar16 != 0x24) goto LAB_0040aca2;
    sVar12 = (short)param_1[2];
    sVar8 = *(short *)((int)param_1 + 10);
    sVar13 = (short)param_1[4];
    sVar9 = *(short *)((int)param_1 + 0x12);
    DAT_00a909c0 = (int)(short)param_1[6];
    DAT_00a909d0 = (int)*(short *)((int)param_1 + 0x1a);
    bVar1 = *(byte *)((int)param_1 + 0xd);
    bVar2 = *(byte *)(param_1 + 3);
    bVar3 = *(byte *)((int)param_1 + 0x15);
    bVar4 = *(byte *)(param_1 + 5);
    bVar5 = *(byte *)((int)param_1 + 0x1d);
    DAT_00a909e0 = (uint)*(byte *)(param_1 + 7) << 0x10;
    DAT_00a8e6fc = (uint)((*(ushort *)((int)param_1 + 0x16) & 0x180) == 0x180);
    DAT_00a8e6f4 = *(byte *)((int)param_1 + 0x16) & 0x1f;
    DAT_00a909fc = &DAT_0096e6e0 + (DAT_00a8e6f4 >> 1) * 0x10000;
    if (DAT_00a8e6fc != 0) {
      DAT_00a909fc = &DAT_00a6e6e0;
    }
    DAT_00a8e6f8 = (*(byte *)((int)param_1 + 0x16) & 0x60) >> 5;
    bVar16 = *(byte *)(param_1 + 1);
    DAT_00a909f8 = 3;
    DAT_00a909c4 = DAT_00a909c0;
    DAT_00a909d4 = DAT_00a909d0;
    DAT_00a909f4 = DAT_00a909e0;
  }
  DAT_00a90a00 = (uint)bVar16;
  DAT_00a909f0 = (uint)bVar5 << 0x10;
  DAT_00a909ec = (uint)bVar3 << 0x10;
  DAT_00a909e8 = (uint)bVar1 << 0x10;
  DAT_00a909dc = (uint)bVar4 << 0x10;
  DAT_00a909d8 = (uint)bVar2 << 0x10;
  DAT_00a909cc = (int)sVar9;
  DAT_00a909c8 = (int)sVar8;
  DAT_00a909bc = (int)sVar13;
  DAT_00a909b8 = (int)sVar12;
  DAT_00a90a04 = (uint)*(ushort *)((int)param_1 + 0xe);
  if ((*(byte *)((int)param_1 + 7) & 2) == 0) {
    FUN_0040d420();
  }
  else {
    FUN_0040e680();
  }
  goto LAB_0040aca2;
}

