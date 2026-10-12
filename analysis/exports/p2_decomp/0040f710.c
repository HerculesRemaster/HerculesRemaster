
void FUN_0040f710(uint param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  
  if ((param_1 < 0x4b0) && ((DAT_00b9e450 != '\0' || (param_1 != 0x338)))) {
    piVar2 = &DAT_00b99908 + param_1 * 4;
    iVar3 = 4;
    do {
      piVar1 = (int *)*piVar2;
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 0x48))(piVar1);
        (**(code **)(*(int *)*piVar2 + 8))((int *)*piVar2);
        *piVar2 = 0;
      }
      piVar2 = piVar2 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  return;
}

