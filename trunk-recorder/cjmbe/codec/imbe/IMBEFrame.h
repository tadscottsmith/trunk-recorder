
#include <math.h>

class IBMEFrame {

public:
  float LOG_2 = (float)log(2.0);
  int RANDOMIZER_SEED[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  int VECTOR_B0[8] = {0, 1, 2, 3, 4, 5, 141, 142};

  /**
   * Message frame bit index of the voiced/unvoiced decision for all values
   * of L harmonics. On encoding, a voicing decision is made for each of the
   * K frequency bands and recorded in the b1 information vector.
   * On decoding, each of the L harmonics are flagged as voiced or unvoiced
   * according to the harmonic's location within each K frequency band.
   */
  int VOICE_DECISION_INDEX[57] = {0, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95, 96,
                                  96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100, 101, 101, 101, 102, 102, 102, 107, 107, 107, 107,
                                  107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107};

  /**
   * Coefficient offsets for bit lengths 0 - 10:   (2 ^ (bit length -1)) - 0.5
   */
  float COEFFICIENT_OFFSET[11] = {0.0f, 0.5f, 1.5f, 3.5f, 7.5f, 15.5f, 31.5f, 63.5f,
                                  127.5f, 255.5f, 511.5f};

private:
};
