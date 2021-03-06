/*****************************************************************************
 * hmr_sse42_functions_pixel.c : homerHEVC encoding library
/*****************************************************************************
 * Copyright (C) 2014 homerHEVC project
 *
 * Juan Casal <jcasal.homer@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/


#include <math.h>

#include "hmr_os_primitives.h"
#include "hmr_sse42_primitives.h"
#include "hmr_sse42_macros.h"
#include "hmr_sse42_functions.h"

//#define EXTRA_OPTIMIZATION	1


uint32_t sse_aligned_sad_4x4(uint8_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride)
{
	uint32_t sad = 0;

	__m128_u32	_128u32_zero = sse_128_zero_vector();

	__m128_u8	_128_u8_src = sse128_unpacklo_u64 (sse128_unpacklo_u32 (sse_128_loadlo_vector64(src),  sse_128_loadlo_vector64(src+src_stride)), sse128_unpacklo_u32 (sse_128_loadlo_vector64(src+2*src_stride),  sse_128_loadlo_vector64(src+3*src_stride)));
	__m128_u8	_128_u8_pred = sse128_packs_i16_u8(sse128_unpacklo_u64(sse_128_load_vector_u(pred),sse_128_load_vector_u(pred+pred_stride)), sse128_unpacklo_u64(sse_128_load_vector_u(pred+2*pred_stride),sse_128_load_vector_u(pred+3*pred_stride)));
	__m128_u8	_128_u8_sad = sse_128_sad_u8(_128_u8_src, _128_u8_pred);
	__m128_u32	_128u32_result = sse_128_add_i64(sse128_unpacklo_u64(_128_u8_sad, _128u32_zero), sse128_unpackhi_u64(_128_u8_sad, _128u32_zero));

	return sad = sse_128_get_data_u32(_128u32_result,0);//+sse_128_get_data_u32(_128u32_result,2);

}


uint32_t sse_aligned_sad_8x8(uint8_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride)
{
	uint32_t sad = 0;
	__m128_u32	_128u32_result = sse_128_zero_vector();

	CALC_ALIGNED_SAD_2x8(_128u32_result, src, src+src_stride, pred, pred+pred_stride)
	CALC_ALIGNED_SAD_2x8(_128u32_result, src+2*src_stride, src+3*src_stride, pred+2*pred_stride, pred+3*pred_stride)
	CALC_ALIGNED_SAD_2x8(_128u32_result, src+4*src_stride, src+5*src_stride, pred+4*pred_stride, pred+5*pred_stride)
	CALC_ALIGNED_SAD_2x8(_128u32_result, src+6*src_stride, src+7*src_stride, pred+6*pred_stride, pred+7*pred_stride)

	return sad = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,2);
}


uint32_t sse_aligned_sad_16x16(uint8_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride)
{
	uint32_t sad = 0;
	__m128_u32	_128u32_result = sse_128_zero_vector();

	uint8_t *psrc = src;
	int16_t *ppred = pred;

	CALC_ALIGNED_SAD_16(_128u32_result, psrc, ppred)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+1*src_stride, ppred+1*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+2*src_stride, ppred+2*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+3*src_stride, ppred+3*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+4*src_stride, ppred+4*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+5*src_stride, ppred+5*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+6*src_stride, ppred+6*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+7*src_stride, ppred+7*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+8*src_stride, ppred+8*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+9*src_stride, ppred+9*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+10*src_stride, ppred+10*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+11*src_stride, ppred+11*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+12*src_stride, ppred+12*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+13*src_stride, ppred+13*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+14*src_stride, ppred+14*pred_stride)
	CALC_ALIGNED_SAD_16(_128u32_result, psrc+15*src_stride, ppred+15*pred_stride)

	return sad = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,2);
}


uint32_t sse_aligned_sad_32x32(uint8_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride)
{
	uint32_t sad = 0;
	__m128_u32	_128u32_result = sse_128_zero_vector();

	CALC_ALIGNED_SAD_32(_128u32_result, src, pred)
	CALC_ALIGNED_SAD_32(_128u32_result, src+1*src_stride, pred+1*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+2*src_stride, pred+2*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+3*src_stride, pred+3*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+4*src_stride, pred+4*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+5*src_stride, pred+5*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+6*src_stride, pred+6*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+7*src_stride, pred+7*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+8*src_stride, pred+8*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+9*src_stride, pred+9*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+10*src_stride, pred+10*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+11*src_stride, pred+11*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+12*src_stride, pred+12*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+13*src_stride, pred+13*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+14*src_stride, pred+14*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+15*src_stride, pred+15*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+16*src_stride, pred+16*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+17*src_stride, pred+17*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+18*src_stride, pred+18*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+19*src_stride, pred+19*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+20*src_stride, pred+20*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+21*src_stride, pred+21*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+22*src_stride, pred+22*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+23*src_stride, pred+23*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+24*src_stride, pred+24*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+25*src_stride, pred+25*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+26*src_stride, pred+26*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+27*src_stride, pred+27*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+28*src_stride, pred+28*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+29*src_stride, pred+29*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+30*src_stride, pred+30*pred_stride)
	CALC_ALIGNED_SAD_32(_128u32_result, src+31*src_stride, pred+31*pred_stride)
	return sad = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,2);
}


uint32_t sse_aligned_sad_64x64(uint8_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride)
{
	int i;
	uint32_t sad = 0;
	__m128_u32	_128u32_result = sse_128_zero_vector();

	uint8_t *psrc = src;
	int16_t *ppred = pred;



#ifndef EXTRA_OPTIMIZATION
	for(i=0;i<16;i++)
	{
		CALC_ALIGNED_SAD_64(_128u32_result, psrc, ppred)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+src_stride, ppred+pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+2*src_stride, ppred+2*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+3*src_stride, ppred+3*pred_stride)

		psrc+=4*src_stride;
		ppred+=4*pred_stride;
	}
#else //EXTRA_OPTIMIZATION
		CALC_ALIGNED_SAD_64(_128u32_result, psrc, ppred)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+src_stride, ppred+pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+2*src_stride, ppred+2*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+3*src_stride, ppred+3*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+4*src_stride, ppred+4*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+5*src_stride, ppred+5*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+6*src_stride, ppred+6*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+7*src_stride, ppred+7*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+8*src_stride, ppred+8*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+9*src_stride, ppred+9*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+10*src_stride, ppred+10*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+11*src_stride, ppred+11*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+12*src_stride, ppred+12*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+13*src_stride, ppred+13*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+14*src_stride, ppred+14*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+15*src_stride, ppred+15*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+16*src_stride, ppred+16*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+17*src_stride, ppred+17*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+18*src_stride, ppred+18*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+19*src_stride, ppred+19*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+20*src_stride, ppred+20*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+21*src_stride, ppred+21*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+22*src_stride, ppred+22*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+23*src_stride, ppred+23*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+24*src_stride, ppred+24*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+25*src_stride, ppred+25*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+26*src_stride, ppred+26*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+27*src_stride, ppred+27*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+28*src_stride, ppred+28*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+29*src_stride, ppred+29*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+30*src_stride, ppred+30*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+31*src_stride, ppred+31*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+32*src_stride, ppred+32*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+33*src_stride, ppred+33*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+34*src_stride, ppred+34*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+35*src_stride, ppred+35*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+36*src_stride, ppred+36*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+37*src_stride, ppred+37*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+38*src_stride, ppred+38*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+39*src_stride, ppred+39*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+40*src_stride, ppred+40*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+41*src_stride, ppred+41*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+42*src_stride, ppred+42*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+43*src_stride, ppred+43*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+44*src_stride, ppred+44*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+45*src_stride, ppred+45*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+46*src_stride, ppred+46*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+47*src_stride, ppred+47*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+48*src_stride, ppred+48*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+49*src_stride, ppred+49*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+50*src_stride, ppred+50*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+51*src_stride, ppred+51*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+52*src_stride, ppred+52*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+53*src_stride, ppred+53*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+54*src_stride, ppred+54*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+55*src_stride, ppred+55*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+56*src_stride, ppred+56*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+57*src_stride, ppred+57*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+58*src_stride, ppred+58*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+59*src_stride, ppred+59*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+60*src_stride, ppred+60*pred_stride)
		CALC_ALIGNED_SAD_64(_128u32_result, psrc+61*src_stride, ppred+61*pred_stride)
#endif
	return sad = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,2);

}


uint32_t sse_aligned_sad(uint8_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride, int size)
{
	if(size==4)
		return sse_aligned_sad_4x4(src, src_stride, pred, pred_stride);
	else if(size==8)
		return sse_aligned_sad_8x8(src, src_stride, pred, pred_stride);
	else if(size==16)
		return sse_aligned_sad_16x16(src, src_stride, pred, pred_stride);
	else if(size==32)
		return sse_aligned_sad_32x32(src, src_stride, pred, pred_stride);
	else// if(size==64)
		return sse_aligned_sad_64x64(src, src_stride, pred, pred_stride);
}



//---------------------------------------------ssd ------------------------------------------------------------------


uint32_t sse_ssd_nxn_16x16(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride, uint32_t size)
{
	uint32_t i,j,n;
	uint32_t ssd = 0;
	__m128_u8	_128_aux;
	__m128_		_128_zero = sse_128_zero_vector();
	__m128_u16	_128u32_result = sse_128_zero_vector();


	uint8_t *psrc = src;
	uint8_t *ppred = pred;

	src_stride-=size;
	pred_stride-=size;

	n = size>>4;
	for(j=0;j<size;j++)
	{
		for(i=0;i<n;i++)
		{
			CALC_ALIGNED_SSD_16(_128u32_result, psrc, ppred, _128_zero, _128_aux)

			psrc+=size;
			ppred+=size;
		}
		psrc+=src_stride;
		ppred+=pred_stride;

	}
	return ssd = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,1)+sse_128_get_data_u32(_128u32_result,2)+sse_128_get_data_u32(_128u32_result,3);
}

uint32_t sse_aligned_ssd_4x4(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride)
{
	uint32_t ssd = 0;
	__m128_u32	_128_aux;
	__m128_u32	_128u32_result = sse_128_zero_vector();
	__m128_		_128_zero = sse_128_zero_vector();


	CALC_ALIGNED_SSD_2x4(_128u32_result, src, src+src_stride, pred, pred+pred_stride, _128_zero, _128_aux)	
	CALC_ALIGNED_SSD_2x4(_128u32_result, src+2*src_stride, src+3*src_stride, pred+2*pred_stride, pred+3*pred_stride, _128_zero, _128_aux)	

	return ssd = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,1)+sse_128_get_data_u32(_128u32_result,2)+sse_128_get_data_u32(_128u32_result,3);
}


uint32_t sse_aligned_ssd_8x8(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride)
{
	uint32_t ssd = 0;
	__m128_u32	_128_aux;
	__m128_u32	_128u32_result = sse_128_zero_vector();
	__m128_		_128_zero = sse_128_zero_vector();

	CALC_ALIGNED_SSD_2x8(_128u32_result, src, src+src_stride, pred, pred+pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_2x8(_128u32_result, src+2*src_stride, src+3*src_stride, pred+2*pred_stride, pred+3*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_2x8(_128u32_result, src+4*src_stride, src+5*src_stride, pred+4*pred_stride, pred+5*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_2x8(_128u32_result, src+6*src_stride, src+7*src_stride, pred+6*pred_stride, pred+7*pred_stride, _128_zero, _128_aux)

	return ssd = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,1)+sse_128_get_data_u32(_128u32_result,2)+sse_128_get_data_u32(_128u32_result,3);
}


uint32_t sse_aligned_ssd_16x16(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride)
{
	uint32_t ssd = 0;
	__m128_u32	_128_aux;
	__m128_u32	_128u32_result = sse_128_zero_vector();
	__m128_		_128_zero = sse_128_zero_vector();

	CALC_ALIGNED_SSD_16(_128u32_result, src, pred, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+1*src_stride, pred+1*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+2*src_stride, pred+2*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+3*src_stride, pred+3*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+4*src_stride, pred+4*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+5*src_stride, pred+5*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+6*src_stride, pred+6*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+7*src_stride, pred+7*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+8*src_stride, pred+8*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+9*src_stride, pred+9*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+10*src_stride, pred+10*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+11*src_stride, pred+11*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+12*src_stride, pred+12*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+13*src_stride, pred+13*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+14*src_stride, pred+14*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_16(_128u32_result, src+15*src_stride, pred+15*pred_stride, _128_zero, _128_aux)

	return ssd = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,1)+sse_128_get_data_u32(_128u32_result,2)+sse_128_get_data_u32(_128u32_result,3);
}



uint32_t sse_aligned_ssd_32x32(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride)
{
	uint32_t ssd = 0;
	__m128_u8	_128_aux;
	__m128_		_128_zero = sse_128_zero_vector();
	__m128_u32	_128u32_result = sse_128_zero_vector();

	CALC_ALIGNED_SSD_32(_128u32_result, src, pred, _128_zero, _128_aux);
	CALC_ALIGNED_SSD_32(_128u32_result, src+1*src_stride, pred+1*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+2*src_stride, pred+2*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+3*src_stride, pred+3*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+4*src_stride, pred+4*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+5*src_stride, pred+5*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+6*src_stride, pred+6*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+7*src_stride, pred+7*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+8*src_stride, pred+8*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+9*src_stride, pred+9*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+10*src_stride, pred+10*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+11*src_stride, pred+11*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+12*src_stride, pred+12*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+13*src_stride, pred+13*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+14*src_stride, pred+14*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+15*src_stride, pred+15*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+16*src_stride, pred+16*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+17*src_stride, pred+17*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+18*src_stride, pred+18*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+19*src_stride, pred+19*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+20*src_stride, pred+20*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+21*src_stride, pred+21*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+22*src_stride, pred+22*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+23*src_stride, pred+23*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+24*src_stride, pred+24*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+25*src_stride, pred+25*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+26*src_stride, pred+26*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+27*src_stride, pred+27*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+28*src_stride, pred+28*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+29*src_stride, pred+29*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+30*src_stride, pred+30*pred_stride, _128_zero, _128_aux)
	CALC_ALIGNED_SSD_32(_128u32_result, src+31*src_stride, pred+31*pred_stride, _128_zero, _128_aux)
	return ssd = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,1)+sse_128_get_data_u32(_128u32_result,2)+sse_128_get_data_u32(_128u32_result,3);
}


uint32_t sse_aligned_ssd_64x64(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride)
{
	int i;
	uint32_t ssd = 0;
	__m128_u8	_128_aux;
	__m128_		_128_zero = sse_128_zero_vector();
	__m128_u32	_128u32_result = sse_128_zero_vector();

	uint8_t *psrc = src;
	uint8_t *ppred = pred;



#ifndef EXTRA_OPTIMIZATION
	for(i=0;i<16;i++)
	{
		CALC_ALIGNED_SSD_64(_128u32_result, psrc, ppred, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+src_stride, ppred+pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+2*src_stride, ppred+2*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+3*src_stride, ppred+3*pred_stride, _128_zero, _128_aux);

		psrc+=4*src_stride;
		ppred+=4*pred_stride;
	}
#else //EXTRA_OPTIMIZATION
		CALC_ALIGNED_SSD_64(_128u32_result, psrc, ppred, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+src_stride, ppred+pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+2*src_stride, ppred+2*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+3*src_stride, ppred+3*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+4*src_stride, ppred+4*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+5*src_stride, ppred+5*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+6*src_stride, ppred+6*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+7*src_stride, ppred+7*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+8*src_stride, ppred+8*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+9*src_stride, ppred+9*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+10*src_stride, ppred+10*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+11*src_stride, ppred+11*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+12*src_stride, ppred+12*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+13*src_stride, ppred+13*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+14*src_stride, ppred+14*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+15*src_stride, ppred+15*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+16*src_stride, ppred+16*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+17*src_stride, ppred+17*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+18*src_stride, ppred+18*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+19*src_stride, ppred+19*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+20*src_stride, ppred+20*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+21*src_stride, ppred+21*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+22*src_stride, ppred+22*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+23*src_stride, ppred+23*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+24*src_stride, ppred+24*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+25*src_stride, ppred+25*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+26*src_stride, ppred+26*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+27*src_stride, ppred+27*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+28*src_stride, ppred+28*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+29*src_stride, ppred+29*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+30*src_stride, ppred+30*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+31*src_stride, ppred+31*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+32*src_stride, ppred+32*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+33*src_stride, ppred+33*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+34*src_stride, ppred+34*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+35*src_stride, ppred+35*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+36*src_stride, ppred+36*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+37*src_stride, ppred+37*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+38*src_stride, ppred+38*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+39*src_stride, ppred+39*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+40*src_stride, ppred+40*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+41*src_stride, ppred+41*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+42*src_stride, ppred+42*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+43*src_stride, ppred+43*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+44*src_stride, ppred+44*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+45*src_stride, ppred+45*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+46*src_stride, ppred+46*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+47*src_stride, ppred+47*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+48*src_stride, ppred+48*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+49*src_stride, ppred+49*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+50*src_stride, ppred+50*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+51*src_stride, ppred+51*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+52*src_stride, ppred+52*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+53*src_stride, ppred+53*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+54*src_stride, ppred+54*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+55*src_stride, ppred+55*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+56*src_stride, ppred+56*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+57*src_stride, ppred+57*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+58*src_stride, ppred+58*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+59*src_stride, ppred+59*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+60*src_stride, ppred+60*pred_stride, _128_zero, _128_aux);
		CALC_ALIGNED_SSD_64(_128u32_result, psrc+61*src_stride, ppred+61*pred_stride, _128_zero, _128_aux);
#endif
	return ssd = sse_128_get_data_u32(_128u32_result,0)+sse_128_get_data_u32(_128u32_result,1)+sse_128_get_data_u32(_128u32_result,2)+sse_128_get_data_u32(_128u32_result,3);

}




uint32_t sse_aligned_ssd(uint8_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride, int size)
{
	if(size==4)
		return sse_aligned_ssd_4x4(src, src_stride, pred, pred_stride);
	else if(size==8)
		return sse_aligned_ssd_8x8(src, src_stride, pred, pred_stride);
	else if(size==16)
		return sse_aligned_ssd_16x16(src, src_stride, pred, pred_stride);
	else if(size==32)
		return sse_aligned_ssd_32x32(src, src_stride, pred, pred_stride);
	else// if(size==64)
		return sse_aligned_ssd_64x64(src, src_stride, pred, pred_stride);
}



//---------------------------------------------predict ------------------------------------------------------------------

void sse_aligned_predict_4x4(uint8_t *orig, int orig_stride, int16_t *pred, int pred_stride, int16_t *residual, int residual_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();
//	int j;
/*	for(j=0;j<4;j++)
	{
		CALC_ALIGNED_PREDICT_4(orig, pred, residual, _128_zero)
		orig+=orig_stride;
		pred+=pred_stride;
		residual+=residual_stride;
	}
*/	
	CALC_ALIGNED_PREDICT_4(orig, pred, residual, _128_zero)
	CALC_ALIGNED_PREDICT_4(orig+orig_stride, pred+pred_stride, residual+residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_4(orig+2*orig_stride, pred+2*pred_stride, residual+2*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_4(orig+3*orig_stride, pred+3*pred_stride, residual+3*residual_stride, _128_zero)

}

void sse_aligned_predict_8x8(uint8_t *orig, int orig_stride, int16_t *pred, int pred_stride, int16_t *residual, int residual_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();
	int j;
	for(j=0;j<8;j++)
	{
		CALC_ALIGNED_PREDICT_8(orig, pred, residual, _128_zero)
		orig+=orig_stride;
		pred+=pred_stride;
		residual+=residual_stride;
	}

/*	CALC_ALIGNED_PREDICT_8(orig, pred, residual, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+orig_stride, pred+pred_stride, residual+residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+2*orig_stride, pred+2*pred_stride, residual+2*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+3*orig_stride, pred+3*pred_stride, residual+3*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+4*orig_stride, pred+4*pred_stride, residual+4*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+5*orig_stride, pred+5*pred_stride, residual+5*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+6*orig_stride, pred+6*pred_stride, residual+6*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_8(orig+7*orig_stride, pred+7*pred_stride, residual+7*residual_stride, _128_zero)
*/
}


void sse_aligned_predict_16x16(uint8_t *orig, int orig_stride, int16_t *pred, int pred_stride, int16_t *residual, int residual_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();

/*	for(int j=0;j<16;j++)
	{
		CALC_ALIGNED_PREDICT_16(orig, pred, residual, _128_zero)
		orig+=orig_stride;
		pred+=pred_stride;
		residual+=residual_stride;
	}
*/
	//this is a bit faster
	CALC_ALIGNED_PREDICT_16(orig, pred, residual, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+orig_stride, pred+pred_stride, residual+residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+2*orig_stride, pred+2*pred_stride, residual+2*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+3*orig_stride, pred+3*pred_stride, residual+3*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+4*orig_stride, pred+4*pred_stride, residual+4*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+5*orig_stride, pred+5*pred_stride, residual+5*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+6*orig_stride, pred+6*pred_stride, residual+6*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+7*orig_stride, pred+7*pred_stride, residual+7*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+8*orig_stride, pred+8*pred_stride, residual+8*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+9*orig_stride, pred+9*pred_stride, residual+9*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+10*orig_stride, pred+10*pred_stride, residual+10*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+11*orig_stride, pred+11*pred_stride, residual+11*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+12*orig_stride, pred+12*pred_stride, residual+12*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+13*orig_stride, pred+13*pred_stride, residual+13*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+14*orig_stride, pred+14*pred_stride, residual+14*residual_stride, _128_zero)
	CALC_ALIGNED_PREDICT_16(orig+15*orig_stride, pred+15*pred_stride, residual+15*residual_stride, _128_zero)

}



void sse_aligned_predict_32x32(uint8_t *orig, int orig_stride, int16_t *pred, int pred_stride, int16_t *residual, int residual_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();
	int j;

	for(j=0;j<32;j++)
	{
		CALC_ALIGNED_PREDICT_32(orig, pred, residual, _128_zero)
		orig+=orig_stride;
		pred+=pred_stride;
		residual+=residual_stride;
	}
}

void sse_aligned_predict_64x64(uint8_t *orig, int orig_stride, int16_t *pred, int pred_stride, int16_t *residual, int residual_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();
	int j;

	for(j=0;j<64;j++)
	{
		CALC_ALIGNED_PREDICT_64(orig, pred, residual, _128_zero)
		orig+=orig_stride;
		pred+=pred_stride;
		residual+=residual_stride;
	}
}


void sse_aligned_predict(uint8_t *orig, int orig_stride, int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int size)
{
	if(size==4)
		sse_aligned_predict_4x4(orig, orig_stride, pred, pred_stride, residual, residual_stride);
	else if(size==8)
		sse_aligned_predict_8x8(orig, orig_stride, pred, pred_stride, residual, residual_stride);
	else if(size==16)
		sse_aligned_predict_16x16(orig, orig_stride, pred, pred_stride, residual, residual_stride);
	else if(size==32)
		sse_aligned_predict_32x32(orig, orig_stride, pred, pred_stride, residual, residual_stride);
	else if(size==64)
		sse_aligned_predict_64x64(orig, orig_stride, pred, pred_stride, residual, residual_stride);
}


//---------------------------------------------reconst ------------------------------------------------------------------


void sse_aligned_reconst_4x4(int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int16_t *decoded, int decoded_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();
	__m128_i16 _128_aux1 = sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_adds_i16(sse128_unpacklo_u64(sse_128_load_vector_u(pred),sse_128_load_vector_u(pred+pred_stride)), sse128_unpacklo_u64(sse_128_load_vector_u(residual),sse_128_load_vector_u(residual+residual_stride))),_128_zero));
	__m128_i16 _128_aux2 = sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_adds_i16(sse128_unpacklo_u64(sse_128_load_vector_u(pred+2*pred_stride),sse_128_load_vector_u(pred+3*pred_stride)), sse128_unpacklo_u64(sse_128_load_vector_u(residual+2*residual_stride),sse_128_load_vector_u(residual+3*residual_stride))),_128_zero));

	sse_64_storel_vector_u(decoded, _128_aux1);
	sse_64_storeh_vector_u(decoded+decoded_stride, _128_aux1);
	sse_64_storel_vector_u(decoded+2*decoded_stride, _128_aux2);
	sse_64_storeh_vector_u(decoded+3*decoded_stride, _128_aux2);
}


