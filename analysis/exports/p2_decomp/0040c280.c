
void FUN_0040c280(short param_1)

{
  uint *puVar1;
  
  puVar1 = (uint *)(&DAT_00a8e890)[(short)(param_1 + -1)];
  if (puVar1 != (uint *)0x0) {
    FUN_00409e60(*puVar1 >> 8,puVar1[1] >> 8);
  }
  return;
}

