
void FUN_10016300(void)

{
  char cVar1;
  undefined2 uStack_10;
  short sStack_e;
  undefined2 uStack_c;
  undefined2 uStack_a;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  undefined2 local_2;
  
  local_8 = 0x390;
  local_6 = 0;
  local_4 = 0x50;
  local_2 = 200;
  FUN_1000c890(&local_8,DAT_1002b370);
  if (*(int *)(DAT_1002bb44 + 0xe4) != 0) {
    FUN_1000cc70(0);
    FUN_1000c2f0();
    FUN_1000c480();
    cVar1 = DAT_1002bb44[0xe0];
    while ((cVar1 != '\0' && (*(int *)(DAT_1002bb44 + 0xe4) != 0))) {
      FUN_1000cc70(0);
      FUN_1000c2f0();
      FUN_1000c480();
      cVar1 = DAT_1002bb44[0xe0];
    }
  }
  FUN_1001d4e0();
  FUN_1000c5c0();
  sStack_e = (ushort)(DAT_1002cc04 == DAT_1002cc08) << 8;
  uStack_10 = 0;
  uStack_c = 0x140;
  uStack_a = 0x100;
  FUN_1000c8e0(&uStack_10,0x140,0);
  FUN_1000c8e0(&uStack_10,0,(uint)(DAT_1002cc04 != DAT_1002cc08) << 8);
  DAT_1002bb44[0x128] = '\x18';
  FUN_1000c370(0x140,0);
  FUN_1000cc80(&DAT_1000c320);
  if (*(int *)(DAT_1002bb44 + 0xe4) == 0) {
    FUN_1000c3d0(&DAT_10029128,(&PTR_s__SRC1_ENGINE_ENGINE1_10022288)[*DAT_1002bb44]);
    return;
  }
  FUN_1000c270();
  return;
}

