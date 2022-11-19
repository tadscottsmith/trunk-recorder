/*
 * Project 25 IMBE Encoder/Decoder Fixed-Point implementation
 * Developed by Pavel Yazev E-mail: pyazev@gmail.com
 * Version 1.0 (c) Copyright 2009
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * The software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Boston, MA
 * 02110-1301, USA.
 */
#include "typedef.h"
#include "aux_sub.h"
#include "basic_op.h"
#include "ch_decode.h"
#include "dsp_sub.h"
#include "encode.h"
#include "globals.h"
#include "imbe.h"
#include "imbe_vocoder.h"
#include "sa_decode.h"
#include "sa_enh.h"
#include "typedef.h"
#include "uv_synt.h"
#include "v_synt.h"

#include <string.h>

void imbe_vocoder::decode_init(IMBE_PARAM *imbe_param) {
  v_synt_init();
  uv_synt_init();
  sa_decode_init();

  // Make previous frame for the first frame
  memset((char *)imbe_param, 0, sizeof(IMBE_PARAM));
  imbe_param->fund_freq = 0x0cf6474a;
  imbe_param->num_harms = 9;
  imbe_param->num_bands = 3;
  imbe_param->errorCoset0 = 0;
  imbe_param->errorCoset4 = 0;
  imbe_param->errorRate = 0.0;
  imbe_param->repeatCount = 0;
}

void imbe_vocoder::decode(IMBE_PARAM *imbe_param, Word16 *frame_vector, Word16 *snd) {
  Word16 snd_tmp[FRAME];
  Word16 j;
  bool muteAudio = false;

  // TSS
  // Moved out of decode_frame_vector
  imbe_param->b_vec[0] = (shr(frame_vector[0], 4) & 0xFC) | (shr(frame_vector[7], 1) & 0x3);

  if (imbe_param->repeatCount > 3) {
    fprintf(stderr, "CH_DECODE - Frame Muting. Too many repeats.\n");
    muteAudio = true;
  }

  if (imbe_param->errorRate >= .0875) {
    fprintf(stderr, "CH_DECODE - Frame Muting. Error Rate.\n");
    muteAudio = true;
  }

  decode_frame_vector(imbe_param, frame_vector, prev_frame_vector);
  v_uv_decode(imbe_param);
  sa_decode(imbe_param);
  sa_enh(imbe_param);
  v_synt(imbe_param, snd);
  uv_synt(imbe_param, snd_tmp);

  if (muteAudio) {
    for (j = 0; j < FRAME; j++) {
      snd[j] = 0;
    }
  } else {
    for (j = 0; j < FRAME; j++) {
      snd[j] = add(snd[j] * 4, snd_tmp[j]); // Attempting to increase volume.
    }
  }

  for (int i = 0; i < 8; i++) {
    prev_frame_vector[i] = frame_vector[i];
  }
}