void sse_aligned_reconst_8x8(int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int16_t *decoded, int decoded_stride)
{
	__m128_ _128_zero = sse_128_zero_vector();
	int j;
	for (j=0;j<8;j++)
	{
		CALC_ALIGNED_RECONST_8(pred, residual, decoded, _128_zero)	

		decoded += decoded_stride;
		residual += residual_stride;//este es 2D.Podria ser lineal
		pred += pred_stride;
	}
}

void sse_aligned_reconst_16x16(int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int16_t *decoded, int decoded_stride)
{
	__m128_	_128_zero = sse_128_zero_vector();
	int j;

	for (j=0;j<16;j++)
	{
		CALC_ALIGNED_RECONST_16(pred, residual, decoded, _128_zero)	

		decoded += decoded_stride;
		residual += residual_stride;//este es 2D.Podria ser lineal
		pred += pred_stride;
	}
}

void sse_aligned_reconst_32x32(int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int16_t *decoded, int decoded_stride)
{
	__m128_		_128_zero = sse_128_zero_vector();
	int j;

	for (j=0;j<32;j++)
	{
		CALC_ALIGNED_RECONST_32(pred, residual, decoded, _128_zero)	

		decoded += decoded_stride;
		residual += residual_stride;//este es 2D.Podria ser lineal
		pred += pred_stride;
	}
}

