
undefined4 __cdecl FUN_1002e3b0(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  short sVar7;
  ushort uVar8;
  int iVar9;
  ushort *puVar10;
  int iVar11;
  undefined4 local_8;
  undefined2 local_4;
  
  iVar2 = DAT_10070f30;
  puVar10 = (ushort *)(DAT_10070f30 + 0x40);
  piVar1 = *(int **)(param_1 + 8);
  if ((uint)*(ushort *)(param_1 + 0x2a) == *(uint *)(param_1 + 0x40)) {
    iVar4 = *(int *)(param_1 + 0x44);
    local_8 = piVar1;
  }
  else {
    *(uint *)(param_1 + 0x40) = (uint)*(ushort *)(param_1 + 0x2a);
    iVar6 = piVar1[1] - *(int *)(param_1 + 0x5c) >> 6;
    iVar11 = (int)*(short *)(DAT_10070f34 + (*(uint *)(param_1 + 100) - 0x400 & 0xfff) * 2);
    iVar9 = (int)*(short *)(DAT_10070f34 + (*(uint *)(param_1 + 100) & 0xfff) * 2);
    iVar4 = *piVar1 - *(int *)(param_1 + 0x54) >> 6;
    iVar5 = iVar4 * iVar9 - iVar6 * iVar11;
    *(uint *)(iVar2 + 0x14) = CONCAT31((int3)(iVar5 >> 0x19),(char)(iVar5 >> 0x11)) & 0xfffffffe;
    local_4 = 0;
    *(int *)(iVar2 + 0x1c) =
         (*(int *)(param_1 + 0x3c) + (iVar6 * iVar9 + iVar4 * iVar11 >> 0x12)) * 2;
    iVar4 = -((piVar1[2] & 0xfffU) + *(int *)(param_1 + 100));
    sVar7 = (short)iVar4;
    if (iVar4 != 0 || DAT_10070cd0 != 0) {
      sVar7 = sVar7 + -1;
    }
    local_8 = (int *)CONCAT22(sVar7,*(undefined2 *)(param_1 + 0x60));
    FUN_1002c920(&local_8,iVar2);
    FUN_1002c940(iVar2,&DAT_1005ceb0);
    FUN_1002c700(iVar2);
    *(int *)(param_1 + 0x44) = iVar4;
  }
  FUN_1002c710(iVar2);
  iVar5 = *(int *)(param_1 + 0x1c) * 0x40 - *(int *)(param_1 + 0x58);
  iVar6 = (*(int *)(param_1 + 0x18) - (*(ushort *)((int)piVar1 + 0xe) & 0x3fff)) * 0x40;
  uVar3 = (undefined2)iVar5;
  uVar8 = (ushort)iVar6;
  if (iVar4 == 0 && DAT_10070cd0 == 0) {
    *puVar10 = uVar8;
    *(undefined2 *)(iVar2 + 0x42) = uVar3;
    *(undefined2 *)(iVar2 + 0x44) = *(undefined2 *)(param_1 + 0x38);
    FUN_1002c690(puVar10);
    FUN_1002c6e0();
    FUN_1002c6c0();
    FUN_1002c760(puVar10);
    if (0xf0 < *(ushort *)(iVar2 + 0x42)) {
      *(undefined1 *)(param_1 + 0x16) = 1;
    }
    if (0x140 < *puVar10) {
      *(undefined1 *)(param_1 + 0x17) = 1;
    }
    FUN_1002c750(param_1 + 0xc);
    if (0xbf < *(short *)(param_1 + 0x10)) {
      *(short *)(param_1 + 0x10) = *(short *)(param_1 + 0x10) >> 2;
      return 0;
    }
    return 0xffffffff;
  }
  *(undefined2 *)(iVar2 + 0x42) = uVar3;
  *puVar10 = uVar8 + 0x20;
  *(undefined2 *)(iVar2 + 0x44) = *(undefined2 *)(param_1 + 0x38);
  FUN_1002c690(puVar10);
  FUN_1002c6e0();
  FUN_1002c760(puVar10);
  if (*(char *)(param_1 + 0x29) == '\0') {
    if ((short)*puVar10 < 0x141) goto LAB_1002e5b5;
  }
  else if (-1 < (short)*puVar10) goto LAB_1002e5b5;
  *(undefined1 *)(param_1 + 0x17) = 1;
LAB_1002e5b5:
  FUN_1002c750(param_1 + 0xc);
  if (0xff < *(short *)(param_1 + 0x10)) {
    *(int *)(param_1 + 0x48) = iVar6;
    *(short *)(param_1 + 0x10) = *(short *)(param_1 + 0x10) >> 2;
    *(int *)(param_1 + 0x4c) = iVar5;
    return 1;
  }
  return 0xffffffff;
}


