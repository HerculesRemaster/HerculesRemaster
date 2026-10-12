
uint FUN_0040c160(undefined4 param_1,short param_2,short param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar1 = (short)param_1 * 4;
  *(int *)(&DAT_00a8e7d0 + iVar1) = (int)DAT_0043a480 * (int)param_2 >> 7;
  iVar3 = (int)DAT_0043a484;
  *(int *)(&DAT_00a8e830 + iVar1) = iVar3 * param_3 >> 7;
  uVar2 = FUN_0040fbf0(0xffffffff,param_1,
                       CONCAT22((short)((uint)iVar1 >> 0x10),*(undefined2 *)(&DAT_00a8e7d0 + iVar1))
                       ,CONCAT22((short)(iVar3 * param_3 >> 0x17),
                                 *(undefined2 *)(&DAT_00a8e830 + iVar1)));
  return uVar2 & 0xffff0000;
}

