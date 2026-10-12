
int __cdecl FUN_10019970(int param_1)

{
  char cVar1;
  undefined4 *puVar2;
  bool bVar3;
  int iVar4;
  undefined3 extraout_var;
  int *piVar5;
  short local_c [6];
  
  cVar1 = *DAT_10070f24;
  if (((cVar1 == '\x01') || (cVar1 == '\x05')) || (cVar1 == '\x03')) {
    iVar4 = FUN_1002a880(DAT_1007217c,local_c,1);
    if ((iVar4 != 0) && (iVar4 = 0, 0 < DAT_10071a20)) {
      piVar5 = &DAT_10071a24;
      do {
        puVar2 = (undefined4 *)*piVar5;
        bVar3 = FUN_1002a810(*puVar2,puVar2[1] + param_1 * 0x10000,puVar2[2],local_c);
        if (CONCAT31(extraout_var,bVar3) != 0) {
          return iVar4 + 1;
        }
        iVar4 = iVar4 + 1;
        piVar5 = piVar5 + 1;
      } while (iVar4 < DAT_10071a20);
      return 0;
    }
  }
  return 0;
}

