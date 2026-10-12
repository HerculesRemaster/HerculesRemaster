
int FUN_1001fc90(void)

{
  int iVar1;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  undefined2 local_2;
  
  iVar1 = DAT_10070ed4;
  local_4 = 0x300;
  local_2 = 0x200;
  local_8 = 0;
  local_6 = 0;
  FUN_1002c7f0(&local_8,DAT_10070ed4 + 0x14 + (*(uint *)(DAT_10070ed4 + 0x14) & 0xfffffffc));
  local_8 = 0x300;
  local_6 = 0x100;
  local_4 = 0x100;
  local_2 = 0x100;
  FUN_1002c7f0(&local_8,iVar1 + 0x18 + (*(uint *)(iVar1 + 0x18) & 0xfffffffc));
  return (iVar1 + 0x1c + *(int *)(iVar1 + 0x1c)) - DAT_10070ed4;
}