void sse_aligned_reconst_64x64(int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int16_t *decoded, int decoded_stride)
{
	__m128_	_128_zero = sse_128_zero_vector();
	int j;

	for (j=0;j<64;j++)
	{
		CALC_ALIGNED_RECONST_64(pred, residual, decoded, _128_zero)	

		decoded += decoded_stride;
		residual += residual_stride;//este es 2D.Podria ser lineal
		pred += pred_stride;
	}
}

void sse_aligned_reconst(int16_t *pred, int pred_stride, int16_t *residual, int residual_stride, int16_t *decoded, int decoded_stride, int size)
{
	if(size==4)
		sse_aligned_reconst_4x4(pred, pred_stride, residual, residual_stride, decoded, decoded_stride);
	else if(size==8)
		sse_aligned_reconst_8x8(pred, pred_stride, residual, residual_stride, decoded, decoded_stride);
	else if(size==16)
		sse_aligned_reconst_16x16(pred, pred_stride, residual, residual_stride, decoded, decoded_stride);
	else if(size==32)
		sse_aligned_reconst_32x32(pred, pred_stride, residual, residual_stride, decoded, decoded_stride);
	else if(size==64)
		sse_aligned_reconst_64x64(pred, pred_stride, residual, residual_stride, decoded, decoded_stride);
}


