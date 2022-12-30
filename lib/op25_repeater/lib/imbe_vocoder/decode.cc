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
#include "globals.h"
#include "imbe.h"
#include "ch_decode.h"
#include "sa_decode.h"
#include "sa_enh.h"
#include "adaptive_smoothing.h"
#include "v_synt.h"
#include "uv_synt.h"
#include "basic_op.h"
#include "aux_sub.h"
#include "encode.h"
#include "dsp_sub.h"
#include "imbe_vocoder.h"

#include <string.h>

// Added TSS
#include <stdio.h>
#include <cstdlib>


void imbe_vocoder::decode_init(IMBE_PARAM *imbe_param)
{
	v_synt_init();
	uv_synt_init();
	sa_decode_init();

	// Make previous frame for the first frame
	memset((char *)imbe_param, 0, sizeof(IMBE_PARAM));
	imbe_param->fund_freq = .00938;
	imbe_param->num_harms = 30;
	imbe_param->num_bands = 10;
	imbe_param->errorCoset0 = 0;
	imbe_param->errorCoset4 = 0;
	imbe_param->errorRate = 0.0;
	imbe_param->repeatCount = 0;
	imbe_param->muteAudio = false;
	imbe_param->spectralEnergy = 75000.0;
	imbe_param->amplitudeThreshold = 20480;
}


void imbe_vocoder::decode(IMBE_PARAM *imbe_param, Word16 *frame_vector, Word16 *snd)
{
	Word16 snd_tmp[FRAME];
	Word16 j;

	decode_frame_vector(imbe_param, frame_vector);
	v_uv_decode(imbe_param);
	sa_decode(imbe_param);
	sa_enh(imbe_param);
	adaptive_smoothing(imbe_param);
	uv_synt(imbe_param, snd_tmp);
	v_synt(imbe_param, snd);

	if (imbe_param->repeatCount > 3) {
			imbe_param->muteAudio = true;
		}

	if (imbe_param->muteAudio) {
		for (j = 0; j < FRAME; j++) {
			snd[j] = 0;
		}
	} else {
		for (j = 0; j < FRAME; j++) {
			snd[j] = add(snd[j], snd_tmp[j]);
			int tmp = snd[j];
			if(snd[j] > 31125){ // ~ .95 * 32768
				snd[j] = 31125;
				fprintf(stderr, "Old: %d\tNew: %d\n", tmp, snd[j]);
			}
			if(snd[j] < -31125){
				snd[j] = -31125;
				fprintf(stderr, "Old: %d\tNew: %d\n", tmp, snd[j]);
			}
				
		}
	}
}
