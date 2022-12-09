
#ifndef INCLUDED_RUST_IMBE_DECODER_H
#define INCLUDED_RUST_IMBE_DECODER_H

#include "imbe_decoder.h"

#include <stdint.h>

class rust_imbe_decoder : public imbe_decoder {
public:
  rust_imbe_decoder();
  virtual ~rust_imbe_decoder();

private:
  // NOTE: Single-letter variable names are upper case only; Lower
  //				  case if needed is spelled. e.g. L, ell

  float ER;    // BER Estimate
  int rpt_ctr; // Frame repeat counter

  int bee[58];     // Encoded Spectral Amplitudes
  float M[57][2];  // Enhanced Spectral Amplitudes
  float Mu[57][2]; // Unenhanced Spectral Amplitudes
  int vee[57][2];  // V/UV decisions
  float suv[160];  // Unvoiced samples
  float sv[160];   // Voiced samples
  float log2Mu[58][2];
  float Olduw[256];
  float psi1;
  float phi[57][2];
  uint32_t u[211];

  int Old;
  int New;
  int L;
  int OldL;
  float w0;
  float Oldw0;
  float Luv; // number of unvoiced spectral amplitudes

  float TM = 20480;    // Amplitude threshold of current frame;
  float OldTM = 20480; // Amplitude threshold of previous frame;

  char sym_b[4096];
  char RxData[4096];
  int sym_bp;
  int ErFlag;
};

#endif /* INCLUDED_SOFTWARE_IMBE_DECODER_H */
