
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl FUN_10020830(int param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  int *piVar6;
  undefined2 *puVar7;
  
  if (param_1 <= DAT_10070ed0) {
    pcVar1 = (char *)(&DAT_10070d5c)[param_1];
    DAT_10070ece = *pcVar1;
    DAT_10070ecf = pcVar1[2];
    _DAT_10070d12 = *(undefined2 *)(pcVar1 + 4);
    DAT_10070d14 = *(undefined2 *)(pcVar1 + 6);
    iVar5 = 0;
    _DAT_10070dac = 0;
    _DAT_10070d6c = *(short *)((&DAT_10070d5c)[param_1] + 10) + 10 + (&DAT_10070d5c)[param_1];
    iVar3 = 0;
    if ('\0' < DAT_10070ece) {
      puVar7 = &DAT_10070e4c;
      piVar6 = (int *)(pcVar1 + 0xc);
      do {
        iVar2 = *piVar6;
        piVar6 = piVar6 + 1;
        iVar3 = iVar3 + iVar2;
        iVar5 = iVar5 + 1;
        *puVar7 = (short)iVar3;
        puVar7 = puVar7 + 1;
      } while (iVar5 < DAT_10070ece);
    }
    puVar4 = &DAT_10070d70;
    DAT_10070d48 = pcVar1 + DAT_10070ece * 4 + 0xc;
    puVar7 = &DAT_10070dae;
    do {
      *puVar4 = 0;
      *puVar7 = 0;
      puVar4 = puVar4 + 1;
      puVar7 = puVar7 + 1;
    } while ((int)puVar4 < 0x10070dac);
  }
  return;
}


