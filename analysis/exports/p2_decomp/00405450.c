
int FUN_00405450(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  undefined1 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  uint uStack_42c;
  undefined4 *puStack_428;
  uint uStack_424;
  int iStack_420;
  tagMSG tStack_41c;
  undefined1 auStack_400 [257];
  undefined1 auStack_2ff [767];
  
  iVar5 = 0;
  FUN_00409e10();
  FUN_00410f40(auStack_400,s_e__data__d_etv_00436658,param_1);
  cVar1 = FUN_00405350(auStack_400);
  if (cVar1 == '\0') {
    return 0;
  }
  iVar8 = 0;
  puVar6 = auStack_2ff;
  do {
    FUN_00406810(iVar8,puVar6 + -1,puVar6,puVar6 + 1);
    iVar8 = iVar8 + 1;
    puVar6 = puVar6 + 3;
  } while (iVar8 < 0x100);
  FUN_00403140(auStack_400,DAT_00b9e440,&puStack_428,&uStack_424,&uStack_42c);
  DAT_0091c44c = 1;
  do {
    while( true ) {
      iVar8 = PeekMessageA(&tStack_41c,DAT_00ba0200,0,0,1);
      while (iVar8 != 0) {
        TranslateMessage(&tStack_41c);
        DispatchMessageA(&tStack_41c);
        iVar8 = PeekMessageA(&tStack_41c,DAT_00ba0200,0,0,1);
      }
      if (DAT_0091c44c == -1) {
        DAT_0091c44c = 0;
        PostMessageA(DAT_00ba0200,0x10,0,0);
        goto LAB_004056dd;
      }
      DAT_00ba0230 = 0;
      FUN_00405a50();
      FUN_00406bb0();
      if ((DAT_00ba0238 != '\0') && (DAT_00ba0228 == '\0')) break;
      FUN_00403770();
      WaitMessage();
    }
    uVar3 = -(uint)(DAT_00b9e866 != '\0') & 13000;
    sVar2 = FUN_0040fb10(uVar3,uVar3);
    iVar8 = FUN_004037a0(&iStack_420,(int)sVar2);
    if (iVar8 == 0) break;
    if (iStack_420 != 0) {
      iVar9 = 0;
      iVar5 = 0;
      do {
        FUN_00406780(iVar9,*(undefined1 *)(iStack_420 + iVar5),
                     *(undefined1 *)(iStack_420 + 1 + iVar5),*(undefined1 *)(iStack_420 + 2 + iVar5)
                    );
        iVar5 = iVar5 + 3;
        iVar9 = iVar9 + 1;
      } while (iVar5 < 0x300);
      FUN_004069a0(0);
    }
    if (0 < iVar8) {
      FUN_00406ff0();
      if ((DAT_00ba01f0 != '\0') && (DAT_00ba0210 != (undefined4 *)0x0)) {
        if (DAT_00ba0214 == uStack_424) {
          puVar7 = puStack_428;
          puVar10 = DAT_00ba0210;
          for (uVar3 = uStack_42c * uStack_424 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
            *puVar10 = *puVar7;
            puVar7 = puVar7 + 1;
            puVar10 = puVar10 + 1;
          }
          for (uVar3 = uStack_42c * uStack_424 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
            *(undefined1 *)puVar10 = *(undefined1 *)puVar7;
            puVar7 = (undefined4 *)((int)puVar7 + 1);
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
        }
        else {
          uVar3 = 0;
          if (uStack_42c != 0) {
            do {
              puVar7 = (undefined4 *)(uStack_424 * uVar3 + (int)puStack_428);
              puVar10 = (undefined4 *)(uVar3 * DAT_00ba0214 + (int)DAT_00ba0210);
              for (uVar4 = uStack_424 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
                *puVar10 = *puVar7;
                puVar7 = puVar7 + 1;
                puVar10 = puVar10 + 1;
              }
              uVar3 = uVar3 + 1;
              for (uVar4 = uStack_424 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
                *(undefined1 *)puVar10 = *(undefined1 *)puVar7;
                puVar7 = (undefined4 *)((int)puVar7 + 1);
                puVar10 = (undefined4 *)((int)puVar10 + 1);
              }
            } while (uVar3 < uStack_42c);
          }
        }
        DAT_0043a590 = 1;
      }
      FUN_00407140();
      FUN_00406620();
    }
    if (DAT_00ba0238 == '\x01') {
      DAT_00ba0238 = '\0';
    }
    FUN_00406bb0();
    FUN_00405a50();
    FUN_0040bcf0();
    iVar5 = FUN_00401f60(param_2);
  } while (iVar5 == 0);
LAB_004056dd:
  iVar9 = 0;
  DAT_0091c44c = 0;
  iVar8 = FUN_00403680();
  if (iVar8 != 0) {
    FUN_00406ec0(0);
  }
  puVar6 = auStack_2ff;
  do {
    FUN_00406780(iVar9,puVar6[-1],*puVar6,puVar6[1]);
    iVar9 = iVar9 + 1;
    puVar6 = puVar6 + 3;
  } while (iVar9 < 0x100);
  FUN_004069a0(1);
  return iVar5;
}

