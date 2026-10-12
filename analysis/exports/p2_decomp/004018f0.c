
void FUN_004018f0(char param_1,uint param_2)

{
  uint uVar1;
  undefined2 uVar2;
  undefined2 extraout_var;
  undefined2 extraout_var_00;
  uint uVar3;
  
  FUN_00401b40();
  uVar1 = (uint)(byte)(param_1 + 1U);
  uVar3 = uVar1;
  if ((byte)(param_1 + 1U) == 0) {
    uVar3 = 1;
  }
  if (*(short *)(PTR_DAT_004361bc + uVar1 * 2 + 0x2b0) == -1) {
    FUN_004063c0(s_ERROR_Specified_vabid_is_not_loa_00435ddc);
    return;
  }
  FUN_004015d0();
  uVar2 = FUN_0040c250(*(undefined4 *)
                        (PTR_DAT_004361bc + (uVar1 + (param_2 & 0xff) * 5) * 4 + 0x24c),uVar3);
  *(undefined2 *)(PTR_DAT_004361bc + uVar1 * 2 + 0x1af0) = uVar2;
  FUN_004063c0(s_StartMusic_0x_x_0x_x_0x_x_00435dc0,*(undefined4 *)(PTR_DAT_004361bc + 0x1b04),
               *(undefined4 *)(PTR_DAT_004361bc + 0x1b0c),*(undefined4 *)(PTR_DAT_004361bc + 0x1b14)
              );
  FUN_0040be30(*(undefined2 *)(PTR_DAT_004361bc + uVar1 * 2 + 0x1af0),
               CONCAT22(extraout_var,*(undefined2 *)(PTR_DAT_004361bc + 0x1b14)),
               CONCAT22(extraout_var,*(undefined2 *)(PTR_DAT_004361bc + 0x1b14)));
  FUN_0040c280(CONCAT22(extraout_var_00,*(undefined2 *)(PTR_DAT_004361bc + uVar1 * 2 + 0x1af0)),1,0)
  ;
  *(int *)(PTR_DAT_004361bc + 0x1afc) = (int)*(short *)(PTR_DAT_004361bc + uVar1 * 2 + 0x1af0);
  FUN_004063c0(s_StartMusic__d__d_00435dac,uVar1,param_2 & 0xff);
  return;
}

