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
#include "basic_op.h"
#include "ch_decode.h"
#include "aux_sub.h"
#include <iostream>
#include <math.h>

static float fundamentalFrequency[208] ={
0.318135965, .310280756, .302804111, .295679309, .288882083, .282390351, .276183970, .270244529, .264555171, .259100425, .253866073,
0.248839022, .244007196, .239359440, .234885432, .230575608, .226421092, .222413639, .218545576, .214809754, .211199506, .207708605,
0.204331230, .201061930, .197895600, .194827451, .191852986, .188967979, .186168454, .183450666, .180811088, .178246392, .175753435,
0.173329250, .170971029, .168676116, .166441995, .164266283, .162146718, .160081154, .158067555, .156103983, .154188596, .152319644,
0.150495456, .148714445, .146975095, .145275961, .143615664, .141992888, .140406376, .138854924, .137337384, .135852655, .134399686,
0.132977467, .131585033, .130221457, .128885852, .127577367, .126295182, .125038514, .123806607, .122598738, .121414209, .120252350,
0.119112518, .117994090, .116896471, .115819084, .114761375, .113722811, .112702875, .111701072, .110716922, .109749962, .108799746,
0.107865842, .106947835, .106045322, .105157913, .104285233, .103426919, .102582617, .101751989, .100934704, .100130443, .099338898,
0.098559770, .097792767, .097037611, .096294028, .095561754, .094840533, .094130117, .093430265, .092740743, .092061323, .091391786,
0.090731918, .090081510, .089440360, .088808273, .088185057, .087570527, .086964503, .086366808, .085777274, .085195733, .084622024,
0.084055991, .083497479, .082946341, .082402430, .081865607, .081335732, .080812673, .080296298, .079786480, .079283095, .078786023,
0.078295144, .077810344, .077331511, .076858536, .076391311, .075929732, .075473697, .075023108, .074577867, .074137880, .073703053,
0.073273298, .072848525, .072428649, .072013585, .071603251, .071197567, .070796454, .070399835, .070007636, .069619782, .069236202,
0.068856825, .068481584, .068110410, .067743238, .067380003, .067020643, .066665096, .066313301, .065965200, .065620734, .065279847,
0.064942484, .064608589, .064278111, .063950995, .063627193, .063306653, .062989326, .062675165, .062364122, .062056151, .061751207,
0.061449245, .061150222, .060854095, .060560822, .060270363, .059982676, .059697723, .059415464, .059135862, .058858879, .058584478,
0.058312625, .058043282, .057776417, .057511994, .057249980, .056990343, .056733050, .056478070, .056225372, .055974925, .055726699,
0.055480665, .055236794, .054995057, .054755428, .054517877, .054282378, .054048906, .053817433, .053587934, .053360385, .053134759,
0.052911034, .052689185, .052469188, .052251021, .052034661, .051820085, .051607272, .051396199, .051186846, .050979191};

static int spectralAmplitudes[208] ={
09, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18,
19,19,19,19,20,20,20,20,21,21,21,21,22,22,22,22,23,23,23,23,24,24,24,24,24,24,24,24,25,25,25,25,26,26,26,26,27,27,27,27,28,28,28,28,
29,29,29,29,30,30,30,30,31,31,31,31,32,32,32,32,33,33,33,33,34,34,34,34,35,35,35,35,36,36,36,36,37,37,37,37,37,37,37,37,38,38,38,38,
39,39,39,39,40,40,40,40,41,41,41,41,42,42,42,42,43,43,43,43,44,44,44,44,45,45,45,45,46,46,46,46,47,47,47,47,48,48,48,48,49,49,49,49,
49,49,49,49,50,50,50,50,51,51,51,51,52,52,52,52,53,53,53,53,54,54,54,54,55,55,55,55,56,56,56,56};

static int voicingDecisions[208] = {
 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
07, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,10,10,10,10,
10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12};


