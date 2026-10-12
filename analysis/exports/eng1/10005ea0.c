
void FUN_10005ea0(void)

{
  int iVar1;
  uint local_8;
  
  if ((*(uint *)(DAT_10070f24 + 0x30) & 0x1000000) != 0) {
    *(undefined1 *)(DAT_10070f24 + 6) = 3;
  }
  if (*(char *)(DAT_10070f24 + 6) == '\x03') {
    DAT_1006eee8 = 0x1518;
  }
  else if (((*(uint *)(DAT_10070f24 + 0x30) & 0x8000000) != 0) ||
          (*(char *)(DAT_10070f24 + 6) == '\x01')) {
    local_8 = (uint)*(byte *)(DAT_10070f24 + 8);
    if ((*(uint *)(DAT_10070f24 + 0x30) & 0x2000000) != 0) {
      local_8 = 0;
    }
    FUN_1002c3c0(&DAT_1006eeec,0x1800,0);
    FUN_1002c390(&DAT_1006eef4,(&PTR_s_M__DEMO_DEMO_DAT_1004d938)[local_8],DAT_1006eeec,0x1800);
    *(undefined1 *)(DAT_10070f24 + 1) = *DAT_1006eeec;
    *(undefined1 *)(DAT_10070f24 + 2) = *(undefined1 *)(DAT_10070f24 + 1);
    *(uint *)(DAT_10070f24 + 0x30) =
         *(uint *)(DAT_10070f24 + 0x30) | (uint)*(ushort *)(DAT_1006eeec + 2);
    *(undefined1 *)(DAT_10070f24 + 0xe) = DAT_1006eeec[4];
    iVar1 = DAT_10070f24;
    *(undefined4 *)(DAT_10070f24 + 0x6c) = DAT_1004d958;
    *(undefined4 *)(iVar1 + 0x70) = DAT_1004d95c;
    *(undefined4 *)(iVar1 + 0x74) = DAT_1004d960;
    *(undefined4 *)(iVar1 + 0x78) = DAT_1004d964;
    *(undefined1 *)(DAT_10070f24 + 0xc) = 3;
    DAT_1006eee8 = 3;
    DAT_1006eee4 = 2;
  }
  return;
}


