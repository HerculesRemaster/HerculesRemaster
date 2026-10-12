
undefined4 FUN_00401110(uint param_1,byte param_2)

{
  int iVar1;
  int *piVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  
  uVar7 = (uint)param_2;
  uVar8 = param_1 & 0xff;
  FUN_004063c0(s_LoadMusic__d__d_00435c18,uVar8,uVar7);
  *(ushort *)(PTR_DAT_004361bc + uVar7 * 2 + 0x2b0) = (ushort)param_1 & 0xff;
  uVar4 = FUN_004040d0((&PTR_s_q__PLAYROOM__00435178)[uVar8 * 2],s_MUSIC_MUS_00435c0c);
  iVar1 = uVar7 * 4 + 0x13c;
  iVar5 = FUN_00404570(PTR_DAT_004361bc + iVar1,(uVar4 & 0xfffff800) + 0x1000,0x10);
  if (iVar5 == 0) {
    return 0xffffffff;
  }
  FUN_00403b50((&PTR_s_q__PLAYROOM__00435178)[uVar8 * 2],s_MUSIC_MUS_00435c0c,
               *(undefined4 *)(PTR_DAT_004361bc + iVar1),uVar4);
  iVar5 = uVar7 * 4 + 0x150;
  piVar2 = *(int **)(PTR_DAT_004361bc + iVar1);
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x164) = *piVar2 + (int)piVar2;
  *(int *)(PTR_DAT_004361bc + iVar5) = piVar2[1] + (int)piVar2;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x24c) = piVar2[2] + (int)piVar2;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x260) = piVar2[3] + (int)piVar2;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x274) = piVar2[4] + (int)piVar2;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x288) = piVar2[5] + (int)piVar2;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x29c) = piVar2[6] + (int)piVar2;
  FUN_0040be30();
  sVar3 = FUN_0040c040(*(undefined4 *)(PTR_DAT_004361bc + iVar5),param_2);
  if (sVar3 < 0) {
    FUN_004063c0(s_Unable_to_open_VAB_00435bf4);
    return 0xffffffff;
  }
  sVar3 = FUN_0040c090(*(undefined4 *)(PTR_DAT_004361bc + uVar7 * 4 + 0x164),param_2);
  if (sVar3 < 0) {
    FUN_004063c0(s_Unable_to_down_load_VAB_00435bd8);
    return 0xffffffff;
  }
  FUN_0040c240(1);
  uVar6 = FUN_0040a340(param_2);
  *(undefined4 *)(PTR_DAT_004361bc + uVar7 * 4 + 0x178) = uVar6;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x18c) =
       *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x178) +
       (*(int *)(PTR_DAT_004361bc + iVar5) - *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x164) >> 2) *
       4;
  FUN_00404690(*(undefined4 *)(PTR_DAT_004361bc + iVar1),
               (int)piVar2 + (uVar4 & 0xfffff800) + (0x1000 - *(int *)(PTR_DAT_004361bc + iVar5)),0,
               1);
  return 0;
}

