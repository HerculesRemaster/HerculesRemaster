
undefined4 __cdecl FUN_1001f9e0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  if (DAT_10070ecf <= param_1) {
    return 0;
  }
  uVar4 = (uint)*(ushort *)(DAT_10070d48 + param_1 * 0xc);
  iVar2 = (int)*(short *)(DAT_10070d48 + param_1 * 0xc + 2);
  if ((&DAT_10071e40)[param_1 + DAT_10070cfc._2_1_ * 8] != 0) {
    FUN_1002c2d0((&DAT_10071e40)[param_1 + DAT_10070cfc._2_1_ * 8]);
  }
  iVar3 = iVar2 * 0x800;
  iVar1 = iVar3 + 0x1000;
  FUN_1002c3c0(&DAT_10071e40 + param_1 + DAT_10070cfc._2_1_ * 8,iVar1,0x20);
  if ((&DAT_10071e40)[param_1 + DAT_10070cfc._2_1_ * 8] == 0) {
    FUN_1001f6a0(0);
    FUN_1002c3c0(&DAT_10071e40 + param_1 + DAT_10070cfc._2_1_ * 8,iVar1,0x20);
    if ((&DAT_10071e40)[param_1 + DAT_10070cfc._2_1_ * 8] == 0) {
      return 0;
    }
  }
  DAT_10070c50 = (undefined1)param_1;
  if (param_2 == 0) {
    FUN_1002c400((&PTR_s_O__PLAYROOM__1005a690)[*DAT_10070f24 * 6],s_MAP_PSX_BIN_10058a68);
    FUN_1002c4f0(uVar4);
    FUN_1002c480((&DAT_10071e40)[param_1 + DAT_10070cfc._2_1_ * 8],iVar2);
    FUN_1002c280();
    FUN_1001fb90();
    return 1;
  }
  *(code **)(DAT_10070f24 + 0xb0) = FUN_1001fb90;
  *(undefined1 **)(DAT_10070f24 + 0xb4) = &LAB_1001fc60;
  FUN_1002c230((&PTR_s_O__PLAYROOM__1005a690)[*DAT_10070f24 * 6],s_MAP_PSX_BIN_10058a68,uVar4,
               (&DAT_10071e40)[param_1 + DAT_10070cfc._2_1_ * 8],iVar3);
  return 1;
}


