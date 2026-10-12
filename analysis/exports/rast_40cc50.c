
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0040cc50(void)

{
  byte bVar1;
  uint uVar2;
  longlong lVar3;
  undefined *puVar4;
  int iVar5;
  short sVar6;
  int iVar8;
  uint uVar9;
  int iVar10;
  char cVar12;
  uint uVar11;
  int iVar13;
  undefined1 uVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
  int iVar18;
  undefined1 *puVar19;
  bool bVar20;
  int local_8;
  short sVar7;
  
  if ((DAT_00ba01f0 != '\0') && (g_framePtr != 0)) {
    iVar13 = 1;
    DAT_00a90968 = DAT_00a909c8;
    DAT_00a90954 = &DAT_0092c390 + DAT_00a90a04 * 0x100;
    DAT_00a909a8 = DAT_00a909c8;
    DAT_00a90960 = DAT_00a909b8;
    DAT_00a90950 = 0;
    DAT_00a9096c = 1;
    iVar15 = DAT_00a909b8;
    iVar10 = DAT_00a909c8;
    if (1 < DAT_00a909f8) {
      local_8 = 0;
      iVar8 = 0;
      iVar17 = DAT_00a909c8;
      do {
        iVar18 = *(int *)((int)&DAT_00a909cc + local_8);
        DAT_00a90950 = iVar13;
        iVar10 = iVar18;
        iVar5 = iVar18;
        if ((iVar17 <= iVar18) &&
           (DAT_00a90950 = iVar8, iVar10 = iVar17, iVar5 = DAT_00a90968, DAT_00a909a8 < iVar18)) {
          DAT_00a909a8 = iVar18;
        }
        DAT_00a90968 = iVar5;
        iVar8 = *(int *)((int)&DAT_00a909bc + local_8);
        iVar17 = iVar8;
        if ((DAT_00a90960 <= iVar8) && (iVar17 = DAT_00a90960, iVar15 < iVar8)) {
          iVar15 = iVar8;
        }
        DAT_00a90960 = iVar17;
        iVar13 = iVar13 + 1;
        local_8 = local_8 + 4;
        iVar8 = DAT_00a90950;
        iVar17 = iVar10;
        DAT_00a9096c = iVar13;
      } while (iVar13 < DAT_00a909f8);
    }
    DAT_00a90988 = iVar15;
    if (((((DAT_00a909a8 - iVar10 < 0x800) && (iVar15 - DAT_00a90960 < 0x800)) && (-1 < iVar15)) &&
        ((DAT_00a90960 < 0x140 && (iVar10 < DAT_0043a48c)))) && (DAT_00a909b4 <= DAT_00a909a8)) {
      if (iVar10 < DAT_00a909b4) {
        DAT_00a90968 = DAT_00a909b4;
        iVar10 = DAT_00a909b4;
      }
      if (DAT_0043a48c < DAT_00a909a8) {
        DAT_00a909a8 = DAT_0043a48c;
      }
      DAT_00a90944 = iVar10 * g_framePitch + g_framePtr;
      DAT_00a909f8 = DAT_00a909f8 + -1;
      DAT_00a9098c = 0;
      DAT_00a90964 = 0;
      iVar13 = DAT_00a90950;
      iVar8 = DAT_00a90950;
      DAT_00a90970 = DAT_00a90950;
      if (iVar10 < DAT_00a909a8) {
        do {
          DAT_00a9096c = iVar13;
          iVar17 = iVar8;
          iVar18 = iVar10;
          if (DAT_00a90964 <= iVar10) {
            do {
              iVar8 = iVar17 + -1;
              if (iVar17 + -1 < 0) {
                iVar8 = DAT_00a909f8;
              }
              if ((&DAT_00a909c8)[iVar8] - (&DAT_00a909c8)[iVar17] != 0) {
                iVar10 = *(int *)(&DAT_00a8e918 +
                                 ((&DAT_00a909c8)[iVar8] - (&DAT_00a909c8)[iVar17] & 0x7ffU) * 4);
                lVar3 = (longlong)(((&DAT_00a909b8)[iVar8] - (&DAT_00a909b8)[iVar17]) * 0x10000) *
                        (longlong)iVar10;
                DAT_00a90924 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)(int)((&DAT_00a909d8)[iVar8] - (&DAT_00a909d8)[iVar17]) *
                        (longlong)iVar10;
                DAT_00a9091c = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)(int)((&DAT_00a909e8)[iVar8] - (&DAT_00a909e8)[iVar17]) *
                        (longlong)iVar10;
                DAT_00a90920 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                iVar13 = DAT_00a90970;
                iVar15 = DAT_00a90988;
                iVar17 = DAT_00a90950;
              }
              DAT_00a90958 = (&DAT_00a909d8)[iVar17];
              DAT_00a9095c = (&DAT_00a909e8)[iVar17];
              DAT_00a90960 = (&DAT_00a909b8)[iVar17] * 0x10000;
              DAT_00a9097c = DAT_00a90968 - (&DAT_00a909c8)[iVar17];
              if (DAT_00a9097c != 0) {
                DAT_00a90960 = DAT_00a90960 + DAT_00a9097c * DAT_00a90924;
                DAT_00a90958 = DAT_00a90958 + DAT_00a9097c * DAT_00a9091c;
                DAT_00a9095c = DAT_00a9095c + DAT_00a9097c * DAT_00a90920;
              }
              DAT_00a90964 = (&DAT_00a909c8)[iVar8];
              DAT_00a9096c = iVar13;
              iVar17 = iVar8;
              iVar18 = DAT_00a90968;
              DAT_00a90950 = iVar8;
            } while (DAT_00a90964 <= DAT_00a90968);
          }
          iVar10 = DAT_00a9096c;
          if (DAT_00a9098c <= iVar18) {
            do {
              DAT_00a9096c = iVar10 + 1;
              if (DAT_00a909f8 < DAT_00a9096c) {
                DAT_00a9096c = 0;
              }
              if ((&DAT_00a909c8)[DAT_00a9096c] - (&DAT_00a909c8)[iVar10] != 0) {
                iVar15 = *(int *)(&DAT_00a8e918 +
                                 ((&DAT_00a909c8)[DAT_00a9096c] - (&DAT_00a909c8)[iVar10] & 0x7ffU)
                                 * 4);
                lVar3 = (longlong)
                        (((&DAT_00a909b8)[DAT_00a9096c] - (&DAT_00a909b8)[iVar10]) * 0x10000) *
                        (longlong)iVar15;
                DAT_00a90934 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)(int)((&DAT_00a909d8)[DAT_00a9096c] - (&DAT_00a909d8)[iVar10]) *
                        (longlong)iVar15;
                DAT_00a90928 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                lVar3 = (longlong)(int)((&DAT_00a909e8)[DAT_00a9096c] - (&DAT_00a909e8)[iVar10]) *
                        (longlong)iVar15;
                DAT_00a90930 = (uint)lVar3 >> 0x1e | (int)((ulonglong)lVar3 >> 0x20) << 2;
                iVar10 = DAT_00a90970;
                iVar8 = DAT_00a90950;
                iVar18 = DAT_00a90968;
              }
              DAT_00a90980 = (&DAT_00a909d8)[iVar10];
              DAT_00a90984 = (&DAT_00a909e8)[iVar10];
              iVar15 = (&DAT_00a909b8)[iVar10] * 0x10000;
              DAT_00a9097c = iVar18 - (&DAT_00a909c8)[iVar10];
              if (DAT_00a9097c != 0) {
                iVar15 = iVar15 + DAT_00a9097c * DAT_00a90934;
                DAT_00a90980 = DAT_00a90980 + DAT_00a9097c * DAT_00a90928;
                DAT_00a90984 = DAT_00a90984 + DAT_00a9097c * DAT_00a90930;
                iVar18 = DAT_00a90968;
              }
              DAT_00a9098c = (&DAT_00a909c8)[DAT_00a9096c];
              iVar10 = DAT_00a9096c;
              DAT_00a90970 = DAT_00a9096c;
              DAT_00a90988 = iVar15;
            } while (DAT_00a9098c <= iVar18);
          }
          iVar10 = DAT_00a90964;
          if (DAT_00a9098c <= DAT_00a90964) {
            iVar10 = DAT_00a9098c;
          }
          DAT_00a909a4 = iVar10 - iVar18;
          iVar5 = iVar10;
          iVar13 = DAT_00a9096c;
          puVar4 = DAT_00a90954;
          iVar17 = DAT_00a90988;
          iVar18 = DAT_00a909fc;
          if (DAT_00a909a8 < iVar10) {
            DAT_00a909a4 = DAT_00a909a4 + (DAT_00a909a8 - iVar10);
          }
          for (; DAT_00a9096c = iVar5, DAT_00a90988 = iVar17, DAT_00a90954 = puVar4,
              DAT_00a90968 = iVar10, DAT_00a909fc = iVar18, 0 < DAT_00a909a4;
              DAT_00a909a4 = DAT_00a909a4 + -1) {
            DAT_00a9096c = iVar15 >> 0x10;
            if (DAT_00a90960 < iVar15) {
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
            DAT_00a90988 = iVar15;
            if (((DAT_00a90974 != 0) && (-1 < DAT_00a9097c)) && (DAT_00a9096c < 0x140)) {
              DAT_00a9097c = *(int *)(&DAT_00a8e918 + (DAT_00a90974 & 0x7ff) * 4);
              uVar11 = (int)((ulonglong)((longlong)(int)DAT_00a90938 * (longlong)DAT_00a9097c) >>
                            0x20) << 2;
              DAT_00a90938 = (uint)((longlong)(int)DAT_00a90938 * (longlong)DAT_00a9097c) >> 0x1e |
                             uVar11;
              uVar9 = (int)((ulonglong)((longlong)(int)DAT_00a9093c * (longlong)DAT_00a9097c) >>
                           0x20) << 2;
              DAT_00a9093c = (uint)((longlong)(int)DAT_00a9093c * (longlong)DAT_00a9097c) >> 0x1e |
                             uVar9;
              DAT_00a90974 = DAT_00a90974 + DAT_00a9096c;
              if (DAT_00a9096c < 0) {
                DAT_00a90990 = DAT_00a90990 - DAT_00a9096c * DAT_00a90938;
                DAT_00a90994 = DAT_00a90994 - DAT_00a9096c * DAT_00a9093c;
                DAT_00a9096c = 0;
              }
              puVar19 = (undefined1 *)(DAT_00a90944 + DAT_00a9096c);
              if (0x140 < (int)DAT_00a90974) {
                DAT_00a90974 = 0x140;
              }
              DAT_00a9096c = DAT_00a90974 - DAT_00a9096c;
              iVar13 = DAT_00a90970;
              DAT_00a90988 = iVar17;
              iVar8 = DAT_00a90950;
              DAT_00a90940 = puVar19;
              if (0 < DAT_00a9096c) {
                _DAT_00a90998 = DAT_00a90938 << 0x10;
                DAT_00a90978 = (char)(uVar9 >> 0x10);
                uVar9 = CONCAT22((short)DAT_00a90990,(short)DAT_00a9096c);
                uVar16 = CONCAT31((int3)((uint)(DAT_00a90994 << 0x10) >> 8),
                                  (char)((uint)DAT_00a90990 >> 0x10));
                uVar11 = CONCAT22((short)DAT_00a9093c,
                                  CONCAT11((char)((uint)DAT_00a90994 >> 0x10),(char)(uVar11 >> 0x10)
                                          ));
                do {
                  uVar14 = (undefined1)uVar16;
                  iVar10 = uVar9 + _DAT_00a90998;
                  bVar20 = CARRY4(uVar16,uVar11);
                  uVar2 = uVar16 + uVar11;
                  uVar16 = uVar2 + CARRY4(uVar9,_DAT_00a90998);
                  cVar12 = (char)(uVar11 >> 8);
                  bVar1 = *(byte *)(iVar18 + (uint)CONCAT11(cVar12,uVar14));
                  uVar11 = CONCAT22((short)(uVar11 >> 0x10),
                                    CONCAT11(cVar12 + DAT_00a90978 +
                                             (bVar20 ||
                                             CARRY4(uVar2,(uint)CARRY4(uVar9,_DAT_00a90998))),
                                             (char)uVar11));
                  if (bVar1 != 0) {
                    *puVar19 = puVar4[bVar1];
                  }
                  puVar19 = puVar19 + 1;
                  sVar7 = (short)iVar10;
                  sVar6 = sVar7 + -1;
                  uVar9 = CONCAT22((short)((uint)iVar10 >> 0x10),sVar6);
                  iVar13 = DAT_00a90970;
                  iVar8 = DAT_00a90950;
                } while (sVar6 != 0 && 0 < sVar7);
              }
            }
            DAT_00a90944 = DAT_00a90944 + g_framePitch;
            DAT_00a90960 = DAT_00a90960 + DAT_00a90924;
            iVar15 = DAT_00a90988 + DAT_00a90934;
            DAT_00a90958 = DAT_00a90958 + DAT_00a9091c;
            DAT_00a9095c = DAT_00a9095c + DAT_00a90920;
            DAT_00a90980 = DAT_00a90980 + DAT_00a90928;
            DAT_00a90984 = DAT_00a90984 + DAT_00a90930;
            iVar10 = DAT_00a90968;
            iVar5 = DAT_00a9096c;
            puVar4 = DAT_00a90954;
            iVar17 = iVar15;
            iVar18 = DAT_00a909fc;
          }
        } while (iVar10 < DAT_00a909a8);
      }
      DAT_0043a590 = 1;
    }
  }
  return;
}


