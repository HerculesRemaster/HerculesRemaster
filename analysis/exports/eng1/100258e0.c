
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_100258e0(void)

{
  undefined1 uVar1;
  ushort uVar2;
  uint uVar3;
  undefined2 *puVar4;
  undefined4 *puVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  
  DAT_10071840 = 0;
  DAT_10071bc4 = (undefined2)((uint)*(undefined4 *)(DAT_10072860 + 8) >> 0x10);
  DAT_10071c5c = (undefined2)((uint)*(undefined4 *)(DAT_10072860 + 8) >> 0x10);
  _DAT_100716a8 =
       ((int)*(short *)(&DAT_1005a848 + (*(uint *)(DAT_10070f24 + 0x38) & 0x7f) * 0x10) << 6) >>
       0x10;
  DAT_10071ca0 = 0;
  DAT_10071740 = 0;
  DAT_10071720 = 0;
  _DAT_10071cc0 = 0;
  DAT_10071680 = 0;
  _DAT_10071c60 = 0;
  DAT_10071718 = *(short *)(&DAT_1005a848 + (*(int *)(DAT_10070f24 + 0x38) + 0x10U & 0x3f) * 0x20) *
                 0x50 >> 0x10;
  FUN_10026480();
  DAT_100714c0 = 0;
  DAT_10071760 = 0;
  DAT_100717c0 = 0;
  DAT_100716c0 = 0;
  DAT_10071580 = 0;
  DAT_10071a20 = 0;
  DAT_10071be0 = 0;
  DAT_10071b20 = 0;
  iVar6 = 0;
  puVar4 = &DAT_1005cef0;
  do {
    uVar1 = *(undefined1 *)puVar4;
    puVar4 = puVar4 + 10;
    (&DAT_100719c0)[iVar6] = uVar1;
    iVar6 = iVar6 + 1;
  } while ((int)puVar4 < 0x1005d544);
  uVar7 = (uint)DAT_1007138c;
  if (uVar7 < DAT_100713b6) {
    iVar6 = uVar7 * 0x94;
    do {
      uVar2 = *(ushort *)(DAT_100734e0 + 0x2e + iVar6);
      puVar5 = (undefined4 *)(DAT_100734e0 + iVar6);
      if (uVar2 != 0) {
        if ((&DAT_100719c0)[*(byte *)((int)puVar5 + 0x22)] == '\x04') {
          FUN_10027e20(puVar5);
        }
        else {
          uVar3 = puVar5[0x14];
          if ((uVar3 & 0x40) != 0) {
            DAT_10071a20 = DAT_10071a20 + 1;
            (&DAT_10071a20)[DAT_10071a20] = (int)puVar5;
          }
          if ((uVar3 & 0x200) != 0) {
            DAT_10071580 = DAT_10071580 + 1;
            (&DAT_10071580)[DAT_10071580] = (int)puVar5;
          }
          if ((uVar3 & 1) != 0) {
            (**(code **)(DAT_1007139c + 4 + (uVar2 & 0x1ff) * 8))(puVar5);
          }
        }
      }
      uVar7 = uVar7 + 1;
      iVar6 = iVar6 + 0x94;
    } while ((int)uVar7 < (int)(uint)DAT_100713b6);
  }
  iVar6 = 0;
  if (DAT_100713a0 != 0) {
    iVar8 = 0;
    do {
      puVar5 = (undefined4 *)(DAT_10073494 + iVar8);
      uVar2 = *(ushort *)((int)puVar5 + 0x2e);
      if (uVar2 != 0) {
        if ((&DAT_100719c0)[*(byte *)((int)puVar5 + 0x22)] == '\x04') {
          FUN_10027860(puVar5);
        }
        else {
          uVar7 = puVar5[0x14];
          if ((uVar7 & 0x800) == 0) {
            puVar5[0x27] = *puVar5;
            puVar5[0x28] = puVar5[1];
          }
          if ((uVar7 & 0x40000) != 0) {
            DAT_100714c0 = DAT_100714c0 + 1;
            (&DAT_100714c0)[DAT_100714c0] = (int)puVar5;
          }
          if ((uVar7 & 0x100000) != 0) {
            DAT_10071b20 = DAT_10071b20 + 1;
            (&DAT_10071b20)[DAT_10071b20] = (int)puVar5;
          }
          if ((uVar7 & 0x200) != 0) {
            DAT_10071580 = DAT_10071580 + 1;
            (&DAT_10071580)[DAT_10071580] = (int)puVar5;
          }
          if ((uVar7 & 0x800000) != 0) {
            DAT_10071be0 = DAT_10071be0 + 1;
            (&DAT_10071be0)[DAT_10071be0] = (int)puVar5;
          }
          if ((uVar7 & 1) != 0) {
            (**(code **)(DAT_1007137c + 4 + (uVar2 & 0x1ff) * 8))(puVar5);
          }
        }
      }
      iVar6 = iVar6 + 1;
      iVar8 = iVar8 + 0xec;
    } while (iVar6 < (int)(uint)DAT_100713a0);
  }
  uVar7 = (uint)DAT_10071378;
  if (uVar7 < DAT_100713a8) {
    iVar6 = uVar7 * 0x134;
    do {
      puVar5 = (undefined4 *)(DAT_10072860 + iVar6);
      if (*(short *)(DAT_10072860 + 0x2e + iVar6) != 0) {
        if ((&DAT_100719c0)[*(byte *)((int)puVar5 + 0x22)] == '\x04') {
          FUN_10027260(puVar5);
        }
        else {
          uVar3 = puVar5[0x14];
          if ((uVar3 & 0x800) == 0) {
            puVar5[0x27] = *puVar5;
            puVar5[0x28] = puVar5[1];
            puVar5[0x29] = puVar5[2];
          }
          if ((uVar3 & 0x100000) != 0) {
            DAT_10071b20 = DAT_10071b20 + 1;
            (&DAT_10071b20)[DAT_10071b20] = (int)puVar5;
          }
          if ((uVar3 & 0x80) != 0) {
            DAT_100716c0 = DAT_100716c0 + 1;
            (&DAT_100716c0)[DAT_100716c0] = (int)puVar5;
          }
          if ((uVar3 & 0x200) != 0) {
            DAT_10071580 = DAT_10071580 + 1;
            (&DAT_10071580)[DAT_10071580] = (int)puVar5;
          }
          if ((uVar3 & 0x400000) != 0) {
            DAT_100717c0 = DAT_100717c0 + 1;
            (&DAT_100717c0)[DAT_100717c0] = (int)puVar5;
          }
          if ((uVar3 & 0x800000) != 0) {
            DAT_10071be0 = DAT_10071be0 + 1;
            (&DAT_10071be0)[DAT_10071be0] = (int)puVar5;
          }
          if ((uVar3 & 0x10) == 0) {
            if (*(char *)(puVar5 + 0x3b) == '\0') {
              *(undefined1 *)((int)puVar5 + 0xed) = 0;
            }
            else {
              *(char *)(puVar5 + 0x3b) = *(char *)(puVar5 + 0x3b) + -1;
            }
          }
          else {
            DAT_10071760 = DAT_10071760 + 1;
            (&DAT_10071760)[DAT_10071760] = (int)puVar5;
          }
          if ((uVar3 & 0x240000) != 0) {
            DAT_100714c0 = DAT_100714c0 + 1;
            (&DAT_100714c0)[DAT_100714c0] = (int)puVar5;
          }
          if ((uVar3 & 1) != 0) {
            (**(code **)(DAT_10071370 + 4 + (*(ushort *)((int)puVar5 + 0x2e) & 0x1ff) * 0xc))
                      (puVar5);
          }
        }
      }
      uVar7 = uVar7 + 1;
      iVar6 = iVar6 + 0x134;
    } while ((int)uVar7 < (int)(uint)DAT_100713a8);
  }
  return;
}


