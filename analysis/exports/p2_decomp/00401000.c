
void FUN_00401000(void)

{
  *(uint *)(PTR_DAT_004361bc + 0x1b14) =
       (uint)(*(int *)(PTR_DAT_004361bc + 0x1b0c) * *(int *)(PTR_DAT_004361bc + 0x1b04)) >> 8;
  *(uint *)(PTR_DAT_004361bc + 0x1b18) =
       (uint)(*(int *)(PTR_DAT_004361bc + 0x1b10) * *(int *)(PTR_DAT_004361bc + 0x1b08)) >> 8;
  if ((*(int *)(PTR_DAT_004361bc + 0x1afc) != -1) &&
     ((uint)*(ushort *)(PTR_DAT_004361bc + 0x1b1c) != *(uint *)(PTR_DAT_004361bc + 0x1b14))) {
    FUN_0040be30(CONCAT22((short)(*(uint *)(PTR_DAT_004361bc + 0x1b14) >> 0x10),
                          *(undefined2 *)(PTR_DAT_004361bc + 0x1afc)),
                 *(undefined2 *)(PTR_DAT_004361bc + 0x1b14),
                 *(undefined2 *)(PTR_DAT_004361bc + 0x1b14));
    *(undefined2 *)(PTR_DAT_004361bc + 0x1b1c) = *(undefined2 *)(PTR_DAT_004361bc + 0x1b14);
  }
  return;
}

