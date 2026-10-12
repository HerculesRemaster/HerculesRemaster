
uint __cdecl FUN_10004ca0(char *param_1,int param_2,byte param_3)

{
  byte *pbVar1;
  char cVar2;
  byte bVar3;
  byte bVar4;
  undefined2 uVar5;
  uint uVar6;
  undefined2 *puVar7;
  ushort uVar8;
  int iVar9;
  int iVar10;
  byte *pbVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  undefined2 *puVar15;
  ushort *puVar16;
  uint uVar17;
  byte *pbVar18;
  byte *pbVar19;
  int local_20;
  int local_1c;
  uint local_18;
  short *local_14;
  int local_10;
  byte *local_c;
  
  local_c = (byte *)(param_1 + param_2 * 4);
  local_14 = (short *)0x0;
  local_18 = 0xffffffff;
  local_20 = DAT_1006ee78;
  local_1c = DAT_1006ee7c;
  iVar14 = DAT_1006ee78;
  iVar12 = DAT_1006ee7c;
  do {
    uVar6 = (uint)*param_1;
    if ((uVar6 & 0x80) == 0) {
      iVar10 = ((int)uVar6 >> 1 & 1U) + ((int)uVar6 >> 2 & 2U);
      if ((iVar10 != 3) && (iVar13 = 0, 0 < iVar12)) {
        puVar16 = (ushort *)&DAT_100734a0;
        do {
          uVar6 = (uint)*puVar16;
          uVar8 = (&DAT_10072880)[uVar6 * 6];
          if (uVar8 != 0xf) {
            if (iVar10 == 0) {
              uVar17 = 0;
              iVar14 = 1;
              if ((uVar8 & 1) != 0) {
                iVar14 = 2;
                uVar17 = 1;
                if ((uVar8 & 2) != 0) {
                  iVar14 = 4;
                  uVar17 = 2;
                  if ((uVar8 & 4) != 0) {
                    iVar14 = 8;
                    uVar17 = 3;
                    (&DAT_10072880)[uVar6 * 6] = (&DAT_10072880)[uVar6 * 6] | 8;
                    goto LAB_10004ed8;
                  }
                }
              }
LAB_10004e02:
              (&DAT_10072880)[uVar6 * 6] = (&DAT_10072880)[uVar6 * 6] | (ushort)iVar14;
              goto LAB_10004ed8;
            }
            if (iVar10 == 1) {
              if ((uVar8 & 3) == 0) {
                iVar14 = 3;
                uVar17 = 0;
                (&DAT_10072880)[uVar6 * 6] = (&DAT_10072880)[uVar6 * 6] | 3;
                goto LAB_10004ed8;
              }
              if ((uVar8 & 0xc) == 0) {
                iVar14 = 0xc;
                uVar17 = 2;
                (&DAT_10072880)[uVar6 * 6] = (&DAT_10072880)[uVar6 * 6] | 0xc;
                goto LAB_10004ed8;
              }
            }
            if (iVar10 == 2) {
              if ((uVar8 & 5) == 0) {
                iVar14 = 5;
                uVar17 = 0;
                (&DAT_10072880)[uVar6 * 6] = (&DAT_10072880)[uVar6 * 6] | 5;
                goto LAB_10004ed8;
              }
              if (uVar8 == 0) {
                iVar14 = 9;
                uVar17 = 1;
                goto LAB_10004e02;
              }
            }
          }
          iVar13 = iVar13 + 1;
          puVar16 = puVar16 + 1;
          iVar14 = local_20;
        } while (iVar13 < iVar12);
      }
      local_20 = iVar14;
      uVar17 = 0;
      iVar13 = 0;
      cVar2 = (&DAT_1005e120)[iVar10];
      iVar14 = (int)cVar2;
      if ((param_3 & 0x40) != 0) {
        iVar13 = 0x6f;
        do {
          iVar10 = iVar13;
          if ((&DAT_10072880)[(uint)(byte)(&DAT_1005e138)[iVar13] * 6] == 0) goto LAB_10004e7e;
          iVar13 = iVar13 + -1;
        } while (0x54 < iVar13);
      }
      iVar9 = 0;
      while( true ) {
        if (0x53 < iVar9) {
          uVar8 = (ushort)local_18;
          DAT_1006ee78 = local_20;
          DAT_1006ee7c = iVar12;
          while (uVar8 != 0xffff) {
            cVar2 = (&DAT_1005e128)[local_18 & 0xf];
            iVar13 = ((int)local_18 >> 4) * 0xc;
            (&DAT_10072880)[((int)local_18 >> 4) * 6] =
                 (&DAT_10072880)[((int)local_18 >> 4) * 6] & 0xf - ((ushort)local_18 & 0xf);
            uVar8 = *(ushort *)(&DAT_10072884 + cVar2 * 2 + iVar13);
            local_18 = (uint)uVar8;
          }
          return CONCAT22((short)((uint)iVar13 >> 0x10),0xffff);
        }
        local_20 = local_20 + 1;
        if (0x53 < local_20) {
          local_20 = 0;
        }
        iVar13 = 0;
        iVar10 = local_20;
        if ((&DAT_10072880)[(uint)(byte)(&DAT_1005e138)[local_20] * 6] == 0) break;
        iVar9 = iVar9 + 1;
      }
LAB_10004e7e:
      local_20 = iVar10;
      iVar13 = iVar12;
      if (0x1f < iVar12) {
        FUN_1002c630(0);
        DAT_1006ee7c = iVar12;
        FUN_100050d0();
        FUN_1002c630(0);
        iVar13 = 0;
      }
      uVar6 = (uint)(byte)(&DAT_1005e138)[local_20];
      *(ushort *)((int)&DAT_100734a0 + iVar13 * 2) = (ushort)(byte)(&DAT_1005e138)[local_20];
      local_1c = iVar13 + 1;
      (&DAT_10072880)[uVar6 * 6] = (short)cVar2;
LAB_10004ed8:
      if (local_14 == (short *)0x0) {
        local_18 = uVar6 * 0x10 + iVar14;
      }
      else {
        *local_14 = (short)(uVar6 << 4) + (short)iVar14;
      }
      local_14 = (short *)(&DAT_10072884 + (uVar17 + uVar6 * 6) * 2);
      *local_14 = -1;
      local_10 = ((int)*param_1 & 0xcU) * 2 + 8;
      iVar10 = ((int)*param_1 & 3U) * 2 + 2;
      puVar15 = (undefined2 *)
                ((((uVar17 & 2) + iVar13 * 4) * 0x10 + (uVar17 & 1)) * 0x10 + DAT_10072870);
      pbVar11 = local_c;
      pbVar18 = (byte *)&DAT_1006e660;
      while( true ) {
        while( true ) {
          bVar3 = *pbVar11;
          local_c = pbVar11 + 1;
          if ('\0' < (char)bVar3) break;
          *pbVar18 = bVar3;
          pbVar11 = local_c;
          pbVar18 = pbVar18 + 1;
        }
        if (bVar3 == 2) break;
        pbVar1 = pbVar18 + bVar3;
        if (&DAT_1006ea60 < pbVar1) {
          if (0x53 < local_20) {
            local_20 = 0;
          }
          DAT_1006ee7c = local_1c;
          DAT_1006ee78 = local_20;
          return CONCAT22((short)((uint)local_1c >> 0x10),(undefined2)local_18);
        }
        bVar4 = *local_c;
        pbVar11 = pbVar11 + 2;
        for (uVar6 = (uint)(bVar3 >> 2); uVar6 != 0; uVar6 = uVar6 - 1) {
          *(uint *)pbVar18 = CONCAT22(CONCAT11(bVar4,bVar4),CONCAT11(bVar4,bVar4));
          pbVar18 = pbVar18 + 4;
        }
        pbVar19 = pbVar18;
        for (uVar6 = bVar3 & 3; pbVar18 = pbVar1, uVar6 != 0; uVar6 = uVar6 - 1) {
          *pbVar19 = bVar4;
          pbVar19 = pbVar19 + 1;
        }
      }
      puVar7 = &DAT_1006e660;
      iVar13 = iVar10 * 2;
      iVar9 = iVar10;
      do {
        while (iVar9 != 0) {
          uVar5 = *puVar7;
          puVar7 = puVar7 + 1;
          *puVar15 = uVar5;
          puVar15 = puVar15 + 1;
          iVar13 = iVar13 + -1;
          iVar9 = iVar13;
        }
        puVar15 = puVar15 + (8 - iVar10) * 2;
        local_10 = local_10 + -1;
        iVar14 = local_20;
        iVar12 = local_1c;
        iVar13 = iVar10 * 2;
        iVar9 = iVar10;
      } while (local_10 != 0);
    }
    param_1 = param_1 + 4;
    param_2 = param_2 + -1;
    if (param_2 == 0) {
      if (0x53 < iVar14) {
        iVar14 = 0;
      }
      DAT_1006ee7c = iVar12;
      DAT_1006ee78 = iVar14;
      return local_18 & 0xffff;
    }
  } while( true );
}


