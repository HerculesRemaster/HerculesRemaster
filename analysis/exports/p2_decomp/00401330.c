
undefined4 FUN_00401330(uint param_1,byte param_2)

{
  int *piVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  undefined1 local_30 [6];
  short local_2a;
  undefined1 local_20 [2];
  undefined1 local_1e;
  
  uVar7 = (uint)param_2;
  uVar8 = param_1 & 0xff;
  FUN_004063c0(s_LoadSfx__d__d_00435d1c,uVar8,uVar7);
  *(ushort *)(PTR_DAT_004361bc + uVar7 * 2 + 0x2b0) = (ushort)param_1 & 0xff;
  uVar3 = FUN_004040d0((&PTR_s_q__PLAYROOM__00435178)[uVar8 * 2],s_SFX_SFX_00435d14);
  iVar6 = uVar7 * 4 + 0x13c;
  iVar4 = FUN_00404570(PTR_DAT_004361bc + iVar6,(uVar3 & 0xfffff800) + 0x1000,0x10);
  if (iVar4 == 0) {
    return 0xffffffff;
  }
  FUN_00403b50((&PTR_s_q__PLAYROOM__00435178)[uVar8 * 2],s_SFX_SFX_00435d14,
               *(undefined4 *)(PTR_DAT_004361bc + iVar6),uVar3);
  piVar1 = *(int **)(PTR_DAT_004361bc + iVar6);
  iVar4 = uVar7 * 4 + 0x150;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x164) = *piVar1 + (int)piVar1;
  *(int *)(PTR_DAT_004361bc + iVar4) = piVar1[1] + (int)piVar1;
  FUN_0040be30();
  sVar2 = FUN_0040c040(*(undefined4 *)(PTR_DAT_004361bc + iVar4),param_2);
  if (sVar2 < 0) {
    FUN_004063c0(s_Unable_to_open_VAB_00435bf4);
    return 0xffffffff;
  }
  sVar2 = FUN_0040c090(*(undefined4 *)(PTR_DAT_004361bc + uVar7 * 4 + 0x164),param_2);
  if (sVar2 < 0) {
    FUN_004063c0(s_Unable_to_down_load_VAB_00435bd8);
    return 0xffffffff;
  }
  FUN_0040c240(1);
  uVar5 = FUN_0040a340(param_2);
  *(undefined4 *)(PTR_DAT_004361bc + uVar7 * 4 + 0x178) = uVar5;
  *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x18c) =
       *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x178) +
       (*(int *)(PTR_DAT_004361bc + iVar4) - *(int *)(PTR_DAT_004361bc + uVar7 * 4 + 0x164) >> 2) *
       4;
  FUN_00404690(*(undefined4 *)(PTR_DAT_004361bc + iVar6),
               (int)piVar1 + (uVar3 & 0xfffff800) + (0x1000 - *(int *)(PTR_DAT_004361bc + iVar4)),0,
               1);
  iVar6 = 0;
  do {
    FUN_0040be60(param_2,iVar6,local_30);
    if (local_2a != 0) {
      if (PTR_DAT_004361bc[local_2a * 3 + 0x2f0] != -1) {
        FUN_004063c0(s__________________________________00435cd4);
        FUN_004063c0(s_Two_or_more_sound_effects_have_b_00435c98);
        FUN_004063c0(s_attribute_number___FX_ID__d_Attr_00435c6c,iVar6,(int)local_2a);
        FUN_004063c0(s__________________________________00435c2c);
      }
      PTR_DAT_004361bc[local_2a * 3 + 0x2f0] = param_2;
      PTR_DAT_004361bc[(local_2a + 0xfb) * 3] = (char)iVar6;
      FUN_0040bec0(param_2,iVar6,0,local_20);
      PTR_DAT_004361bc[local_2a * 3 + 0x2f2] = local_1e;
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 0x7f);
  return 0;
}

