
void FUN_00404470(int param_1)

{
  if (param_1 != 0) {
    param_1 = param_1 - (int)DAT_00ba29e0;
    DAT_00ba29e0 = (uint *)((int)DAT_00ba29e0 + param_1);
    DAT_00ba29e4 = DAT_00ba29e4 - param_1;
  }
  *DAT_00ba29e0 =
       ((DAT_00ba29e4 - 8U >> 2) - (DAT_00ba29d0 - (int)DAT_00ba29c0 >> 2)) + DAT_00ba29ec |
       0x80000000;
  DAT_00ba29c0 = DAT_00ba29e0;
  DAT_00ba29d0 = (int)DAT_00ba29e0 + DAT_00ba29e4 + -8;
  FUN_00404830(0);
  FUN_004063c0(s_ReInitheap_Heap_d___0x_x_size__d_00436068,0,DAT_00ba29e0,DAT_00ba29e4);
  return;
}

