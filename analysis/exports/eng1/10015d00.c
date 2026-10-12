
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_10015d00(void)

{
  short sVar1;
  undefined2 uVar2;
  undefined *puVar3;
  uint uVar4;
  int iVar5;
  byte bVar6;
  char *pcVar7;
  ushort uVar8;
  short sVar9;
  undefined2 extraout_var;
  int iVar10;
  short sVar11;
  
  *(short *)(DAT_10072860 + 0xc) = (short)DAT_10072860[0xc] + -1;
  if ((short)DAT_10072860[0xc] < 0) {
    do {
      *(short *)((int)DAT_10072860 + 0x32) = *(short *)((int)DAT_10072860 + 0x32) + 1;
      iVar10 = (int)*(short *)((int)DAT_10072860 + 0x32);
      puVar3 = (undefined *)DAT_10072860[0x19];
      sVar1 = *(short *)(puVar3 + iVar10 * 4);
      if (-1 < sVar1) goto switchD_10015d61_default;
      switch(sVar1 + 0x34) {
      case 0:
        if ((*DAT_10070f24 == '\x03') || (*DAT_10070f24 == '\x01')) {
          if ((DAT_100718e0 == 0) || (DAT_10072860[1] >> 0x10 <= (int)(uint)DAT_100718e0)) {
            if (DAT_1006efa8 == 3) {
              FUN_10006da0(0x401e,(int)DAT_1007217c);
              DAT_1006efa8 = 2;
            }
            else if (DAT_1006efa8 == 2) {
              FUN_10006da0(0x401f,(int)DAT_1007217c);
              DAT_1006efa8 = 1;
            }
            else if (DAT_1006efa8 == 1) {
              FUN_10006da0(0x4020,(int)DAT_1007217c);
              DAT_1006efa8 = 0;
            }
          }
          else {
            FUN_10006da0(*(ushort *)(puVar3 + iVar10 * 4 + 2),(int)DAT_1007217c);
            DAT_1006efa8 = 3;
          }
        }
        break;
      case 1:
        DAT_10070f24[3] = '\x02';
        break;
      case 2:
        _DAT_1006ef9c =
             CONCAT22(DAT_1006ef9c_2,
                      (-(ushort)(puVar3 != &DAT_10053c40) & 0xffe0) + 0x10 + (short)_DAT_1006ef9c) &
             0xffff03ff;
        break;
      case 3:
        DAT_1006ef98 = *(undefined2 *)(puVar3 + iVar10 * 4 + 2);
        break;
      case 4:
        if (((DAT_10072110 == 0) &&
            (((DAT_10070f24[0x5c] & 0x80U) == 0 || ((char)DAT_10072860[0x25] == '\0')))) &&
           (((DAT_10070f24[0x5c] & 0x20U) == 0 || ((char)DAT_10072860[0x25] != '\0')))) {
          DAT_10072860[0x19] = (int)&DAT_10054850;
          *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        }
        break;
      case 5:
        if ((((DAT_10072110 == 0) && ((DAT_10070f24[0x5c] & 0x80U) != 0)) &&
            ((char)DAT_10072860[0x25] == '\0')) ||
           (((DAT_10070f24[0x5c] & 0x20U) != 0 && ((char)DAT_10072860[0x25] != '\0')))) {
          DAT_10072860[0x19] = (int)&DAT_10054890;
          *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
LAB_100161f3:
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        }
        break;
      case 6:
        if (DAT_1007217c[0x18] != 0) {
          DAT_1007217c[0x15] = DAT_1007217c[0x15] & 0xff7fffff;
        }
        break;
      case 7:
        iVar5 = DAT_1007217c[0x18];
        if (iVar5 != 0) {
          *(undefined2 *)(iVar5 + 0x34) = *(undefined2 *)(puVar3 + iVar10 * 4 + 2);
          DAT_1006efa0 = 5;
          DAT_1007217c[0x15] = DAT_1007217c[0x15] | 0x800000;
          *(undefined4 *)(iVar5 + 0x54) = 0x4000605;
          bVar6 = *(byte *)(DAT_1007217c + 0x25);
          *(undefined1 *)(iVar5 + 0x22) = 1;
          *(uint *)(iVar5 + 0x54) = bVar6 & 0x10 | 0x4000605;
          if ((((*(short *)(iVar5 + 0x34) == 0x1df) || (*(short *)(iVar5 + 0x34) == 0x1e0)) &&
              ((*DAT_10070f24 == '\x03' || (*DAT_10070f24 == '\x01')))) &&
             ((DAT_100718e0 != 0 && ((int)(DAT_100718e0 - 8) < DAT_10072860[1] >> 0x10)))) {
            DAT_1007217c[0x15] = DAT_1007217c[0x15] & 0xff7fffff;
          }
        }
        break;
      case 8:
        DAT_1006ef84 = 0;
        break;
      case 9:
        _DAT_10072012 = _DAT_10072012 & ~*(ushort *)(puVar3 + iVar10 * 4 + 2);
        break;
      case 10:
        _DAT_10072012 = _DAT_10072012 | *(ushort *)(puVar3 + iVar10 * 4 + 2);
        break;
      case 0xb:
        FUN_1001a280();
        break;
      case 0xc:
        FUN_1001a220();
        break;
      case 0xd:
        if (((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) ||
           (DAT_1006efb4 != '\x02')) {
          DAT_10072108 = 0x2e;
          DAT_10072860[0x19] = (int)&DAT_100541d8;
LAB_10016423:
          *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
          FUN_10015d00();
        }
        break;
      case 0xe:
        DAT_1006ef90._0_2_ = 1;
        break;
      case 0xf:
        DAT_1006ef7c = 0;
        break;
      case 0x10:
        DAT_1006ef7c = 1;
        break;
      case 0x11:
        if ((*(ushort *)(DAT_10070f24 + 0x70) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
          if ((DAT_10070f24[0x5c] & 0xa0U) != 0) goto LAB_10016440;
          DAT_10072108 = 0x29;
          DAT_10072860[0x19] = (int)&DAT_10053f20;
          goto LAB_10016423;
        }
        break;
      case 0x12:
        if ((DAT_10070f24[0x5c] & 0x40U) == 0) {
LAB_10016440:
          FUN_10016790();
        }
        else {
          DAT_10072108 = 0xb;
          DAT_10072860[0x19] = (int)&DAT_100547e8;
          *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
          FUN_10015d00();
        }
        break;
      case 0x13:
        if ((*(ushort *)(DAT_10070f24 + 0x70) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
          if ((DAT_10070f24[0x5c] & 0x40U) == 0) {
            DAT_10072860[0x19] = (int)(puVar3 + iVar10 * 4 + 4);
          }
          goto switchD_10015d61_caseD_33;
        }
        DAT_10072108 = 0xe;
        DAT_10072860[0x19] = (int)&DAT_10054078;
LAB_10016383:
        *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
        *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        break;
      case 0x14:
        if ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
          DAT_10072108 = 0x2e;
          DAT_10072860[0x19] = (int)&DAT_10054470;
          goto LAB_10016383;
        }
        break;
      case 0x15:
        if ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) {
          DAT_1007217c[0x15] = DAT_1007217c[0x15] & 0xff7fffff;
          DAT_1006ef7c = 0;
          DAT_10072108 = 0x2e;
          DAT_10072860[0x19] = (int)&DAT_100541b0;
          goto LAB_10016383;
        }
        break;
      case 0x16:
        if (((*(ushort *)(DAT_10070f24 + 0x72) & *(ushort *)(DAT_10070f24 + 0x5c)) == 0) ||
           (DAT_10072110 != 0)) {
          DAT_10072860[0x19] = (int)&DAT_10054018;
          *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
          FUN_10007f8e(0x15,(int)DAT_1007217c);
        }
        else {
          DAT_10072194 = DAT_10072194 + 0x10;
          if (0xff < DAT_10072194) {
            if (DAT_1007216c == 0) {
              DAT_1007216c = 1;
              FUN_10006da0(0x4029,(int)DAT_1007217c);
            }
            DAT_10072194 = 0xff;
          }
        }
        break;
      case 0x17:
        if (DAT_1006ef88 == 0) {
          FUN_10006da0(*(ushort *)(puVar3 + iVar10 * 4 + 2),(int)DAT_1007217c);
        }
        break;
      case 0x18:
        sVar1 = DAT_1007216a;
        break;
      case 0x19:
        if ((*(ushort *)(DAT_10070f24 + 0x6c) & *(ushort *)(DAT_10070f24 + 0x5c)) != 0)
        goto LAB_100161f3;
        DAT_10072860[0x19] = (int)(puVar3 + iVar10 * 4 + 4);
        *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        break;
      case 0x1a:
        FUN_1001a410();
        break;
      case 0x1b:
        FUN_1001a3e0();
        break;
      case 0x1c:
        *DAT_1007217c =
             *DAT_1007217c + ((-(uint)((char)DAT_10072860[0x25] != '\0') & 0x200000) - 0x100000);
        break;
      case 0x1d:
        iVar10 = DAT_1007217c[0x18];
        if (iVar10 != 0) {
          DAT_1006efa0 = 2;
          DAT_1007217c[0x15] = DAT_1007217c[0x15] | 0x800000;
          uVar4 = *(uint *)(iVar10 + 0x54);
          uVar2 = *(undefined2 *)(DAT_10072860[0x19] + 2 + *(short *)((int)DAT_10072860 + 0x32) * 4)
          ;
          *(uint *)(iVar10 + 0x54) = uVar4 & 0xfffffffb;
          *(undefined2 *)(iVar10 + 0x34) = uVar2;
          bVar6 = *(byte *)(DAT_1007217c + 0x25);
          *(undefined1 *)(iVar10 + 0x22) = 1;
          *(uint *)(iVar10 + 0x54) = bVar6 & 0x10 | uVar4 & 0xffffffeb;
        }
        break;
      case 0x1e:
        DAT_1007217c[0x15] =
             DAT_1007217c[0x15] &
             ~((int)*(short *)(DAT_1007217c[0x19] + 2 + *(short *)((int)DAT_1007217c + 0x32) * 4) <<
              0x10);
        break;
      case 0x1f:
        DAT_1007217c[0x15] =
             DAT_1007217c[0x15] &
             ~(int)*(short *)(DAT_1007217c[0x19] + 2 + *(short *)((int)DAT_1007217c + 0x32) * 4);
        break;
      case 0x20:
        DAT_1007217c[0x15] =
             DAT_1007217c[0x15] |
             (int)*(short *)(DAT_1007217c[0x19] + 2 + *(short *)((int)DAT_1007217c + 0x32) * 4) <<
             0x10;
        break;
      case 0x21:
        DAT_1007217c[0x15] =
             DAT_1007217c[0x15] |
             (int)*(short *)(DAT_1007217c[0x19] + 2 + *(short *)((int)DAT_1007217c + 0x32) * 4);
        break;
      case 0x22:
        FUN_10030180((int)DAT_1007217c,*DAT_10072860,DAT_10072860[1],DAT_10072860[2],
                     CONCAT22((short)((uint)puVar3 >> 0x10),*(undefined2 *)(puVar3 + iVar10 * 4 + 2)
                             ));
        break;
      case 0x23:
        FUN_100300c0(*DAT_10072860,DAT_10072860[1],DAT_10072860[2],
                     CONCAT22((short)((uint)(sVar1 + 0x34) >> 0x10),
                              *(undefined2 *)(puVar3 + iVar10 * 4 + 2)));
        break;
      case 0x24:
        DAT_1007183c = *(undefined2 *)(puVar3 + iVar10 * 4 + 2);
        break;
      case 0x25:
        DAT_1007217c[0x14] = DAT_1007217c[0x14] & ~(int)*(short *)(puVar3 + iVar10 * 4 + 2);
        break;
      case 0x26:
        DAT_1007217c[0x14] = DAT_1007217c[0x14] | (int)*(short *)(puVar3 + iVar10 * 4 + 2);
        break;
      case 0x27:
        *(undefined *)((int)DAT_1007217c + 0x22) = puVar3[iVar10 * 4 + 2];
        break;
      case 0x28:
        bVar6 = DAT_10070f24[0x5c] & 0x10;
        goto LAB_10015d7e;
      case 0x29:
        if (DAT_10070f24[0xc] == '\0') {
          DAT_10070f24[3] = '\"';
        }
        else {
          DAT_10070f24[3] = '!';
        }
        break;
      case 0x2a:
        DAT_1007286c = 0xf8;
        DAT_1004d858._0_2_ = 0x80;
        DAT_1006ee80 = 0;
        FUN_1002c4e0(1);
        pcVar7 = DAT_10070f24;
        pcVar7[0x1b30] = '\x04';
        pcVar7[0x1b31] = '\0';
        pcVar7[0x1b32] = '\0';
        pcVar7[0x1b33] = '\0';
        break;
      case 0x2b:
        DAT_10070f24[3] = '1';
        break;
      case 0x2c:
        if (puVar3 != &DAT_10053830) {
          DAT_10072860[0x13] = 2;
        }
        DAT_10072860[0x19] = (int)&DAT_10053820;
LAB_10015f3f:
        *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
        break;
      case 0x2d:
        if (DAT_1007201c == 0) {
          sVar11 = 0;
          if (DAT_10072860[0x13] == 0) {
            uVar8 = FUN_10030010((-(uint)((char)DAT_10072860[0x25] != '\0') & 0xff900000) + 0x380000
                                 + *DAT_10072860,DAT_10072860[1] + -0x10000,DAT_1007217c[2],1);
            if (CONCAT22(extraout_var,uVar8) != 0) {
              sVar11 = 2;
            }
            sVar9 = FUN_1002c460(0x14);
            sVar11 = sVar11 + sVar9;
          }
          else {
            sVar11 = 10;
            DAT_10072860[0x13] = DAT_10072860[0x13] + -1;
          }
          if (sVar11 == 0) {
            DAT_10072860[0x19] = (int)&DAT_100538d0;
          }
          else if (sVar11 == 1) {
            DAT_10072860[0x19] = (int)&DAT_10053930;
          }
          else if (sVar11 == 2) {
            DAT_10072860[0x19] = (int)&DAT_10053878;
          }
          else {
            DAT_10072860[0x19] = (int)&DAT_10053830;
          }
          goto LAB_10015f3f;
        }
        DAT_10072860[0x19] = (int)&DAT_10053a10;
        *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        *(undefined2 *)(DAT_10072860 + 0xc) = 0xffff;
        break;
      case 0x2e:
        DAT_10072860[0x15] = DAT_10072860[0x15] ^ 0x10;
        break;
      case 0x2f:
        bVar6 = DAT_10070f24[0x5c] & 0x40;
LAB_10015d7e:
        if (bVar6 == 0) {
switchD_10015d61_caseD_32:
          DAT_10072860[0x19] = (int)(puVar3 + iVar10 * 4 + 4);
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        }
        else {
switchD_10015d61_caseD_33:
          *(undefined2 *)((int)DAT_10072860 + 0x32) = 0xffff;
        }
        break;
      case 0x30:
        *(byte *)(DAT_10072860 + 0x25) = ~*(byte *)(DAT_10072860 + 0x25);
        DAT_1007217c[0x15] =
             *(byte *)(DAT_1007217c + 0x25) & 0x10 | DAT_1007217c[0x15] & 0xffffffefU;
        iVar10 = DAT_1007217c[0x18];
        if (iVar10 != 0) {
          *(uint *)(iVar10 + 0x54) =
               *(byte *)(DAT_1007217c + 0x25) & 0x10 | *(uint *)(iVar10 + 0x54) & 0xffffffef;
        }
        break;
      case 0x31:
        return 1;
      case 0x32:
        goto switchD_10015d61_caseD_32;
      case 0x33:
        goto switchD_10015d61_caseD_33;
      }
switchD_10015d61_default:
    } while (sVar1 < 0);
    *(short *)(DAT_10072860 + 0xd) = sVar1;
    *(undefined2 *)(DAT_10072860 + 0xc) =
         *(undefined2 *)(DAT_10072860[0x19] + 2 + *(short *)((int)DAT_10072860 + 0x32) * 4);
  }
  return 0;
}


