
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0040dd70(void)

{
  byte bVar1;
  uint uVar2;
  longlong lVar3;
  undefined1 *puVar4;
  int iVar5;
  short sVar6;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  undefined1 *puVar17;
  bool bVar18;
  int local_8;
  short sVar7;
  
  if ((DAT_00ba01f0 != '\0') && (g_framePtr != 0)) {
    if (((DAT_00a909f8 == 4) &&
        (((((DAT_00a909b8 <= DAT_00a909c0 && (DAT_00a909c8 <= DAT_00a909d0)) &&
           (DAT_00a909c8 == DAT_00a909cc)) &&
          ((DAT_00a909d0 == DAT_00a909d4 && (DAT_00a909b8 == DAT_00a909c4)))) &&
         (DAT_00a909bc == DAT_00a909c0)))) &&
       ((((DAT_00a909d8 <= DAT_00a909e0 && (DAT_00a909e8 <= DAT_00a909f0)) &&
         (DAT_00a909e8 == DAT_00a909ec)) &&
        (((DAT_00a909f0 == DAT_00a909f4 && (DAT_00a909d8 == DAT_00a909e4)) &&
         (DAT_00a909dc == DAT_00a909e0)))))) {
      DAT_00a90a0c = (short)DAT_00a909bc - (short)DAT_00a909b8;
      DAT_00a90a10 = (short)DAT_00a909d4 - (short)DAT_00a909c8;
      if ((DAT_00a909e0 - DAT_00a909d8 >> 0x10 == (int)DAT_00a90a0c) &&
         (DAT_00a909f0 - DAT_00a909e8 >> 0x10 == (int)DAT_00a90a10)) {
        DAT_00a909d8 = DAT_00a909d8 >> 0x10;
        DAT_00a909e8 = DAT_00a909e8 >> 0x10;
        FUN_0040c700();
        return;
      }
    }
    DAT_00a90948 = &DAT_0092e3d0 + (DAT_00a8e6f8 & 3) * 0x10000;
    DAT_00a9092c = &DAT_0091c488 + (DAT_00a90a00 & 0xff) * 0x100;
    iVar11 = 1;
    DAT_00a90968 = DAT_00a909c8;
    DAT_00a909a8 = DAT_00a909c8;
    DAT_00a90960 = DAT_00a909b8;
    DAT_00a90950 = 0;
    DAT_00a9096c = 1;
    iVar12 = DAT_00a909b8;
    iVar15 = DAT_00a909c8;
    if (1 < DAT_00a909f8) {
      local_8 = 0;
      iVar8 = 0;
      iVar14 = DAT_00a909c8;
      do {
        iVar16 = *(int *)((int)&DAT_00a909cc + local_8);
        DAT_00a90950 = iVar11;
        iVar15 = iVar16;
        iVar5 = iVar16;
        if ((iVar14 <= iVar16) &&
           (DAT_00a90950 = iVar8, iVar15 = iVar14, iVar5 = DAT_00a90968, DAT_00a909a8 < iVar16)) {
          DAT_00a909a8 = iVar16;
        }
        DAT_00a90968 = iVar5;
        iVar8 = *(int *)((int)&DAT_00a909bc + local_8);
        iVar14 = iVar8;
        if ((DAT_00a90960 <= iVar8) && (iVar14 = DAT_00a90960, iVar12 < iVar8)) {
          iVar12 = iVar8;
        }
        DAT_00a90960 = iVar14;
        iVar11 = iVar11 + 1;
        local_8 = local_8 + 4;
        iVar8 = DAT_00a90950;
        iVar14 = iVar15;
        DAT_00a9096c = iVar11;
      } while (iVar11 < DAT_00a909f8);
    }
    DAT_00a90988 = iVar12;
    if ((((DAT_00a909a8 - iVar15 < 0x800) && (iVar12 - DAT_00a90960 < 0x800)) && (-1 < iVar12)) &&
       (((DAT_00a90960 < 0x140 && (iVar15 < DAT_0043a48c)) && (DAT_00a909b4 <= DAT_00a909a8)))) {
      if (iVar15 < DAT_00a909b4) {
        DAT_00a90968 = DAT_00a909b4;
        iVar15 = DAT_00a909b4;
      }
      if (DAT_0043a48c < DAT_00a909a8) {
        DAT_00a909a8 = DAT_0043a48c;
      }
      DAT_00a90944 = iVar15 * g_framePitch + g_framePtr;
      DAT_00a909f8 = DAT_00a909f8 + -1;
      DAT_00a9098c = 0;
      DAT_00a90964 = 0;
      iVar11 = DAT_00a90950;
      iVar8 = DAT_00a90950;
      DAT_00a90970 = DAT_00a90950;
      if (iVar15 < DAT_00a909a8) {
        do {
          DAT_00a9096c = iVar11;
          iVar14 = iVar8;
          iVar16 = iVar15;
          if (DAT_00a90964 <= iVar15) {
            do {
              iVar8 = iVar14 + -1;
              if (iVar14 + -1 < 0) {
                iVar8 = DAT_00a909f8;
              }
              if ((&DAT_00a909c8)[iVar8] - (&DAT_00a909c8)[iVar14] != 0) {
                iVar15 = *(int *)(&DAT_00a8e918 +
                                 ((&DAT_00a909c8)[iVar8] - (&DAT_00a909c8)[iVar14] & 0x7ffU) * 4);
                lVar3 = (longlong)(((&DAT_00a909b8)[iVar8] - (&DAT_00a909b8)[iVar14]) * 0x10000) *
                        (longlong)iVar15;
                DAT_00a90924 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)((&DAT_00a909d8)[iVar8] - (&DAT_00a909d8)[iVar14]) *
                        (longlong)iVar15;
                DAT_00a9091c = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)((&DAT_00a909e8)[iVar8] - (&DAT_00a909e8)[iVar14]) *
                        (longlong)iVar15;
                DAT_00a90920 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                iVar11 = DAT_00a90970;
                iVar12 = DAT_00a90988;
                iVar14 = DAT_00a90950;
              }
              DAT_00a90958 = (&DAT_00a909d8)[iVar14];
              DAT_00a9095c = (&DAT_00a909e8)[iVar14];
              DAT_00a90960 = (&DAT_00a909b8)[iVar14] * 0x10000;
              DAT_00a9097c = DAT_00a90968 - (&DAT_00a909c8)[iVar14];
              if (DAT_00a9097c != 0) {
                DAT_00a90960 = DAT_00a90960 + DAT_00a9097c * DAT_00a90924;
                DAT_00a90958 = DAT_00a90958 + DAT_00a9097c * DAT_00a9091c;
                DAT_00a9095c = DAT_00a9095c + DAT_00a9097c * DAT_00a90920;
              }
              DAT_00a90964 = (&DAT_00a909c8)[iVar8];
              DAT_00a9096c = iVar11;
              iVar14 = iVar8;
              iVar16 = DAT_00a90968;
              DAT_00a90950 = iVar8;
            } while (DAT_00a90964 <= DAT_00a90968);
          }
          iVar15 = DAT_00a9096c;
          if (DAT_00a9098c <= iVar16) {
            do {
              DAT_00a9096c = iVar15 + 1;
              if (DAT_00a909f8 < DAT_00a9096c) {
                DAT_00a9096c = 0;
              }
              if ((&DAT_00a909c8)[DAT_00a9096c] - (&DAT_00a909c8)[iVar15] != 0) {
                iVar12 = *(int *)(&DAT_00a8e918 +
                                 ((&DAT_00a909c8)[DAT_00a9096c] - (&DAT_00a909c8)[iVar15] & 0x7ffU)
                                 * 4);
                lVar3 = (longlong)
                        (((&DAT_00a909b8)[DAT_00a9096c] - (&DAT_00a909b8)[iVar15]) * 0x10000) *
                        (longlong)iVar12;
                DAT_00a90934 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)((&DAT_00a909d8)[DAT_00a9096c] - (&DAT_00a909d8)[iVar15]) *
                        (longlong)iVar12;
                DAT_00a90928 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)((&DAT_00a909e8)[DAT_00a9096c] - (&DAT_00a909e8)[iVar15]) *
                        (longlong)iVar12;
                DAT_00a90930 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                iVar15 = DAT_00a90970;
                iVar8 = DAT_00a90950;
                iVar16 = DAT_00a90968;
              }
              DAT_00a90980 = (&DAT_00a909d8)[iVar15];
              DAT_00a90984 = (&DAT_00a909e8)[iVar15];
              iVar12 = (&DAT_00a909b8)[iVar15] * 0x10000;
              DAT_00a9097c = iVar16 - (&DAT_00a909c8)[iVar15];
              if (DAT_00a9097c != 0) {
                iVar12 = iVar12 + DAT_00a9097c * DAT_00a90934;
                DAT_00a90980 = DAT_00a90980 + DAT_00a9097c * DAT_00a90928;
                DAT_00a90984 = DAT_00a90984 + DAT_00a9097c * DAT_00a90930;
                iVar16 = DAT_00a90968;
              }
              DAT_00a9098c = (&DAT_00a909c8)[DAT_00a9096c];
              iVar15 = DAT_00a9096c;
              DAT_00a90970 = DAT_00a9096c;
              DAT_00a90988 = iVar12;
            } while (DAT_00a9098c <= iVar16);
          }
          iVar15 = DAT_00a90964;
          if (DAT_00a9098c <= DAT_00a90964) {
            iVar15 = DAT_00a9098c;
          }
          DAT_00a909a4 = iVar15 - iVar16;
          iVar16 = iVar15;
          iVar11 = DAT_00a9096c;
          puVar4 = DAT_00a90948;
          iVar14 = DAT_00a90988;
          if (DAT_00a909a8 < iVar15) {
            DAT_00a909a4 = DAT_00a909a4 + (DAT_00a909a8 - iVar15);
          }
          for (; DAT_00a9096c = iVar16, DAT_00a90988 = iVar14, DAT_00a90948 = puVar4,
              DAT_00a90968 = iVar15, 0 < DAT_00a909a4; DAT_00a909a4 = DAT_00a909a4 + -1) {
            DAT_00a9096c = iVar12 >> 0x10;
            if (DAT_00a90960 < iVar12) {
              DAT_00a90938 = DAT_00a90980 - DAT_00a90958;
              DAT_00a9093c = DAT_00a90984 - DAT_00a9095c;
              DAT_00a9097c = DAT_00a9096c;
              DAT_00a9096c = DAT_00a90960 >> 0x10;
              DAT_00a90994 = DAT_00a9095c;
              DAT_00a90990 = DAT_00a90958;
            }
            else {
              DAT_00a90938 = DAT_00a90958 - DAT_00a90980;
              DAT_00a9093c = DAT_00a9095c - DAT_00a90984;
              DAT_00a9097c = DAT_00a90960 >> 0x10;
              DAT_00a90994 = DAT_00a90984;
              DAT_00a90990 = DAT_00a90980;
            }
            DAT_00a90974 = DAT_00a9097c - DAT_00a9096c;
            DAT_00a90988 = iVar12;
            if (((DAT_00a90974 != 0) && (-1 < DAT_00a9097c)) && (DAT_00a9096c < 0x140)) {
              DAT_00a9097c = *(int *)(&DAT_00a8e918 + (DAT_00a90974 & 0x7ff) * 4);
              DAT_00a90938 = (uint)((longlong)(int)DAT_00a90938 * (longlong)DAT_00a9097c) >> 0x1e |
                             (int)((ulonglong)((longlong)(int)DAT_00a90938 * (longlong)DAT_00a9097c)
                                  >> 0x20) << 2;
              uVar13 = (int)((ulonglong)((longlong)(int)DAT_00a9093c * (longlong)DAT_00a9097c) >>
                            0x20) << 2;
              DAT_00a9093c = (uint)((longlong)(int)DAT_00a9093c * (longlong)DAT_00a9097c) >> 0x1e |
                             uVar13;
              DAT_00a90974 = DAT_00a90974 + DAT_00a9096c;
              if (DAT_00a9096c < 0) {
                DAT_00a90990 = DAT_00a90990 - DAT_00a9096c * DAT_00a90938;
                DAT_00a90994 = DAT_00a90994 - DAT_00a9096c * DAT_00a9093c;
                DAT_00a9096c = 0;
              }
              puVar17 = (undefined1 *)(DAT_00a90944 + DAT_00a9096c);
              if (0x140 < (int)DAT_00a90974) {
                DAT_00a90974 = 0x140;
              }
              DAT_00a9096c = DAT_00a90974 - DAT_00a9096c;
              iVar11 = DAT_00a90970;
              DAT_00a90988 = iVar14;
              iVar8 = DAT_00a90950;
              DAT_00a90940 = puVar17;
              if (0 < DAT_00a9096c) {
                _DAT_00a90998 = DAT_00a9093c << 0x10;
                _DAT_00a9099c = (int)DAT_00a90938 >> 0x10;
                uVar9 = CONCAT22((short)DAT_00a90994,(short)DAT_00a9096c);
                uVar10 = CONCAT31((int3)((DAT_00a90938 << 0x10) >> 8),(char)(uVar13 >> 0x10));
                iVar15 = DAT_00a909fc + (DAT_00a90990 >> 0x10);
                uVar13 = CONCAT31((int3)((DAT_00a90990 << 0x10) >> 8),
                                  (char)((uint)DAT_00a90994 >> 0x10));
                _DAT_00a909a0 = DAT_00a9092c;
                do {
                  bVar18 = CARRY4(uVar9,_DAT_00a90998);
                  iVar12 = uVar9 + _DAT_00a90998;
                  uVar2 = uVar13 + uVar10;
                  bVar1 = *(byte *)(iVar15 + (uVar13 & 0xff) * 0x100);
                  iVar15 = iVar15 + _DAT_00a9099c +
                           (uint)(CARRY4(uVar13,uVar10) || CARRY4(uVar2,(uint)bVar18));
                  if (bVar1 != 0) {
                    *puVar17 = puVar4[CONCAT11(_DAT_00a909a0[bVar1],*puVar17)];
                  }
                  puVar17 = puVar17 + 1;
                  sVar7 = (short)iVar12;
                  sVar6 = sVar7 + -1;
                  uVar9 = CONCAT22((short)((uint)iVar12 >> 0x10),sVar6);
                  iVar11 = DAT_00a90970;
                  uVar13 = uVar2 + bVar18;
                  iVar8 = DAT_00a90950;
                } while (sVar6 != 0 && 0 < sVar7);
              }
            }
            DAT_00a90944 = DAT_00a90944 + g_framePitch;
            DAT_00a90960 = DAT_00a90960 + DAT_00a90924;
            iVar12 = DAT_00a90988 + DAT_00a90934;
            DAT_00a90958 = DAT_00a90958 + DAT_00a9091c;
            DAT_00a9095c = DAT_00a9095c + DAT_00a90920;
            DAT_00a90980 = DAT_00a90980 + DAT_00a90928;
            DAT_00a90984 = DAT_00a90984 + DAT_00a90930;
            iVar15 = DAT_00a90968;
            iVar16 = DAT_00a9096c;
            puVar4 = DAT_00a90948;
            iVar14 = iVar12;
          }
        } while (iVar15 < DAT_00a909a8);
      }
      DAT_0043a590 = 1;
    }
  }
  return;
}


