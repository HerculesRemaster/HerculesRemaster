
void FUN_00401a00(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  if (DAT_0043f738 == 0) {
    DAT_0043f738 = 1;
    FUN_004010e0();
    *(undefined4 *)(PTR_DAT_004361bc + 0x1b24) = 1;
  }
  FUN_0040c100();
  FUN_0040c220(0x1000);
  FUN_0040be30(PTR_DAT_004361bc + 0x1a0,1,1);
  FUN_0040a340(1);
  FUN_0040c2c0(4);
  FUN_0040be30(0,0);
  FUN_0040be30();
  uVar1 = FUN_0040c2d0(param_1);
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b20) = uVar1;
  if (param_1 == 0) {
    uVar1 = 0x1000;
  }
  else {
    uVar1 = 5;
  }
  FUN_0040c220(uVar1);
  FUN_0040be30();
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b28) = 0x69;
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b2c) = 0x69;
  *(undefined4 *)(PTR_DAT_004361bc + 0x1b30) = 1;
  FUN_0040c200(0x69,0x69);
  *(undefined4 *)(PTR_DAT_004361bc + 0x1afc) = 0xffffffff;
  iVar3 = 0;
  do {
    iVar2 = iVar3 + 3;
    PTR_DAT_004361bc[iVar3 + 0x2f0] = 0xff;
    PTR_DAT_004361bc[iVar3 + 0x2f1] = 0xff;
    iVar3 = iVar2;
  } while (iVar2 < 0x1800);
  iVar3 = 0x2b0;
  do {
    iVar2 = iVar3 + 2;
    *(undefined2 *)(PTR_DAT_004361bc + iVar3) = 0xffff;
    iVar3 = iVar2;
  } while (iVar2 < 0x2f0);
  return;
}