//--------------------------------------- variance -----------------------------------------

uint32_t sse_variance_16nx16n(uint8_t *__restrict p, int size, int stride, int modif)
{
	int i,j;
	unsigned int s=0;
	uint8_t *__restrict paux = p;
	__m128_i16	_128_one = sse_128_vector_i16(1);
	__m128_i16	_128_modif = sse_128_vector_i16(modif);
	__m128_i32	acc = sse_128_zero_vector();
	__m128_i16	avg_128;
	__m128_i32	var_128;

	//media 
	for (j=0; j<size; j++)
	{
		__m128_i16	s_128 = sse_128_zero_vector();
		for (i=0; i<size; i+=16)
		{
			__m128_i16	v0 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+i));
			__m128_i16	v1 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+i+8));
			s_128 = sse_128_add_i16(s_128, sse_128_add_i16(v0,v1));
		}
		acc = sse_128_add_i32(acc, sse_128_add_i32(sse_128_convert_i16_i32(s_128), sse_128_convert_i16_i32(sse128_unpackhi_u64(s_128,s_128))));

		paux+= stride;
	}
	acc = sse_128_hadd_i32(acc,sse_128_zero_vector());
	acc = sse_128_hadd_i32(acc,sse_128_zero_vector());

	s = sse_128_get_data_u32(acc,0);
	s/=(size*size);
	
	paux = p;

	avg_128 = sse_128_vector_i16(s);
	var_128 = sse_128_zero_vector();
	paux = p;
	for (j=0; j<size; j++)
	{
		__m128_i16	v_128 = sse_128_zero_vector();
		for (i=0; i<size; i+=16)
		{
			__m128_i16	v0 = sse_128_sub_i16(sse_128_convert_u8_i16(sse_128_load_vector_u(paux+i)),avg_128) ;
			__m128_i16	v1 = sse_128_sub_i16(sse_128_convert_u8_i16(sse_128_load_vector_u(paux+i+8)),avg_128);
			v0 = sse_128_add_i16(_128_one,sse_128_mul_i16(v0,_128_modif));
			v1 = sse_128_add_i16(_128_one,sse_128_mul_i16(v1,_128_modif));

			v_128 = sse_128_add_i32(v_128, sse_128_add_i32(sse_128_madd_i16_i32(v0,v0),sse_128_madd_i16_i32(v1,v1)));
		}
		var_128 = sse_128_add_i32(var_128, v_128);
		paux+= stride;
	}
	var_128 = sse_128_hadd_i32(var_128,sse_128_zero_vector());
	var_128 = sse_128_hadd_i32(var_128,sse_128_zero_vector());

	return 	sse_128_get_data_u32(var_128,0);
}

