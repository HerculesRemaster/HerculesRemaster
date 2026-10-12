
void FUN_004041a0(void)

{
  ushort uVar1;
  ushort uVar2;
  
  uVar2 = ~DAT_00ba2a42;
  *(undefined2 *)(PTR_DAT_004361bc + 0x60) = 0;
  if (DAT_00ba2a40 == -1) {
    PTR_DAT_004361bc[0x60] = PTR_DAT_004361bc[0x60] | 1;
  }
  else if ((DAT_00ba2a41 & 0xf0) != 0x40) {
    PTR_DAT_004361bc[0x60] = PTR_DAT_004361bc[0x60] | 2;
  }
  if (*(short *)(PTR_DAT_004361bc + 0x60) != 0) {
    uVar2 = 0;
  }
  uVar1 = *(ushort *)(PTR_DAT_004361bc + 0x5c);
  *(ushort *)(PTR_DAT_004361bc + 0x5c) = uVar2;
  *(ushort *)(PTR_DAT_004361bc + 0x5e) =
       (uVar1 ^ *(ushort *)(PTR_DAT_004361bc + 0x5c)) & *(ushort *)(PTR_DAT_004361bc + 0x5c);
  *(undefined2 *)(PTR_DAT_004361bc + 100) = 0;
  *(undefined2 *)(PTR_DAT_004361bc + 0x62) = 0;
  *(undefined2 *)(PTR_DAT_004361bc + 0x66) = 0;
  return;
}

