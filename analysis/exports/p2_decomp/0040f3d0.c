
void FUN_0040f3d0(int **param_1)

{
  int *piVar1;
  undefined1 uVar2;
  int **ppiVar3;
  ushort uVar4;
  ushort uVar5;
  char cVar6;
  int iVar7;
  int *piStack_158;
  int *piStack_154;
  int **ppiStack_150;
  int *piStack_13c;
  int *local_138;
  int local_134;
  int *local_130;
  undefined4 local_12c;
  uint local_128;
  int local_124;
  ushort local_120;
  ushort uStack_11e;
  int **local_11c;
  int **local_118;
  int local_114;
  uint local_110;
  uint local_10c;
  undefined4 local_108;
  undefined4 *local_104;
  int local_100 [64];
  
  local_134 = 0;
  local_130 = (int *)0x0;
  local_138 = (int *)0x0;
  local_11c = (int **)0x0;
  if ((param_1 < (int **)0x4b0) &&
     (piVar1 = &DAT_00b99908 + (int)param_1 * 4, (&DAT_00b99908)[(int)param_1 * 4] == 0)) {
    ppiStack_150 = (int **)((uint)param_1 & 0xffff);
    piStack_158 = local_100;
    piStack_154 = (int *)s_m__language_sonix_pc__ld_esf_0043a510;
    FUN_00410f40();
    piStack_154 = local_100;
    ppiStack_150 = (int **)&DAT_00435fe4;
    piStack_158 = (int *)0x40f442;
    ppiVar3 = (int **)FS_Open();
    piStack_154 = (int *)0x1;
    piStack_158 = (int *)0x10;
    ppiStack_150 = ppiVar3;
    local_118 = ppiVar3;
    FS_Read(&DAT_00a99838);
    if (DAT_00a99838 != 0x3465345) {
      ppiStack_150 = (int **)s_PC_CreateSFX_1__0043a500;
      piStack_154 = (int *)0x40f477;
      RT_FatalError();
    }
    local_128 = DAT_00a99840 & 0xffff;
    cVar6 = (char)((uint)DAT_00a99844 >> 8);
    local_12c = 0x10001;
    uVar4 = (-(ushort)(cVar6 != '\0') & 0xfff8) + 0x10;
    uVar5 = uVar4 >> 3;
    _local_120 = CONCAT22(uVar4,uVar5);
    local_124 = uVar5 * local_128;
    local_108 = 0;
    local_110 = (-(uint)(param_1 < (int **)0x3e8) & 0xfffffffa) + 8 | 0xe0;
    local_114 = 0x14;
    local_10c = (uint)DAT_00a9983c / ((cVar6 != '\0') + 1);
    local_104 = &local_12c;
    if (DAT_00b9e440 != (int *)0x0) {
      ppiStack_150 = (int **)0x0;
      piStack_158 = &local_114;
      piStack_154 = piVar1;
      (**(code **)(*DAT_00b9e440 + 0xc))(DAT_00b9e440);
    }
    piStack_154 = (int *)*piVar1;
    ppiStack_150 = ppiVar3;
    if (piStack_154 != (int *)0x0) {
      ppiStack_150 = (int **)0x0;
      piStack_158 = (int *)0x40f54e;
      (**(code **)(*piStack_154 + 0x34))();
      piStack_158 = (int *)0x0;
      (**(code **)(*(int *)*piVar1 + 0x44))((int *)*piVar1);
      (**(code **)(*(int *)*piVar1 + 0x40))((int *)*piVar1,0);
      (**(code **)(*(int *)*piVar1 + 0x3c))((int *)*piVar1,0);
      iVar7 = (int)&DAT_00b99847 - ((uint)DAT_00a9983c >> 2);
      FS_Read(iVar7,(uint)DAT_00a9983c >> 2,1,ppiVar3);
      (**(code **)(*(int *)*piVar1 + 0x2c))
                ((int *)*piVar1,0,(uint)DAT_00a9983c / ((DAT_00a99844._1_1_ != '\0') + 1),
                 &piStack_154,&piStack_158,&ppiStack_150,&piStack_13c,0);
      if ((local_134 != 0) && (local_138 != (int *)0x0)) {
        ppiStack_150 = &piStack_13c;
        piStack_154 = DAT_00a9983c;
        piStack_158 = (int *)&DAT_00a99848;
        piStack_13c = (int *)0x0;
        FUN_00402410(iVar7);
        if (DAT_00a99844._1_1_ == '\0') {
          piStack_13c = (int *)0x0;
          if (((uint)local_138 & 0xfffffffe) != 0) {
            do {
              *(undefined2 *)(local_134 + (int)piStack_13c * 2) = (&DAT_00a99848)[(int)piStack_13c];
              piStack_13c = (int *)((int)piStack_13c + 1);
            } while (piStack_13c < (int *)((uint)local_138 >> 1));
          }
        }
        else {
          piStack_13c = (int *)0x0;
          if (local_138 != (int *)0x0) {
            do {
              *(char *)((int)piStack_13c + local_134) =
                   '\x7f' - (char)((ushort)(&DAT_00a99848)[(int)piStack_13c] >> 8);
              piStack_13c = (int *)((int)piStack_13c + 1);
            } while (piStack_13c < local_138);
          }
        }
        ppiStack_150 = local_11c;
        piStack_154 = local_130;
        piStack_158 = local_138;
        (**(code **)(*(int *)*piVar1 + 0x4c))((int *)*piVar1,local_134);
      }
      piStack_13c = (int *)0x1;
      do {
        ppiStack_150 = (int **)(&DAT_00b99908 + (int)(piStack_13c + (int)param_1));
        piStack_154 = (int *)*piVar1;
        piStack_158 = DAT_00b9e440;
        (**(code **)(*DAT_00b9e440 + 0x14))();
        if ((&DAT_00b99908)[(int)(piStack_13c + (int)param_1)] == 0) {
          ppiStack_150 = param_1;
          piStack_154 = (int *)0x40f6d9;
          FUN_0040f710();
          ppiStack_150 = local_118;
          break;
        }
        piStack_13c = (int *)((int)piStack_13c + 1);
        ppiStack_150 = local_118;
      } while ((int)piStack_13c < 4);
    }
    uVar2 = (undefined1)DAT_00a99844;
    *(undefined2 *)((int)&DAT_00a98ed8 + (int)param_1 * 2) = 0;
    (&DAT_00a98a18)[(int)param_1] = uVar2;
    piStack_154 = (int *)0x40f6fc;
    FS_Close();
  }
  return;
}