ALIGN(16) static const int8_t shuffle_mask_variance_16_0[16] ={ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};//0,1,2,3,4,5,6,7 -> 7,6,5,4,3,2,1,0

uint32_t sse_variance_8x8(uint8_t *__restrict p, int stride, int modif)
{
	uint8_t *__restrict paux = p;
	__m128_i16	_128_modif = sse_128_vector_i16(modif);
	__m128_i16	_128_one = sse_128_vector_i16(1);

	__m128_i16	zero = sse_128_zero_vector();
	__m128_i32	acc = sse_128_zero_vector();
	__m128_i32	var_128;
	__m128_i16	avg_128;

	__m128_i16	v0, v1, v2, v3, v4, v5, v6, v7;
	v0 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux));
	v1 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+stride));
	v2 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+2*stride));
	v3 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+3*stride));
	v4 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+4*stride));
	v5 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+5*stride));
	v6 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+6*stride));
	v7 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+7*stride));

	acc = sse_128_add_i16(sse_128_add_i16(v2, v3), sse_128_add_i16(v0, v1));
	acc = sse_128_add_i16(acc, sse_128_add_i16(sse_128_add_i16(v6, v7), sse_128_add_i16(v4, v5)));
	acc = sse_128_hadd_i16(acc, zero);
	acc = sse_128_hadd_i16(acc, zero);
	acc = sse_128_hadd_i16(acc, zero);

	avg_128 = sse_128_shift_r_i16(sse_128_shuffle_8(acc, sse_128_load_vector_u(shuffle_mask_variance_16_0)),6);

	v0 = sse_128_sub_i16(v0, avg_128);
	v0 = sse_128_add_i16(_128_one,sse_128_mul_i16(v0,_128_modif));
	v1 = sse_128_sub_i16(v1, avg_128);
	v1 = sse_128_add_i16(_128_one,sse_128_mul_i16(v1,_128_modif));
	v2 = sse_128_sub_i16(v2, avg_128);
	v2 = sse_128_add_i16(_128_one,sse_128_mul_i16(v2,_128_modif));
	v3 = sse_128_sub_i16(v3, avg_128);
	v3 = sse_128_add_i16(_128_one,sse_128_mul_i16(v3,_128_modif));
	v4 = sse_128_sub_i16(v4, avg_128);
	v4 = sse_128_add_i16(_128_one,sse_128_mul_i16(v4,_128_modif));
	v5 = sse_128_sub_i16(v5, avg_128);
	v5 = sse_128_add_i16(_128_one,sse_128_mul_i16(v5,_128_modif));
	v6 = sse_128_sub_i16(v6, avg_128);
	v6 = sse_128_add_i16(_128_one,sse_128_mul_i16(v6,_128_modif));
	v7 = sse_128_sub_i16(v7, avg_128);
	v7 = sse_128_add_i16(_128_one,sse_128_mul_i16(v7,_128_modif));


	var_128 = sse_128_madd_i16_i32(v0,v0);
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v1,v1));
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v2,v2));
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v3,v3));
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v4,v4));
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v5,v5));
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v6,v6));
	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v7,v7));

	var_128 = sse_128_hadd_i32(var_128,zero);
	var_128 = sse_128_hadd_i32(var_128,zero);

	return 	sse_128_get_data_u32(var_128,0);
}


