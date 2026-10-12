
void FUN_00406440(int param_1,int param_2,undefined4 param_3)

{
  char cVar1;
  uint uVar2;
  char *pcVar3;
  HDC pHStack_104;
  char acStack_100 [256];
  
  DAT_0096e408 = DAT_0096e408 + 1;
  FUN_00410f40(acStack_100,s___c___s_00436a10,(DAT_0096e408 & 7) + 0x30,param_3);
  if (DAT_00ba020c == '\0') {
    pHStack_104 = GetDC((HWND)0x0);
    uVar2 = 0xffffffff;
    pcVar3 = acStack_100;
    do {
      if (uVar2 == 0) break;
      uVar2 = uVar2 - 1;
      cVar1 = *pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (cVar1 != '\0');
    TextOutA(pHStack_104,param_1,param_2,acStack_100,~uVar2 - 1);
    ReleaseDC(DAT_00ba0200,pHStack_104);
  }
  else {
    if (DAT_0096e3ec != (int *)0x0) {
      (**(code **)(*DAT_0096e3ec + 0x44))(DAT_0096e3ec,&pHStack_104);
    }
    uVar2 = 0xffffffff;
    pcVar3 = acStack_100;
    do {
      if (uVar2 == 0) break;
      uVar2 = uVar2 - 1;
      cVar1 = *pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (cVar1 != '\0');
    TextOutA(pHStack_104,param_1,param_2,acStack_100,~uVar2 - 1);
    if (DAT_0096e3ec != (int *)0x0) {
      (**(code **)(*DAT_0096e3ec + 0x68))(DAT_0096e3ec,pHStack_104);
      return;
    }
  }
  return;
}

