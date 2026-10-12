
void FUN_10015600(void)

{
  DAT_100721a8 = 1;
  DAT_10072190 = 0;
  *(undefined2 *)(DAT_10070f24 + 0x1e) = 0x80;
  *(undefined2 *)(DAT_10070f24 + 0x20) = 0;
  *(undefined2 *)(DAT_10070f24 + 0x1c) = 0;
  *(undefined1 *)(DAT_10070f24 + 0x11) = 0;
  if (*(char *)(DAT_10070f24 + 0xc) == '\0') {
    *(undefined1 *)(DAT_10070f24 + 0xc) = 1;
  }
  if ('\t' < *(char *)(DAT_10070f24 + 0xc)) {
    *(undefined1 *)(DAT_10070f24 + 0xc) = 9;
  }
  *(undefined1 *)(DAT_10070f24 + 0xd) = 0;
  return;
}

