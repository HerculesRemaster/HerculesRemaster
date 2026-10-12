
void FUN_004104d0(void)

{
  if (DAT_00ba023c != '\0') {
    FUN_00411d10(0xfffffebf);
  }
  DAT_00ba023c = 1;
  FUN_00405e90();
  if (DAT_00ba0200 != (HWND)0x0) {
    SendNotifyMessageA(DAT_00ba0200,0x10,0,0);
  }
  FUN_00411d10(0);
  return;
}

