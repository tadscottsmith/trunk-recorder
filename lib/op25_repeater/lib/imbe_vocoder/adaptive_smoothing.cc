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
#include "basic_op.h"
#include "imbe.h"
#include "qnt_sub.h"
#include "aux_sub.h"
#include "math_sub.h"
#include <cmath>
#include "adaptive_smoothing.h"

#include <iostream>


void adaptive_smoothing(IMBE_PARAM *imbe_param)
{

	float adaptiveThreshold;
	float amplitudeMeasure = 0;

	if((imbe_param->errorRate <= .005) && (imbe_param->errorTotal <=4)){
		adaptiveThreshold = FLT_MAX;
	}
	else if ((imbe_param->errorRate <= .0125) && (imbe_param->errorCoset4 == 0)){
		adaptiveThreshold = (45.255 * pow(imbe_param->spectralEnergy,.375)) / exp(277.26 * imbe_param->errorRate);
	}
	else{
		adaptiveThreshold = 1.414 * pow(imbe_param->spectralEnergy, .375);
	}

	for(int i = 0; i<imbe_param->num_harms;i++){
		amplitudeMeasure += imbe_param->sa[i];
		if(imbe_param->sa[i] > adaptiveThreshold){
			imbe_param->v_uv_dsn[i] = 1;
		}
	}

	if ((imbe_param->errorRate <= .005) && (imbe_param->errorTotal <= 6)){
		imbe_param->amplitudeThreshold = 20480;
	}
	else{
		imbe_param->amplitudeThreshold = 6000 - (300 * imbe_param->errorTotal) + imbe_param->amplitudeThreshold;
	}

	if(amplitudeMeasure > imbe_param->amplitudeThreshold)
	{
		float scaleFactor = imbe_param->amplitudeThreshold / amplitudeMeasure;
		for(int i = 0; i<imbe_param->num_harms;i++){
			imbe_param->sa[i] = imbe_param->sa[i] * scaleFactor;
		}
	}
}
