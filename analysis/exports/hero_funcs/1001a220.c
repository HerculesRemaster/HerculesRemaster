
void FUN_1001a220(void)

{
  if (*(short *)((int)DAT_10072018 + 0x2e) == 0) {
    FUN_10027a50(DAT_10072018);
    FUN_10027a50(DAT_10072164);
    *(undefined2 *)((int)DAT_10072018 + 0x2e) = 0x4006;
    *(undefined2 *)((int)DAT_10072164 + 0x2e) = 0x4006;
  }
  FUN_1001a2b0(DAT_10072018);
  FUN_1001a2b0(DAT_10072164);
  return;
}