uint32_t sse_variance_4x4(uint8_t *__restrict p, int stride, int modif)
{
	uint8_t *__restrict paux = p;
	__m128_i16	_128_modif = sse_128_vector_i16(modif);
	__m128_i16	_128_one = sse_128_vector_i16(1);
	__m128_i32	acc = sse_128_zero_vector();
	__m128_i16	v0 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux));
	__m128_i16	v1 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+stride));
	__m128_i16	v2 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+2*stride));
	__m128_i16	v3 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+3*stride));
	__m128_i16	v4, v5;
	__m128_i16	avg_128, var_128;


	acc = sse_128_add_i16(sse_128_add_i16(v2, v3), sse_128_add_i16(v0, v1));
	acc = sse_128_hadd_i16(acc, acc);
	acc = sse_128_hadd_i16(acc, acc);

	avg_128 = sse_128_shift_r_i16(sse_128_shuffle_8(acc, sse_128_load_vector_u(shuffle_mask_variance_16_0)),4);

	v4 = sse_128_sub_i16(sse128_unpacklo_u16(v0,v1), avg_128);
	v4 = sse_128_add_i16(_128_one,sse_128_mul_i16(v4,_128_modif));
	var_128 = sse_128_madd_i16_i32(v4,v4);

	v5 = sse_128_sub_i16(sse128_unpacklo_u16(v2,v3), avg_128);
	v5 = sse_128_add_i16(_128_one,sse_128_mul_i16(v5,_128_modif));

	var_128 = sse_128_add_i32(var_128, sse_128_madd_i16_i32(v5,v5));

	var_128 = sse_128_hadd_i32(var_128,sse_128_zero_vector());
	var_128 = sse_128_hadd_i32(var_128,sse_128_zero_vector());

	return 	sse_128_get_data_u32(var_128,0);
}

