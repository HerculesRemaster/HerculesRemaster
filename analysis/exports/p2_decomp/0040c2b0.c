
void thunk_FUN_00409e10(void)

{
  int iVar1;
  
  if (DAT_0096e62c != 0) {
    iVar1 = FUN_00409c30();
    if (iVar1 != 0) {
      mciSendStringA(s_stop_cdaudio_wait_00437394,(LPSTR)0x0,0,(HWND)0x0);
    }
  }
  DAT_0096e62c = 0;
  DAT_0096e620 = 0;
  DAT_0096e61c = 0;
  DAT_0096e628 = 0;
  DAT_0096e624 = 0;
  return;
}

