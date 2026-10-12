
void FUN_00405760(undefined4 param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  undefined1 *puVar5;
  int iVar6;
  char *pcVar7;
  char *pcVar8;
  char *pcVar9;
  char acStack_500 [124];
  undefined1 auStack_484 [4];
  char acStack_480 [126];
  char acStack_402 [130];
  char acStack_380 [128];
  undefined1 uStack_300;
  undefined1 auStack_2ff [767];
  
  if (param_2 != 0) {
    uVar2 = FS_Open(param_2,&DAT_00435fe4);
    FS_Seek(uVar2,0x12,0);
    FS_Read(&uStack_300,0x300,1,uVar2);
    FS_Close(uVar2);
    iVar6 = 0;
    puVar5 = auStack_2ff;
    do {
      FUN_00406780(iVar6,puVar5[1],*puVar5,puVar5[-1]);
      iVar6 = iVar6 + 1;
      puVar5 = puVar5 + 3;
    } while (iVar6 < 0x80);
    FUN_004069a0(1);
    return;
  }
  switch(param_1) {
  default:
    goto switchD_004057ff_caseD_0;
  case 1:
    pcVar7 = s_o__training_map_psx_bin_00436744;
    break;
  case 2:
    pcVar7 = s_o__gauntlet_map_psx_bin_00436684;
    break;
  case 3:
    pcVar7 = s_o__forest_map_psx_bin_0043672c;
    break;
  case 5:
    pcVar7 = s_o__thebes_map_psx_bin_00436714;
    break;
  case 6:
    pcVar7 = s_o__hydra_map_psx_bin_004366fc;
    break;
  case 7:
    pcVar7 = s_o__medusa_map_psx_bin_004366e4;
    break;
  case 8:
    pcVar7 = s_o__cyclops_map_psx_bin_0043666c;
    break;
  case 9:
    pcVar7 = s_o__titans_map_psx_bin_004366cc;
    break;
  case 10:
    pcVar7 = s_o__torment_map_psx_bin_0043669c;
    break;
  case 0xb:
    pcVar7 = s_o__vortex_map_psx_bin_004366b4;
    break;
  case 0xfffffffa:
    pcVar7 = s_o__title_tally_map_psx_pal_0043675c;
    break;
  case 0xfffffffb:
    pcVar7 = s_o__title_pswd_map_psx_pal_00436778;
    break;
  case 0xfffffffc:
    pcVar7 = s_o__title_mainmenu_map_psx_pal_00436794;
    break;
  case 0xfffffffd:
    pcVar7 = s_o__title_loadlev_map_psx_pal_004367b4;
    break;
  case 0xfffffffe:
    pcVar7 = s_o__title_credits_map_psx_pal_004367d4;
    break;
  case 0xffffffff:
    pcVar7 = s_o__title_cheat_map_psx_pal_004367f4;
  }
  uVar3 = 0xffffffff;
  do {
    pcVar9 = pcVar7;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar9 = pcVar7 + 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar9;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar7 = pcVar9 + -uVar3;
  pcVar9 = acStack_500;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
    pcVar7 = pcVar7 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar9 = *pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar9 = pcVar9 + 1;
  }
  FUN_00411420(acStack_500,0,acStack_402 + 2,acStack_480,acStack_380);
  uVar3 = 0xffffffff;
  pcVar7 = acStack_380;
  do {
    pcVar9 = pcVar7;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar9 = pcVar7 + 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar9;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  iVar6 = -1;
  pcVar7 = acStack_480;
  do {
    pcVar8 = pcVar7;
    if (iVar6 == 0) break;
    iVar6 = iVar6 + -1;
    pcVar8 = pcVar7 + 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar8;
  } while (cVar1 != '\0');
  pcVar7 = pcVar9 + -uVar3;
  pcVar9 = pcVar8 + -1;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
    pcVar7 = pcVar7 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar9 = *pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar9 = pcVar9 + 1;
  }
  uVar3 = 0xffffffff;
  pcVar7 = acStack_402 + 2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  if (acStack_402[~uVar3] == '\\') {
    uVar3 = 0xffffffff;
    pcVar7 = acStack_402 + 2;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
    acStack_402[~uVar3] = '\0';
  }
  uVar3 = 0xffffffff;
  pcVar7 = acStack_500;
  do {
    pcVar9 = pcVar7;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar9 = pcVar7 + 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar9;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar7 = pcVar9 + -uVar3;
  pcVar9 = acStack_480;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
    pcVar7 = pcVar7 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar9 = *pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar9 = pcVar9 + 1;
  }
  uVar3 = 0xffffffff;
  pcVar7 = acStack_480;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  auStack_484[~uVar3] = 0;
  uVar3 = 0xffffffff;
  pcVar7 = &DAT_00436668;
  do {
    pcVar9 = pcVar7;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar9 = pcVar7 + 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar9;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  iVar6 = -1;
  pcVar7 = acStack_480;
  do {
    pcVar8 = pcVar7;
    if (iVar6 == 0) break;
    iVar6 = iVar6 + -1;
    pcVar8 = pcVar7 + 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar8;
  } while (cVar1 != '\0');
  pcVar7 = pcVar9 + -uVar3;
  pcVar9 = pcVar8 + -1;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
    pcVar7 = pcVar7 + 4;
    pcVar9 = pcVar9 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar9 = *pcVar7;
    pcVar7 = pcVar7 + 1;
    pcVar9 = pcVar9 + 1;
  }
  FUN_004068c0(acStack_480);
  FUN_004069a0(1);
switchD_004057ff_caseD_0:
  return;
}

