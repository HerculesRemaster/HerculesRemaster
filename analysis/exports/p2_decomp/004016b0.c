
undefined4 FUN_004016b0(char *param_1)

{
  char cVar1;
  int iVar2;
  byte *pbVar3;
  int iVar4;
  
  iVar4 = 0;
  do {
    iVar2 = iVar4 + 3;
    PTR_DAT_004361bc[iVar4 + 0x2f0] = 0xff;
    PTR_DAT_004361bc[iVar4 + 0x2f1] = 0xff;
    iVar4 = iVar2;
  } while (iVar2 < 0x1800);
  FUN_004063c0(s_LoadLevelSonix___________________00435d84);
  if (*param_1 == -1) {
    pbVar3 = (byte *)(param_1 + 2);
    *(uint *)(PTR_DAT_004361bc + 0x1b04) = (uint)(byte)param_1[1];
    param_1 = param_1 + 3;
    *(uint *)(PTR_DAT_004361bc + 0x1b08) = (uint)*pbVar3;
  }
  else {
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b04) = 0x80;
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b08) = 0x80;
  }
  FUN_00401000();
  if (*param_1 == -2) {
    FUN_0040be30(param_1[1],param_1[1]);
    param_1 = param_1 + 2;
    FUN_00401f40();
  }
  else {
    FUN_0040be30(0,0);
  }
  FUN_004063c0(s_LoadLevelSonix_setting_Music__d_S_00435d58,
               *(undefined4 *)(PTR_DAT_004361bc + 0x1b04),*(undefined4 *)(PTR_DAT_004361bc + 0x1b08)
              );
  FUN_00401110(*param_1,1);
  iVar4 = 2;
  cVar1 = param_1[1];
  while (cVar1 != -1) {
    if (cVar1 != 'c') {
      FUN_004063c0(s_Load_Level_Sonix__d_00435d40,cVar1);
      FUN_00401330(param_1[1],iVar4);
      iVar4 = iVar4 + 1;
    }
    cVar1 = param_1[2];
    param_1 = param_1 + 1;
  }
  return 0;
}

