
int FUN_0040bec0(short param_1,short param_2,undefined4 param_3,undefined4 *param_4)

{
  int iVar1;
  undefined4 *puVar2;
  
  if ((((&DAT_00a8e790)[param_1] != 0) && (-1 < param_2)) && (param_2 < 0x80)) {
    puVar2 = param_4;
    for (iVar1 = 8; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    }
    *(undefined1 *)((int)param_4 + 2) = *(undefined1 *)(param_2 + 0x144 + (&DAT_00a8e790)[param_1]);
    return (uint)(ushort)(param_2 >> 0xf) << 0x10;
  }
  return CONCAT22((short)((uint)(&DAT_00a8e790)[param_1] >> 0x10),0xffff);
}

