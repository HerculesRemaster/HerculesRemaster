
/* WARNING: Removing unreachable block (ram,0x0040d5e4) */

void FUN_0040d420(void)

{
  char cVar1;
  longlong lVar2;
  int iVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  ushort uVar8;
  char cVar9;
  int iVar10;
  int iVar12;
  uint uVar13;
  short sVar14;
  uint uVar16;
  int iVar17;
  int iVar18;
  char *pcVar19;
  bool bVar20;
  int local_8;
  uint uVar11;
  short sVar15;
  
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
        FUN_0040cab0();
        return;
      }
    }
    DAT_00a90a04 = ((int)DAT_00a90a04 >> 6) - 0x1e0;
    if (0x1e < DAT_00a90a04) {
      DAT_00a90a04 = 0;
    }
    if (DAT_0096e3e0 == 0) {
      if (DAT_00a90a04 != 0) {
        DAT_00a90a08 = CONCAT31(DAT_00a90a08._1_3_,
                                *(undefined1 *)
                                 ((DAT_00a909d8 + DAT_00a909dc + DAT_00a909e0 + DAT_00a909e4 >> 0x12
                                  ) + DAT_00a909fc +
                                      (uint)(uint3)(DAT_00a909e8 + DAT_00a909ec + DAT_00a909f0 +
                                                    DAT_00a909f4 >> 0x12) * 0x100));
        FUN_0040c2e0();
        return;
      }
    }
    else if (DAT_00a90a04 != 0) {
      FUN_0040cc50();
      return;
    }
    iVar10 = 1;
    DAT_00a90968 = DAT_00a909c8;
    DAT_00a909a8 = DAT_00a909c8;
    DAT_00a90960 = DAT_00a909b8;
    DAT_00a90950 = 0;
    DAT_00a9096c = 1;
    iVar12 = DAT_00a909b8;
    iVar6 = DAT_00a909c8;
    if (1 < DAT_00a909f8) {
      local_8 = 0;
      iVar5 = 0;
      iVar17 = DAT_00a909c8;
      do {
        iVar18 = *(int *)((int)&DAT_00a909cc + local_8);
        DAT_00a90950 = iVar10;
        iVar6 = iVar18;
        iVar3 = iVar18;
        if ((iVar17 <= iVar18) &&
           (DAT_00a90950 = iVar5, iVar6 = iVar17, iVar3 = DAT_00a90968, DAT_00a909a8 < iVar18)) {
          DAT_00a909a8 = iVar18;
        }
        DAT_00a90968 = iVar3;
        iVar5 = *(int *)((int)&DAT_00a909bc + local_8);
        iVar17 = iVar5;
        if ((DAT_00a90960 <= iVar5) && (iVar17 = DAT_00a90960, iVar12 < iVar5)) {
          iVar12 = iVar5;
        }
        DAT_00a90960 = iVar17;
        iVar10 = iVar10 + 1;
        local_8 = local_8 + 4;
        iVar5 = DAT_00a90950;
        iVar17 = iVar6;
        DAT_00a9096c = iVar10;
      } while (iVar10 < DAT_00a909f8);
    }
    DAT_00a90988 = iVar12;
    if ((((DAT_00a909a8 - iVar6 < 0x800) && (iVar12 - DAT_00a90960 < 0x800)) && (-1 < iVar12)) &&
       (((DAT_00a90960 < 0x140 && (iVar6 < DAT_0043a48c)) && (DAT_00a909b4 <= DAT_00a909a8)))) {
      if (iVar6 < DAT_00a909b4) {
        DAT_00a90968 = DAT_00a909b4;
        iVar6 = DAT_00a909b4;
      }
      if (DAT_0043a48c < DAT_00a909a8) {
        DAT_00a909a8 = DAT_0043a48c;
      }
      DAT_00a90944 = iVar6 * g_framePitch + g_framePtr;
      DAT_00a909f8 = DAT_00a909f8 + -1;
      DAT_00a9098c = 0;
      DAT_00a90964 = 0;
      iVar10 = DAT_00a90950;
      iVar5 = DAT_00a90950;
      DAT_00a90970 = DAT_00a90950;
      if (iVar6 < DAT_00a909a8) {
        do {
          DAT_00a9096c = iVar10;
          iVar17 = iVar5;
          iVar18 = iVar6;
          if (DAT_00a90964 <= iVar6) {
            do {
              iVar5 = iVar17 + -1;
              if (iVar17 + -1 < 0) {
                iVar5 = DAT_00a909f8;
              }
              if ((&DAT_00a909c8)[iVar5] - (&DAT_00a909c8)[iVar17] != 0) {
                iVar6 = *(int *)(&DAT_00a8e918 +
                                ((&DAT_00a909c8)[iVar5] - (&DAT_00a909c8)[iVar17] & 0x7ffU) * 4);
                lVar2 = (longlong)(((&DAT_00a909b8)[iVar5] - (&DAT_00a909b8)[iVar17]) * 0x10000) *
                        (longlong)iVar6;
                DAT_00a90924 = (uint)lVar2 >> 0x1e | (int)((ulonglong)lVar2 >> 0x20) << 2;
                lVar2 = (longlong)((&DAT_00a909d8)[iVar5] - (&DAT_00a909d8)[iVar17]) *
                        (longlong)iVar6;
                DAT_00a9091c = (uint)lVar2 >> 0x1e | (int)((ulonglong)lVar2 >> 0x20) << 2;
                lVar2 = (longlong)((&DAT_00a909e8)[iVar5] - (&DAT_00a909e8)[iVar17]) *
                        (longlong)iVar6;
                DAT_00a90920 = (uint)lVar2 >> 0x1e | (int)((ulonglong)lVar2 >> 0x20) << 2;
                iVar10 = DAT_00a90970;
                iVar12 = DAT_00a90988;
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
              DAT_00a90964 = (&DAT_00a909c8)[iVar5];
              DAT_00a9096c = iVar10;
              iVar17 = iVar5;
              iVar18 = DAT_00a90968;
              DAT_00a90950 = iVar5;
            } while (DAT_00a90964 <= DAT_00a90968);
          }
          iVar6 = DAT_00a9096c;
          if (DAT_00a9098c <= iVar18) {
            do {
              DAT_00a9096c = iVar6 + 1;
              if (DAT_00a909f8 < DAT_00a9096c) {
                DAT_00a9096c = 0;
              }
              if ((&DAT_00a909c8)[DAT_00a9096c] - (&DAT_00a909c8)[iVar6] != 0) {
                iVar12 = *(int *)(&DAT_00a8e918 +
                                 ((&DAT_00a909c8)[DAT_00a9096c] - (&DAT_00a909c8)[iVar6] & 0x7ffU) *
                                 4);
                lVar2 = (longlong)
                        (((&DAT_00a909b8)[DAT_00a9096c] - (&DAT_00a909b8)[iVar6]) * 0x10000) *
                        (longlong)iVar12;
                DAT_00a90934 = (uint)lVar2 >> 0x1e | (int)((ulonglong)lVar2 >> 0x20) << 2;
                lVar2 = (longlong)((&DAT_00a909d8)[DAT_00a9096c] - (&DAT_00a909d8)[iVar6]) *
                        (longlong)iVar12;
                DAT_00a90928 = (uint)lVar2 >> 0x1e | (int)((ulonglong)lVar2 >> 0x20) << 2;
                lVar2 = (longlong)((&DAT_00a909e8)[DAT_00a9096c] - (&DAT_00a909e8)[iVar6]) *
                        (longlong)iVar12;
                DAT_00a90930 = (uint)lVar2 >> 0x1e | (int)((ulonglong)lVar2 >> 0x20) << 2;
                iVar6 = DAT_00a90970;
                iVar5 = DAT_00a90950;
                iVar18 = DAT_00a90968;
              }
              DAT_00a90980 = (&DAT_00a909d8)[iVar6];
              DAT_00a90984 = (&DAT_00a909e8)[iVar6];
              iVar12 = (&DAT_00a909b8)[iVar6] * 0x10000;
              DAT_00a9097c = iVar18 - (&DAT_00a909c8)[iVar6];
              if (DAT_00a9097c != 0) {
                iVar12 = iVar12 + DAT_00a9097c * DAT_00a90934;
                DAT_00a90980 = DAT_00a90980 + DAT_00a9097c * DAT_00a90928;
                DAT_00a90984 = DAT_00a90984 + DAT_00a9097c * DAT_00a90930;
                iVar18 = DAT_00a90968;
              }
              DAT_00a9098c = (&DAT_00a909c8)[DAT_00a9096c];
              iVar6 = DAT_00a9096c;
              DAT_00a90970 = DAT_00a9096c;
              DAT_00a90988 = iVar12;
            } while (DAT_00a9098c <= iVar18);
          }
          iVar6 = DAT_00a90964;
          if (DAT_00a9098c <= DAT_00a90964) {
            iVar6 = DAT_00a9098c;
          }
          DAT_00a909a4 = iVar6 - iVar18;
          iVar3 = iVar6;
          iVar10 = DAT_00a9096c;
          iVar17 = DAT_00a90988;
          iVar18 = DAT_00a909fc;
          if (DAT_00a909a8 < iVar6) {
            DAT_00a909a4 = DAT_00a909a4 + (DAT_00a909a8 - iVar6);
          }
          for (; DAT_00a9096c = iVar3, DAT_00a90988 = iVar17, DAT_00a90968 = iVar6,
              DAT_00a909fc = iVar18, 0 < DAT_00a909a4; DAT_00a909a4 = DAT_00a909a4 + -1) {
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
              uVar11 = (int)((ulonglong)((longlong)(int)DAT_00a90938 * (longlong)DAT_00a9097c) >>
                            0x20) << 2;
              DAT_00a90938 = (uint)((longlong)(int)DAT_00a90938 * (longlong)DAT_00a9097c) >> 0x1e |
                             uVar11;
              uVar16 = (int)((ulonglong)((longlong)(int)DAT_00a9093c * (longlong)DAT_00a9097c) >>
                            0x20) << 2;
              DAT_00a9093c = (uint)((longlong)(int)DAT_00a9093c * (longlong)DAT_00a9097c) >> 0x1e |
                             uVar16;
              DAT_00a90974 = DAT_00a90974 + DAT_00a9096c;
              if (DAT_00a9096c < 0) {
                DAT_00a90990 = DAT_00a90990 - DAT_00a9096c * DAT_00a90938;
                DAT_00a90994 = DAT_00a90994 - DAT_00a9096c * DAT_00a9093c;
                DAT_00a9096c = 0;
              }
              DAT_00a90940 = DAT_00a90944 + DAT_00a9096c;
              if (0x140 < (int)DAT_00a90974) {
                DAT_00a90974 = 0x140;
              }
              DAT_00a9096c = DAT_00a90974 - DAT_00a9096c;
              iVar10 = DAT_00a90970;
              DAT_00a90988 = iVar17;
              iVar5 = DAT_00a90950;
              if (0 < DAT_00a9096c) {
                uVar13 = DAT_00a90990 << 0x10;
                uVar8 = CONCAT11((char)((uint)DAT_00a90994 >> 0x10),
                                 (char)((uint)DAT_00a90990 >> 0x10));
                cVar4 = (char)(uVar16 >> 0x10);
                cVar9 = (char)(uVar11 >> 0x10);
                uVar11 = CONCAT22((short)DAT_00a90938,CONCAT11(cVar4,cVar9));
                pcVar19 = (char *)(DAT_00a90940 + -1);
                uVar16 = CONCAT22((short)DAT_00a9093c,(short)DAT_00a9096c);
                iVar6 = DAT_00a90994 << 0x10;
                do {
                  cVar1 = *(char *)(iVar18 + (uint)uVar8);
                  uVar7 = CONCAT31((int3)((uint)iVar6 >> 8),cVar1);
                  bVar20 = CARRY4(uVar13,uVar11);
                  uVar13 = uVar13 + uVar11;
                  pcVar19 = pcVar19 + 1;
                  if (cVar1 != '\0') {
                    *pcVar19 = cVar1;
                  }
                  iVar6 = uVar7 + uVar16;
                  uVar8 = CONCAT11((char)(uVar8 >> 8) + cVar4 + CARRY4(uVar7,uVar16),
                                   (char)uVar8 + cVar9 + bVar20);
                  sVar15 = (short)uVar16;
                  sVar14 = sVar15 + -1;
                  uVar16 = CONCAT22((short)(uVar16 >> 0x10),sVar14);
                  iVar10 = DAT_00a90970;
                  iVar5 = DAT_00a90950;
                } while (sVar14 != 0 && 0 < sVar15);
              }
            }
            DAT_00a90944 = DAT_00a90944 + g_framePitch;
            DAT_00a90960 = DAT_00a90960 + DAT_00a90924;
            iVar12 = DAT_00a90988 + DAT_00a90934;
            DAT_00a90958 = DAT_00a90958 + DAT_00a9091c;
            DAT_00a9095c = DAT_00a9095c + DAT_00a90920;
            DAT_00a90980 = DAT_00a90980 + DAT_00a90928;
            DAT_00a90984 = DAT_00a90984 + DAT_00a90930;
            iVar6 = DAT_00a90968;
            iVar3 = DAT_00a9096c;
            iVar17 = iVar12;
            iVar18 = DAT_00a909fc;
          }
        } while (iVar6 < DAT_00a909a8);
      }
      DAT_0043a590 = 1;
    }
  }
  return;
}


