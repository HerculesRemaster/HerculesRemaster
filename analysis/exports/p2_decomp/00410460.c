
/* MessageBox 'Hercules Error' then exit */

void RT_FatalError(LPCSTR param_1)

{
  if (DAT_00ba023c != '\0') {
    FUN_00411d10(0xfffffebf);
  }
  DAT_00ba023c = 1;
  FUN_00405e90();
  if (DAT_00ba020c == '\0') {
    MessageBoxA(DAT_00ba0200,param_1,s_Hercules_Error_0043a598,0x10);
  }
  if (DAT_00ba0200 != (HWND)0x0) {
    SendNotifyMessageA(DAT_00ba0200,0x10,0,0);
  }
  FUN_00411d10(0xffffff85);
  return;
}