uint32_t sse_variance_2x2(uint8_t *__restrict p, int stride, int modif)
{
	uint8_t *__restrict paux = p;

	__m128_i16	_128_modif = sse_128_vector_i16(modif);
	__m128_i16	_128_one = sse_128_vector_i16(1);

	__m128_i32	acc = sse_128_zero_vector();

	__m128_i16	v0 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux));
	__m128_i16	v1 = sse_128_convert_u8_i16(sse_128_load_vector_u(paux+stride));
	__m128_i16	avg_128;
	__m128_i16	v3;
	__m128_i16	var_128;

	acc = sse_128_add_i16(v0, v1);
	acc = sse_128_hadd_i16(acc, acc);

	avg_128 = sse_128_shift_r_i16(sse_128_shuffle_8(acc, sse_128_load_vector_u(shuffle_mask_variance_16_0)),2);

	v3 = sse_128_sub_i16(sse128_unpacklo_u16(v0,v1), avg_128);
	v3 = sse_128_add_i16(_128_one,sse_128_mul_i16(v3,_128_modif));
	var_128 = sse_128_madd_i16_i32(v3,v3);

	var_128 = sse_128_hadd_i32(var_128, var_128);
	return 	sse_128_get_data_u32(var_128,0);
}



uint32_t sse_modified_variance(uint8_t *p, int size, int stride, int modif)
{
	if(size==2)
		return sse_variance_2x2(p, stride, modif);
	if(size==4)
		return sse_variance_4x4(p, stride, modif);
	else if(size==8)
		return sse_variance_8x8(p, stride, modif);
	else //if(size>8)
		return sse_variance_16nx16n(p, size, stride, modif);
}



