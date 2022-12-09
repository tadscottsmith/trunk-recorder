

#include "rust_imbe_decoder.h"

rust_imbe_decoder::rust_imbe_decoder() {
  int i, j;
  // initialize
  ER = 0;
  rpt_ctr = 0;
  OldL = 0;
  L = 9;
  Old = 1;
  New = 0;
  psi1 = 0.0;
  for (i = 0; i < 58; i++) {
    for (j = 0; j < 2; j++) {
      log2Mu[i][j] = 0.0;
    }
  }
  for (i = 0; i < 57; i++) {
    for (j = 0; j < 2; j++) {
      phi[i][j] = 0.0;
    }
  }
  for (i = 0; i < 256; i++) {
    Olduw[i] = 0.0;
  }
  u[0] = 3147;
  for (i = 1; i < 211; i++) {
    u[i] = next_u(u[i - 1]);
  }
}