void decode_frame_vector(IMBE_PARAM *imbe_param, Word16 *frame_vector, Word16 *previous_frame_vector)
{
	Word16 bit_stream[BIT_STREAM_LEN];
	Word16 i, vec_num, tmp, tmp1, tmp2, bit_thr, shift;
	Word16 *b_ptr, *ba_ptr, index0;
	Word32 L_tmp;

	imbe_param->b_vec[0] = (shr(frame_vector[0], 4) & 0xFC) | (shr(frame_vector[7], 1) & 0x3);

	// 7.7 FRAME REPEATS
	// INVALID PITCH ESTIMATE
	if (imbe_param->b_vec[0] < 0 || imbe_param->b_vec[0] > 207){
		imbe_param->repeatCount++;
		return; // If we return here IMBE parameters from previous frame will be used (frame repeating)		
	}

	// 7.7 FRAME REPEATS
	// ALGORITHM 97
	if (imbe_param->errorCoset0 >= 2){
		imbe_param->repeatCount++;
		return; // If we return here IMBE parameters from previous frame will be used (frame repeating)		
	}

	// 7.7 FRAME REPEATS
	// ALGORITHM 98
	if (imbe_param->errorTotal >= (10 + 40 * imbe_param->errorRate)){
		imbe_param->repeatCount++;
		return; // If we return here IMBE parameters from previous frame will be used (frame repeating)		
	}

	// 7.7 FRAME MUTING
	// SEVER BIT ERRORS
	if (imbe_param->errorRate >= .0875) {
		imbe_param->muteAudio = true;
		return; // If we return here IMBE parameters from previous frame will be used and frame will be muted.	
	}

	// If we make it hear, we have a good frame. Clean up the variables.
	imbe_param->repeatCount = 0;
	imbe_param->muteAudio = false;

	tmp = ((imbe_param->b_vec[0] & 0xFF) << 1) + 0x4F;                                      // Convert b_vec[0] to unsigned Q15.1 format and add 39.5

	// Calculate fundamental frequency with higher precession
	shift = norm_s(tmp);
	tmp1  = tmp << shift;

	tmp2 = div_s(0x4000, tmp1);
	imbe_param->fund_freq = L_shr(L_deposit_h(tmp2), 11 - shift);    

	L_tmp = L_sub(0x40000000, L_mult(tmp1, tmp2));
	tmp2  = div_s(extract_l(L_shr(L_tmp, 2)), tmp1);
	L_tmp = L_shr(L_deposit_l(tmp2), 11 - shift - 2);
	imbe_param->fund_freq = L_add(imbe_param->fund_freq, L_tmp);

	// 6.1 FUNDAMENTAL FREQUENCY ENCODING AND DECODING
	// ALGORITHM 46
	float fundFrequency = fundamentalFrequency[imbe_param->b_vec[0]];
	imbe_param->fund_freq = fundFrequency;
	fprintf(stderr, "Made it past the FF. Mine: %f\t Theirs: %d\tB0: %d\n", fundFrequency,L_add(imbe_param->fund_freq, L_tmp),imbe_param->b_vec[0]);

	// 6.1 FUNDAMENTAL FREQUENCY ENCODING AND DECODING
	// ALGORITHM 47
	int spectAmplitudes = spectralAmplitudes[imbe_param->b_vec[0]];
	imbe_param->num_harms = spectAmplitudes;
	fprintf(stderr, "Made it past the num harms. %d\n", numHarms);

	int voiceDecisions = voicingDecisions[imbe_param->b_vec[0]];
	imbe_param->num_bands = voiceDecisions;

	fprintf(stderr, "Made it past NUM BANDS. %d\n", imbe_param->num_bands);
	
	// Convert input vector (from b_3 to b_L+1) to bit stream
	bit_stream[0] = (frame_vector[0] & 0x4)?1:0;
	bit_stream[1] = (frame_vector[0] & 0x2)?1:0;
	bit_stream[2] = (frame_vector[0] & 0x1)?1:0;

	bit_stream[BIT_STREAM_LEN - 3] = (frame_vector[7] & 0x40)?1:0;
	bit_stream[BIT_STREAM_LEN - 2] = (frame_vector[7] & 0x20)?1:0;
	bit_stream[BIT_STREAM_LEN - 1] = (frame_vector[7] & 0x10)?1:0;


	index0 = 3 + 3 * 12 - 1;
	for(vec_num = 3; vec_num >= 1;  vec_num--)
	{	
		tmp = frame_vector[vec_num];
		for(i = 0; i < 12; i++)
		{
			bit_stream[index0] = (tmp & 0x1)?1:0;
			tmp >>= 1;
			index0--;
		}
	}
	
	index0 = 3 + 3 * 12 + 3 * 11 - 1;
	for(vec_num = 6; vec_num >= 4;  vec_num--)
	{	
		tmp = frame_vector[vec_num];
		for(i = 0; i < 11; i++)
		{
			bit_stream[index0] = (tmp & 0x1)?1:0;
			tmp >>= 1;
			index0--;
		}
	}

	// Rebuild b1
	index0 = 3 + 3 * 12;
	tmp = 0;
	for(i = 0; i < imbe_param->num_bands; i++)
		tmp = (tmp << 1) | bit_stream[index0++];		

	imbe_param->b_vec[1] = tmp;

	// Rebuild b2
	tmp = 0;
	tmp |= bit_stream[index0++] << 1;			
	tmp |= bit_stream[index0++];				
	imbe_param->b_vec[2] = (frame_vector[0] & 0x38) | (tmp << 1) | (shr(frame_vector[7], 3) & 0x01);

	// Shift the rest of sequence
	tmp = imbe_param->num_bands + 2;            // shift
	for(; index0 < BIT_STREAM_LEN; index0++)
		bit_stream[index0 - tmp] = bit_stream[index0];

    // Priority ReScanning
	b_ptr = &imbe_param->b_vec[3];
	ba_ptr = imbe_param->bit_alloc;
	for(i = 0; i < B_NUM; i++)
		ba_ptr[i] = b_ptr[i] = 0;


	// Unpack bit allocation table's item
	get_bit_allocation(imbe_param->num_harms, imbe_param->bit_alloc);

	index0 = 0;
	bit_thr = (imbe_param->num_harms == 0xb)?9:ba_ptr[0];

	while(index0 < BIT_STREAM_LEN - imbe_param->num_bands - 2)
	{	
		for(i = 0; i < imbe_param->num_harms - 1; i++)
			if(bit_thr && bit_thr <= ba_ptr[i])
				b_ptr[i] = (b_ptr[i] << 1) | bit_stream[index0++];		
		bit_thr--;
		if (bit_thr < 0) {
			std::cout << "Weird Error - imploder malfunction" << std::endl;
			break;
		}
	}

	// Synchronization Bit Decoding
	imbe_param->b_vec[imbe_param->num_harms + 2] = frame_vector[7] & 1;
}


void v_uv_decode(IMBE_PARAM *imbe_param)
{
	Word16 num_harms;
	Word16 num_bands;
	Word16 vu_vec, *p_v_uv_dsn, mask, i, uv_cnt;

	num_harms = imbe_param->num_harms;
    num_bands = imbe_param->num_bands;
	vu_vec    = imbe_param->b_vec[1];

	p_v_uv_dsn = imbe_param->v_uv_dsn;

	mask = 1 << (num_bands - 1); 

	v_zap(p_v_uv_dsn, NUM_HARMS_MAX);

	i = 0; uv_cnt = 0;
	while(num_harms--)
	{
		if(vu_vec & mask)
			*p_v_uv_dsn++ = 1;
		else
		{
			*p_v_uv_dsn++ = 0;
			uv_cnt++;
		}

		if(++i == 3)
		{
			if(num_bands > 1)
			{
				num_bands--;
				mask >>= 1;
			}
			i = 0;
		}
	}
	imbe_param->l_uv = uv_cnt;
}
