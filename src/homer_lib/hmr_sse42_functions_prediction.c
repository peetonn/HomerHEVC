/*****************************************************************************
* hmr_sse42_functions_prediction.c : homerHEVC encoding library
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

#include "hmr_private.h"
#include "hmr_common.h"

#include "hmr_sse42_primitives.h"
#include "hmr_sse42_macros.h"
#include "hmr_sse42_functions.h"


//------------------------------------- create_prediction ---------------------------------------------------------------------


ALIGN(16) static const int8_t shuffle_mask_prediction_16_0[16] ={ 14, 15, 12, 13, 10, 11,  8,  9,  6,  7,  4,  5,  2,  3,  0,  1};//0,1,2,3,4,5,6,7 -> 7,6,5,4,3,2,1,0
ALIGN(16) static const int8_t shuffle_mask_prediction_16_1[16] ={  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1};//0,1,2,3,4,5,6,7 -> 1,1,1,1,1,1,1,1
ALIGN(16) static const int8_t shuffle_mask_prediction_16_2[16] ={  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2};
ALIGN(16) static const int8_t shuffle_mask_prediction_16_3[16] ={  1,  0,  2,  0,  3,  0,  4,  0,  5,  0,  6,  0,  7,  0,  8,  0};//
ALIGN(16) static const int8_t shuffle_mask_prediction_16_4[16] ={ 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15};//0,1,2,3,4,5,6,7 -> 1,1,1,1,1,1,1,1



void sse_create_intra_planar_prediction_4(uint16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size)
{
	int j;
	int16_t  *adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size);

	__m128_u16 shuffle_mask_reorder16 = sse_128_load_vector_u(shuffle_mask_prediction_16_0);

	__m128_u16 _128_cu_size = sse_128_vector_i16(4);

	__m128_u16 bottomLeft = sse_128_vector_i16(adi_ptr[-(4+1)]);//this is 8 for cu_size=8 and cu_size=4, because of the reordering mask aplied
	__m128_u16 topRight = sse_128_vector_i16(adi_ptr[4+1]);

	__m128_u16 left = sse_128_shuffle_8(sse_128_load_vector_u(&adi_ptr[-8]),shuffle_mask_reorder16);
	__m128_u16 top = sse_128_load_vector_u(&adi_ptr[1]);

	__m128_u16 bottomRow = sse_128_sub_i16(bottomLeft,top);
	__m128_u16 rightColumn = sse_128_sub_i16(topRight,left);
	__m128_u16 topRow = sse_128_shift_l_i16(top,2);
	__m128_u16 leftColumn = sse_128_shift_l_i16(left,2);

	__m128_u16 shuffle_mask_next_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_2);
	__m128_u16 mask_mulitiply16 = sse_128_load_vector_u(shuffle_mask_prediction_16_3);
	__m128_u16 left_colum_plus_cu_size = sse_128_add_i16(leftColumn, _128_cu_size);
	__m128_u16 shuffle_mask_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_1);

	for (j=0;j<4;j++)
	{
		__m128_u16 right_column_j = sse_128_mul_i16(sse_128_shuffle_8(rightColumn, shuffle_mask_element16),mask_mulitiply16);
		__m128_u16 horPred = sse_128_shuffle_8(left_colum_plus_cu_size, shuffle_mask_element16);//make a 126_i16 register full of left_colum_plus_cu_size[j]
		horPred = sse_128_add_i16(horPred, right_column_j);
		topRow = sse_128_add_i16(topRow, bottomRow);

		sse_64_storel_vector_u(ref_wnd, sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_shift_r_i16(sse_128_add_i16(horPred, topRow), 2+1), _128_cu_size)));//this _128_cu_size is whatever
		
		ref_wnd+=ref_wnd_stride_2D;
		shuffle_mask_element16 = sse_128_add_i8(shuffle_mask_element16, shuffle_mask_next_element16);
	}
}



void sse_create_intra_planar_prediction_8(uint16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size)
{
	int j;
	int16_t  *adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size);

	__m128_u16 shuffle_mask_reorder16 = sse_128_load_vector_u(shuffle_mask_prediction_16_0);

	__m128_u16 _128_cu_size = sse_128_vector_i16(8);

	__m128_u16 bottomLeft = sse_128_vector_i16(adi_ptr[-(8+1)]);
	__m128_u16 topRight = sse_128_vector_i16(adi_ptr[8+1]);

	__m128_u16 left = sse_128_shuffle_8(sse_128_load_vector_u(&adi_ptr[-8]),shuffle_mask_reorder16);
	__m128_u16 top = sse_128_load_vector_u(&adi_ptr[1]);

	__m128_u16 bottomRow = sse_128_sub_i16(bottomLeft,top);
	__m128_u16 rightColumn = sse_128_sub_i16(topRight,left);
	__m128_u16 topRow = sse_128_shift_l_i16(top,3);
	__m128_u16 leftColumn = sse_128_shift_l_i16(left,3);

	// Generate prediction signal
//	__m128_u16 shuffle_mask_last_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_4);
	__m128_u16 shuffle_mask_next_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_2);
	__m128_u16 mask_mulitiply16 = sse_128_load_vector_u(shuffle_mask_prediction_16_3);
	__m128_u16 left_colum_plus_cu_size = sse_128_add_i16(leftColumn, _128_cu_size);
	__m128_u16 shuffle_mask_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_1);

	for (j=0;j<8;j++)
	{
		__m128_u16 right_column_j = sse_128_mul_i16(sse_128_shuffle_8(rightColumn, shuffle_mask_element16),mask_mulitiply16);
		__m128_u16 horPred = sse_128_shuffle_8(left_colum_plus_cu_size, shuffle_mask_element16);//make a 126_i16 register full of left_colum_plus_cu_size[j]

		horPred = sse_128_add_i16(horPred, right_column_j);
		topRow = sse_128_add_i16(topRow, bottomRow);
		sse_128_store_vector_u(ref_wnd, sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_shift_r_i16(sse_128_add_i16(horPred, topRow), 3+1), _128_cu_size)));//this _128_cu_size is whatever
		
		ref_wnd+=ref_wnd_stride_2D;
		shuffle_mask_element16 = sse_128_add_i8(shuffle_mask_element16, shuffle_mask_next_element16);
	}

}



void sse_create_intra_planar_prediction_8_16_32_64(int16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_size_shift)
{
	int i, j, jj, iterations = (cu_size>>3);
	int16_t  *adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size);

	__m128_u16 bottomRow[8];
	__m128_u16 rightColumn[8];
	__m128_u16 topRow[8];
	__m128_u16 leftColumn[8];

	__m128_u16 shuffle_mask_reorder16 = sse_128_load_vector_u(shuffle_mask_prediction_16_0);

	__m128_u16 _128_cu_size = sse_128_vector_i16(cu_size);

	__m128_u16 bottomLeft = sse_128_vector_i16(adi_ptr[-(cu_size+1)]);
	__m128_u16 topRight = sse_128_vector_i16(adi_ptr[cu_size+1]);
	__m128_u16 shuffle_mask_last_element16, shuffle_mask_next_element16, mask_mulitiply16;

	for(i=0;i<iterations;i++)
	{
		__m128_u16 left = sse_128_shuffle_8(sse_128_load_vector_u(&adi_ptr[-(i*8+8)]),shuffle_mask_reorder16);
		__m128_u16 top = sse_128_load_vector_u(&adi_ptr[(i*8+1)]);

		bottomRow[i] = sse_128_sub_i16(bottomLeft,top);
		rightColumn[i] = sse_128_sub_i16(topRight,left);
		topRow[i] = sse_128_shift_l_i16(top,cu_size_shift);
		leftColumn[i] = sse_128_shift_l_i16(left,cu_size_shift);

//		left = sse_128_shuffle_8(sse_128_load_vector_u(&adi_ptr[-((i+1)*8+8)]),shuffle_mask_reorder16);
//		top = sse_128_load_vector_u(&adi_ptr[((i+1)*8+1)]);

//		bottomRow[(i+1)] = sse_128_sub_i16(bottomLeft,top);
//		rightColumn[(i+1)] = sse_128_sub_i16(topRight,left);
//		topRow[(i+1)] = sse_128_shift_l_i16(top,cu_size_shift);
//		leftColumn[(i+1)] = sse_128_shift_l_i16(left,cu_size_shift);
	}


	// Generate prediction signal
	shuffle_mask_last_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_4);
	shuffle_mask_next_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_2);
	mask_mulitiply16 = sse_128_load_vector_u(shuffle_mask_prediction_16_3);

	for (jj=0;jj<iterations;jj++)
	{
		__m128_u16 left_colum_plus_cu_size = sse_128_add_i16(leftColumn[jj], _128_cu_size);
		__m128_u16 shuffle_mask_element16 = sse_128_load_vector_u(shuffle_mask_prediction_16_1);
		for (j=0;j<8;j++)
		{
			__m128_u16 horPred = sse_128_shuffle_8(left_colum_plus_cu_size, shuffle_mask_element16);//make a 126_i16 register full of left_colum_plus_cu_size[j]
			__m128_u16 right_column_j = sse_128_mul_i16(sse_128_shuffle_8(rightColumn[jj], shuffle_mask_element16),mask_mulitiply16);
			for (i=0;i<iterations;i++)
			{
				__m128_u16 aux;
				horPred = sse_128_shuffle_8(horPred, shuffle_mask_last_element16);
				aux = sse_128_add_i16(horPred, right_column_j);
				topRow[i] = sse_128_add_i16(topRow[i], bottomRow[i]);
				horPred = sse_128_shuffle_8(aux, shuffle_mask_last_element16);
//				horPred = sse_128_add_i16(horPred, right_column_j);
//				topRow[i+1] = sse_128_add_i16(topRow[i+1], bottomRow[i+1]);
				sse_128_store_vector_u(ref_wnd+8*i, sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_shift_r_i16(sse_128_add_i16(aux, topRow[i]), cu_size_shift+1), sse_128_shift_r_i16(sse_128_add_i16(horPred, topRow[i+1]), cu_size_shift+1))));
//				sse_128_store_vector_a(ref_wnd+8*i, sse128_packs_i16_u8(sse_128_shift_r_i16(sse_128_add_i16(aux, topRow[i]), cu_size_shift+1), sse_128_shift_r_i16(sse_128_add_i16(horPred, topRow[i+1]), cu_size_shift+1)));
			}
			ref_wnd+=ref_wnd_stride_2D;
			shuffle_mask_element16 = sse_128_add_i8(shuffle_mask_element16, shuffle_mask_next_element16);
		}
	}
}

void sse_create_intra_planar_prediction(henc_thread_t* et, int16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_size_shift)
{
	if(cu_size==4)
		sse_create_intra_planar_prediction_4(ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size);
	else if(cu_size==8)
		sse_create_intra_planar_prediction_8(ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size);
	else
		sse_create_intra_planar_prediction_8_16_32_64(ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size, cu_size, cu_size_shift);
}



// get dc value for intra pred
int16_t sse_pred_intra_calc_dc(int16_t *adi_ptr, int width, int height, int top, int left )
{
	int aux = 0;
	int16_t dc;

	__m128_	zero = sse_128_zero_vector();
	__m128_	sum = sse_128_zero_vector();

	if (top)
	{
		if(width==4)
		{
			sum = sse_128_hadd_i16(sse_128_loadlo_vector64(adi_ptr+1),sum);
		}
		else if(width==8)
		{
			sum = sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+1),sum);
		}
		else if(width==16)
		{
			sum = sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+1),sse_128_load_vector_u(adi_ptr+9)),sum);
		}
		else if(width==32)
		{
			sum = sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+1),sse_128_load_vector_u(adi_ptr+9)),sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+17),sse_128_load_vector_u(adi_ptr+25))),sum);
		}
		else if(width==64)
		{
			sum = sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+1),sse_128_load_vector_u(adi_ptr+9)),sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+17),sse_128_load_vector_u(adi_ptr+25))),sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+33),sse_128_load_vector_u(adi_ptr+41)),sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr+49),sse_128_load_vector_u(adi_ptr+57)))),sum);
		}
	}
	if (left)
	{
		if(width==4)
		{
			sum = sse_128_hadd_i16(sse_128_loadlo_vector64(adi_ptr-4),sum);
		}
		else if(width==8)
		{
			sum = sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-8),sum);
		}
		else if(width==16)
		{
			sum = sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-8),sse_128_load_vector_u(adi_ptr-16)),sum);
		}
		else if(width==32)
		{
			sum = sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-8),sse_128_load_vector_u(adi_ptr-16)),sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-24),sse_128_load_vector_u(adi_ptr-32))),sum);
		}
		else if(width==64)
		{
			sum = sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-8),sse_128_load_vector_u(adi_ptr-16)),sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-24),sse_128_load_vector_u(adi_ptr-32))),sse_128_hadd_i16(sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-40),sse_128_load_vector_u(adi_ptr-48)),sse_128_hadd_i16(sse_128_load_vector_u(adi_ptr-56),sse_128_load_vector_u(adi_ptr-64)))),sum);
		}
	}

	if (top && left)
	{
		aux = sse_128_get_data_u16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sum,zero),zero),zero),0);
		dc = (aux + width) / (width + height);
	}
	else if (top)
	{
		aux = sse_128_get_data_u16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sum,zero),zero),zero),0);
		dc = (aux + width/2) / width;
	}
	else if (left)
	{
		aux = sse_128_get_data_u16(sse_128_hadd_i16(sse_128_hadd_i16(sse_128_hadd_i16(sum,zero),zero),zero),0);
		dc = (aux + height/2) / height;
	}
	else
	{
		dc = adi_ptr[0];//ref_wnd[-1]; // Default DC value already done and placed in the prediction array if no neighbors are available
	}

	return dc;
}


ALIGN(16) static const int8_t shuffle_mask_predict_16_0[16] ={ 14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1};//0,1,2,3,4,5,6,7 -> 7,6,5,4,3,2,1,0
//for cu_size >= 8
void sse_create_intra_angular_prediction_nxn(henc_thread_t* et, ctu_info_t* ctu, uint16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_mode, int is_luma)//creamos el array de prediccion angular
{
	int i, j;
	int is_DC_mode = cu_mode < 2;
	int16_t *adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size);

	// dc prediction
	if (is_DC_mode)
	{
		uint16_t dcval = sse_pred_intra_calc_dc(adi_ptr, cu_size, cu_size, ctu->top, ctu->left);	
		__m128_u8 _128_dcval = sse_128_vector_i16((byte)dcval);
		int16_t *dst = ref_wnd;	

		for (j=0;j<cu_size;j++)
		{
			for (i=0;i<cu_size;i+=8)
			{
				sse_128_store_vector_u(dst+i, _128_dcval);
			}
			dst+=ref_wnd_stride_2D;
		}

		if(cu_size<=16 && cu_mode == DC_IDX && is_luma)
		{
			__m128_u8 _128_two = sse_128_vector_i16(2);
			__m128_u8 _128_three = sse_128_vector_i16(3);

			uint8_t ref0 = ((adi_ptr[-1] + adi_ptr[1] + 2*(int16_t)ref_wnd[0] + 2)>>2);//

			adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size) + 1;

			for(i=0;i<cu_size;i+=8)		//for(i=1;i<cu_size;i+=8)		
			{
				__m128_u8 aux = sse_128_mul_i16(sse_128_load_vector_u(ref_wnd+i),_128_three);
				__m128_u8 aux2 = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_load_vector_u(adi_ptr+i), aux), _128_two), 2);
				sse_128_store_vector_u(ref_wnd+i, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux2,aux2)));
			}
			ref_wnd[0] = ref0;

	
			adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size) - 1;
			for(j=1;j<cu_size;j++)		
			{
				ref_wnd[j*ref_wnd_stride_2D] = ((adi_ptr[-j] + 3*(int16_t)ref_wnd[j*ref_wnd_stride_2D] + 2)>>2);
			}
		}

	}
	else//angular predictions
	{
		int is_Hor_mode = !is_DC_mode && (cu_mode < 18);
		int is_Ver_mode = !is_DC_mode && !is_Hor_mode;
		int pred_angle = is_Ver_mode ? cu_mode - VER_IDX : is_Hor_mode ? -(cu_mode - HOR_IDX) : 0;
		int abs_angle = abs(pred_angle);
		int sign_angle = SIGN(pred_angle);
		int inv_angle = et->ed->inv_ang_table[abs_angle];
		int bit_depth = 8;//et->bit_depth;
		int16_t* refMain;
		int16_t* refSide;
		int16_t  *refAbove = et->top_pred_buff;
		int16_t  *refLeft = et->left_pred_buff;
		int invAngleSum;
		int bFilter = is_luma?(cu_size<=16):0;
		__m128_i16 shuff_mask = sse_128_load_vector_a(shuffle_mask_predict_16_0);//0,1,2,3,4,5,6,7 -> 7,6,5,4,3,2,1,0
		abs_angle = et->ed->ang_table[abs_angle];
		pred_angle = sign_angle * abs_angle;

		if (pred_angle < 0)
		{
			for (i=0;i<cu_size;i+=8)
			{
				sse_128_store_vector_u(refAbove +(i+cu_size-1), sse_128_load_vector_u(adi_ptr+i));
			}
			refAbove[i+cu_size-1] = adi_ptr[i];

			for (i=0;i<cu_size;i+=8)
			{
				sse_128_store_vector_u(refLeft+(i+cu_size-1), sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7+i)), shuff_mask));
			}
			refLeft[i+cu_size-1] = adi_ptr[-(i)];
			refMain = (is_Ver_mode ? refAbove : refLeft) + (cu_size-1);
			refSide = (is_Ver_mode ? refLeft : refAbove) + (cu_size-1);

			invAngleSum    = 128;       // rounding for (shift by 8)
			for (i=-1; i>((cu_size*pred_angle)>>5); i--)
			{
				invAngleSum += inv_angle;
				refMain[i] = refSide[invAngleSum>>8];
			}
		}
		else	//pred_angle >= 0
		{
			for (i=0;i<2*cu_size;i+=8)
			{
				sse_128_store_vector_u(refAbove+i, sse_128_load_vector_u(adi_ptr+i));
				sse_128_store_vector_u(refLeft+i, sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7+i)), shuff_mask));
			}
			refAbove[i] = adi_ptr[i];
			refLeft[i] = adi_ptr[-(i)];
			refMain = is_Ver_mode ? refAbove : refLeft;
			refSide = is_Ver_mode ? refLeft  : refAbove;
		}

		if (pred_angle == 0)
		{
//			int aux_stride = is_Hor_mode?1:ref_wnd_stride_2D;//if is_Hor_mode flip horizontal and vertical axis by changing the x and y stride
//			int aux_stride2 = is_Hor_mode?ref_wnd_stride_2D:1;

			for (i=0;i<cu_size;i+=8)
			{
				__m128_i16 aux = sse_128_load_vector_u(refMain+i+1);
				for (j=0;j<cu_size;j++)
				{
					sse_128_store_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, aux);
				}
			}			

			if(is_Hor_mode)//transpose matrix. 
			{
				TRANSPOSE_MATRIX_16BIT(ref_wnd, cu_size, ref_wnd_stride_2D)
				if ( bFilter )
				{
					int16_t max_value = (1<<bit_depth)-1;
					for (i=0;i<cu_size;i++)
					{
						ref_wnd[i] = clip(ref_wnd[i] + (( refSide[i+1] - refSide[0] ) >> 1) , 0, max_value);
					}
				}
			}
			else
			{
				if ( bFilter )
				{
					int16_t max_value = (1<<bit_depth)-1;
					for (i=0;i<cu_size;i++)
					{
						ref_wnd[i*ref_wnd_stride_2D] = clip(ref_wnd[i*ref_wnd_stride_2D] + (( refSide[i+1] - refSide[0] ) >> 1) , 0, max_value);
					}
				}
			}
		}
		else
		{
			int aux_stride = is_Hor_mode?1:ref_wnd_stride_2D;//si is_Hor_mode le damos la vuelta (cambiando el stride de x e y)
			int aux_stride2 = is_Hor_mode?ref_wnd_stride_2D:1;
			int pos_delta=0;
			int aux_delta;
			int ref_main_idx;

			if(pred_angle==32)
			{
				for (j=0;j<cu_size;j++)
				{
					pos_delta += pred_angle;
					aux_delta   = pos_delta >> 5;

					for (i=0;i<cu_size;i+=8)
					{
						__m128_i16 aux = sse_128_load_vector_u(refMain+i+aux_delta+1);
						sse_128_store_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, aux);//sse_64_storel_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, sse128_packs_i16_u8(aux,aux));
					}
				}
			}
			else
			{
				__m128_i16 _128_pos_delta=sse_128_zero_vector();
				__m128_i16 _128_pred_angle=sse_128_vector_i16(pred_angle);
				__m128_i16 _128_32 = sse_128_vector_i16(32);
				__m128_i16 _128_32_mask = sse_128_vector_i16(31);
				__m128_i16 _128_round = sse_128_vector_i16(16);
				__m128_i16 _128_frac_delta, _128_32_minus_frac_delta;
				for (j=0;j<cu_size;j++)//for (j=0;j<8;j++)//for (j=0;j<16;j++)
				{
					pos_delta += pred_angle;
					aux_delta   = pos_delta >> 5;
					_128_pos_delta=sse_128_add_i16(_128_pos_delta,_128_pred_angle);
					_128_frac_delta = sse_128_and(_128_pos_delta, _128_32_mask);
					_128_32_minus_frac_delta = sse_128_sub_i16 (_128_32,_128_frac_delta);

					for (i=0;i<cu_size;i+=8)//for (i=0;i<cu_size;i+=8)//for (i=0;i<cu_size;i+=16)
					{
						__m128_i16 aux;
						ref_main_idx = i+aux_delta+1;
						aux = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx), _128_32_minus_frac_delta), sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx+1), _128_frac_delta)),_128_round),5);
						sse_128_store_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux,aux)));
					}
				}
			}
			if(is_Hor_mode)//transpose matrix. 
			{
				TRANSPOSE_MATRIX_16BIT(ref_wnd, cu_size, ref_wnd_stride_2D)
			}
		}
	}
}


								

/*void sse_create_intra_angular_prediction_8x8(henc_thread_t* et, ctu_info_t* ctu, int16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_mode, int is_luma)//creamos el array de prediccion angular
{
	int i, j, jj;
	int is_DC_mode = cu_mode < 2;
	int is_Hor_mode = !is_DC_mode && (cu_mode < 18);
	int is_Ver_mode = !is_DC_mode && !is_Hor_mode;
	int pred_angle = is_Ver_mode ? cu_mode - VER_IDX : is_Hor_mode ? -(cu_mode - HOR_IDX) : 0;
	//pred_angle = is_Ver_mode?cu_mode=[18,34]-26 = [-8,8]:modeHor?dirMode=[2,17]-10 =[8,-7]:0	
	//darse cuenta que -8 vertical = -8 horizontal (es la diagonal);//por eso hay 35 modos. 33 angulares+dc+planar
	int abs_angle = abs(pred_angle);
	int sign_angle = SIGN(pred_angle);
	int16_t  *adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size);
	int inv_angle = et->ed->inv_ang_table[abs_angle];
	int bit_depth = et->bit_depth;
	int16_t *dst;
	int cu_size = 8; 
	abs_angle = et->ed->ang_table[abs_angle];
	pred_angle = sign_angle * abs_angle;

	// dc prediction
	if (is_DC_mode)
	{
		uint16_t dcval = sse_pred_intra_calc_dc(adi_ptr, cu_size, cu_size, ctu->top, ctu->left);
		__m128_u8 _128_dcval = sse_128_vector_i16((byte)dcval);

		dst = ref_wnd;
		for (j=0;j<cu_size;j++)
		{
			sse_128_store_vector_u(dst, _128_dcval);
			dst+=ref_wnd_stride_2D;
		}

		if(cu_mode == DC_IDX && is_luma)
		{
			__m128_u8 _128_two = sse_128_vector_i16(2);
			__m128_u8 _128_three = sse_128_vector_i16(3);
			__m128_u8 aux, aux2;
			uint8_t ref0 = ((adi_ptr[-1] + adi_ptr[1] + 2*(int16_t)ref_wnd[0] + 2)>>2);//			ref_wnd[0] = ((adi_ptr[-1] + adi_ptr[1] + 2*(int16_t)ref_wnd[0] + 2)>>2);

			adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size) + 1;

			aux = sse_128_mul_i16(sse_128_convert_u8_i16(sse_128_load_vector_u(ref_wnd)),_128_three);
			aux2 = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_load_vector_u(adi_ptr), aux), _128_two), 2);
			sse_128_store_vector_u(ref_wnd, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux2,aux2)));
			ref_wnd[0] = ref0;

	
			adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size) - 1;
			for(j=1;j<cu_size;j++)		
			{
				ref_wnd[j*ref_wnd_stride_2D] = ((adi_ptr[-j] + 3*(int16_t)ref_wnd[j*ref_wnd_stride_2D] + 2)>>2);
			}
		}

	}
	else//angular predictions
	{
		int16_t* refMain;
		int16_t* refSide;
		int16_t  *refAbove = et->top_pred_buff;
		int16_t  *refLeft = et->left_pred_buff;
		int invAngleSum;
		int bFilter = is_luma?(cu_size<=16):0;
		__m128i shuff_mask = sse_128_load_vector_a(shuffle_mask_predict_16_0);//0,1,2,3,4,5,6,7 -> 7,6,5,4,3,2,1,0

//		memset(et->top_pred_buff,0,128);
//		memset(et->left_pred_buff,0,128);

		if (pred_angle < 0)
		{

			sse_128_store_vector_u(refAbove +(cu_size-1), sse_128_load_vector_u(adi_ptr));
			refAbove[8+cu_size-1] = adi_ptr[8];

			sse_128_store_vector_u(refLeft+(cu_size-1), sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7)), shuff_mask));
			refLeft[8+cu_size-1] = adi_ptr[-8];

			refMain = (is_Ver_mode ? refAbove : refLeft) + (cu_size-1);
			refSide = (is_Ver_mode ? refLeft : refAbove) + (cu_size-1);

			invAngleSum    = 128;       // rounding for (shift by 8)
			for (i=-1; i>((cu_size*pred_angle)>>5); i--)
			{
				invAngleSum += inv_angle;
				refMain[i] = refSide[invAngleSum>>8];
			}
		}
		else	//pred_angle >= 0
		{

			sse_128_store_vector_u(refAbove, sse_128_load_vector_u(adi_ptr));
			sse_128_store_vector_u(refAbove+8, sse_128_load_vector_u(adi_ptr+8));
			refAbove[16] = adi_ptr[16];
			sse_128_store_vector_u(refLeft, sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7)), shuff_mask));
			sse_128_store_vector_u(refLeft+8, sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7+8)), shuff_mask));
			refLeft[16] = adi_ptr[-16];
			refMain = is_Ver_mode ? refAbove : refLeft;
			refSide = is_Ver_mode ? refLeft  : refAbove;
		}

		if (pred_angle == 0)
		{
			int aux_stride = is_Hor_mode?1:ref_wnd_stride_2D;//if is_Hor_mode flip horizontal and vertical axis by changing the x and y stride
			int aux_stride2 = is_Hor_mode?ref_wnd_stride_2D:1;

			if(is_Hor_mode)
			{
				__m128_i16 l[8];
				__m128_i16 aux = sse_128_load_vector_u(refMain+1);

				for (j=0;j<cu_size;j++)
				{
					l[j] = sse128_packs_i16_u8(aux,aux);
				}
				TRANSPOND_MATRIX_8x8_16BITS(l, ref_wnd, ref_wnd_stride_2D)
			}
			else
			{
				for (i=0;i<cu_size;i+=8)
				{
					__m128_i16 aux = sse_128_load_vector_u(refMain+i+1);
					for (j=0;j<cu_size;j++)
					{
						sse_128_store_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux,aux)));
					}
				}
			}
			

			if ( bFilter )
			{
				for (i=0;i<cu_size;i++)
				{
					ref_wnd[i*aux_stride] = clip(ref_wnd[i*aux_stride] + (( refSide[i+1] - refSide[0] ) >> 1) , 0, (1<<bit_depth)-1);
				}
			}
		}
		else
		{
			int aux_stride = is_Hor_mode?1:ref_wnd_stride_2D;//si is_Hor_mode le damos la vuelta (cambiando el stride de x e y)
			int aux_stride2 = is_Hor_mode?ref_wnd_stride_2D:1;
			int pos_delta=0;
			int aux_delta;
			int fract_delta;
			int ref_main_idx;
			__m128_i16 _128_pos_delta=sse_128_zero_vector();
			__m128_i16 _128_pred_angle=sse_128_vector_i16(pred_angle);
			__m128_i16 _128_32 = sse_128_vector_i16(32);
			__m128_i16 _128_32_mask = sse_128_vector_i16(31);
			__m128_i16 _128_round = sse_128_vector_i16(16);
			__m128_i16 _128_frac_delta, _128_32_minus_frac_delta;

			if(is_Hor_mode)
			{
				__m128_i16 l[8];
				if(pred_angle==32)
				{
					for (j=0;j<cu_size;j++)
					{
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;

						l[j] = sse128_packs_i16_u8(sse_128_load_vector_u(refMain+aux_delta+1),_128_32);//_128_32 or whatever (is discarded);
					}
				}
				else
				{
					for (j=0;j<cu_size;j++)
					{
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;
						_128_pos_delta=sse_128_add_i16(_128_pos_delta,_128_pred_angle);
						_128_frac_delta = sse_128_and(_128_pos_delta, _128_32_mask);
						_128_32_minus_frac_delta = sse_128_sub_i16 (_128_32,_128_frac_delta);

						ref_main_idx = aux_delta+1;//i+aux_delta+1;
						l[j]= sse128_packs_i16_u8(sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx), _128_32_minus_frac_delta), sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx+1), _128_frac_delta)),_128_round),5),_128_32);//_128_32 or whatever (is discarded)
					}
				}
				TRANSPOND_MATRIX_8x8_16BITS(l, ref_wnd, ref_wnd_stride_2D)
			}
			else
			{
				if(pred_angle==32)
				{
					for (j=0;j<cu_size;j++)
					{
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;

						for (i=0;i<cu_size;i+=8)
						{
							__m128_i16 aux = sse_128_load_vector_u(refMain+i+aux_delta+1);
							sse_128_store_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux,aux)));
						}
					}
				}
				else
				{
					for (j=0;j<cu_size;j++)
					{
						__m128_i16 aux;
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;
						_128_pos_delta=sse_128_add_i16(_128_pos_delta,_128_pred_angle);
						_128_frac_delta = sse_128_and(_128_pos_delta, _128_32_mask);
						_128_32_minus_frac_delta = sse_128_sub_i16 (_128_32,_128_frac_delta);

						ref_main_idx = aux_delta+1;//i+aux_delta+1;
						aux = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx), _128_32_minus_frac_delta), sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx+1), _128_frac_delta)),_128_round),5);
						sse_128_store_vector_u(ref_wnd+(j*ref_wnd_stride_2D), sse_128_convert_u8_i16(aux,aux));//8x8 y 4x4
					}
				}
			}
		}
	}
}
*/


void sse_create_intra_angular_prediction_4x4(henc_thread_t* et, ctu_info_t* ctu, uint16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_mode, int is_luma)//creamos el array de prediccion angular
{
	int i, j;
	int is_DC_mode = cu_mode < 2;
	int is_Hor_mode = !is_DC_mode && (cu_mode < 18);
	int is_Ver_mode = !is_DC_mode && !is_Hor_mode;
	int pred_angle = is_Ver_mode ? cu_mode - VER_IDX : is_Hor_mode ? -(cu_mode - HOR_IDX) : 0;
	//pred_angle = is_Ver_mode?cu_mode=[18,34]-26 = [-8,8]:modeHor?dirMode=[2,17]-10 =[8,-7]:0	
	//have to realize that -8 vertical = -8 horizontal (is the diagonal);//thats why there are 35 modes. 33 angular+dc+planar
	int abs_angle = abs(pred_angle);
	int sign_angle = SIGN(pred_angle);
	int16_t  *adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size);
	int inv_angle = et->ed->inv_ang_table[abs_angle];
	int bit_depth = et->bit_depth;
	int16_t *dst;
	int cu_size = 4; 
	abs_angle = et->ed->ang_table[abs_angle];
	pred_angle = sign_angle * abs_angle;

	// dc prediction
	if (is_DC_mode)
	{
		uint16_t dcval = sse_pred_intra_calc_dc(adi_ptr, cu_size, cu_size, ctu->top, ctu->left);
		
		__m128_u8 _128_dcval = sse_128_vector_i16((byte)dcval);

		dst = ref_wnd;
		for (j=0;j<cu_size;j++)
		{
			sse_64_storel_vector_u(dst, _128_dcval);
			dst+=ref_wnd_stride_2D;
		}

		if(cu_mode == DC_IDX && is_luma)
		{
			__m128_u8 _128_two = sse_128_vector_i16(2);
			__m128_u8 _128_three = sse_128_vector_i16(3);
			__m128_u8 aux, aux2;
			uint8_t ref0 = ((adi_ptr[-1] + adi_ptr[1] + 2*(int16_t)ref_wnd[0] + 2)>>2);//			ref_wnd[0] = ((adi_ptr[-1] + adi_ptr[1] + 2*(int16_t)ref_wnd[0] + 2)>>2);

			adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size) + 1;

			aux = sse_128_mul_i16(sse_128_load_vector_u(ref_wnd),_128_three);
			aux2 = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_load_vector_u(adi_ptr), aux), _128_two), 2);
			sse_64_storel_vector_u(ref_wnd, aux2);
			ref_wnd[0] = ref0;

	
			adi_ptr = ADI_POINTER_MIDDLE(adi_pred_buff, adi_size) - 1;
			for(j=1;j<cu_size;j++)		
			{
				ref_wnd[j*ref_wnd_stride_2D] = ((adi_ptr[-j] + 3*(int16_t)ref_wnd[j*ref_wnd_stride_2D] + 2)>>2);
			}
		}

	}
	else//angular predictions
	{
		int16_t* refMain;
		int16_t* refSide;
		int16_t	 top_pred_buff[64*4+1], left_pred_buff[64*4+1];
		int16_t  *refAbove = top_pred_buff;
		int16_t  *refLeft = left_pred_buff;//a este se accede de dcha a izq y equivale a ir de arriba a abajo
		int invAngleSum;
		int bFilter = is_luma?(cu_size<=16):0;
		__m128i shuff_mask = sse_128_load_vector_a(shuffle_mask_predict_16_0);//0,1,2,3,4,5,6,7 -> 7,6,5,4,3,2,1,0

		if (pred_angle < 0)
		{

			sse_128_store_vector_u(refAbove +(cu_size-1), sse_128_load_vector_u(adi_ptr));

			sse_128_store_vector_u(refLeft+(cu_size-1), sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7)), shuff_mask));

			refMain = (is_Ver_mode ? refAbove : refLeft) + (cu_size-1);
			refSide = (is_Ver_mode ? refLeft : refAbove) + (cu_size-1);

			invAngleSum    = 128;       // rounding for (shift by 8)
			for (i=-1; i>((cu_size*pred_angle)>>5); i--)
			{
				invAngleSum += inv_angle;
				refMain[i] = refSide[invAngleSum>>8];
			}
		}
		else	//pred_angle >= 0
		{

			sse_128_store_vector_u(refAbove, sse_128_load_vector_u(adi_ptr));
			refAbove[8] = adi_ptr[8];

			sse_128_store_vector_u(refLeft, sse_128_shuffle_8(sse_128_load_vector_u(adi_ptr-(7)), shuff_mask));
			refLeft[8] = adi_ptr[-8];

			refMain = is_Ver_mode ? refAbove : refLeft;
			refSide = is_Ver_mode ? refLeft  : refAbove;
		}

		if (pred_angle == 0)
		{
			int aux_stride = is_Hor_mode?1:ref_wnd_stride_2D;//if is_Hor_mode flip horizontal and vertical axis by changing the x and y stride
			int aux_stride2 = is_Hor_mode?ref_wnd_stride_2D:1;

			if(is_Hor_mode)
			{
				__m128_i16 l[4];

				for (j=0;j<cu_size;j++)
				{
					l[j] = sse_128_load_vector_u(refMain+1);//sse128_packs_i16_u8(aux,aux);
				}
				TRANSPOSE_MATRIX_4x4_16BITS(l, ref_wnd, ref_wnd_stride_2D)
			}
			else
			{
				for (i=0;i<cu_size;i+=8)
				{
					__m128_i16 aux = sse_128_load_vector_u(refMain+i+1);
					for (j=0;j<cu_size;j++)
					{
						sse_64_storel_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux,aux)));
					}
				}
			}
			

			if ( bFilter )
			{
				for (i=0;i<cu_size;i++)
				{
					ref_wnd[i*aux_stride] = clip(ref_wnd[i*aux_stride] + (( refSide[i+1] - refSide[0] ) >> 1) , 0, (1<<bit_depth)-1);
				}
			}
		}
		else
		{
			int aux_stride = is_Hor_mode?1:ref_wnd_stride_2D;//si is_Hor_mode le damos la vuelta (cambiando el stride de x e y)
			int aux_stride2 = is_Hor_mode?ref_wnd_stride_2D:1;
			int pos_delta=0;
			int aux_delta;
			int ref_main_idx;
			__m128_i16 _128_pos_delta=sse_128_zero_vector();
			__m128_i16 _128_pred_angle=sse_128_vector_i16(pred_angle);
			__m128_i16 _128_32 = sse_128_vector_i16(32);
			__m128_i16 _128_32_mask = sse_128_vector_i16(31);
			__m128_i16 _128_round = sse_128_vector_i16(16);
			__m128_i16 _128_frac_delta, _128_32_minus_frac_delta;

			if(is_Hor_mode)
			{
				__m128_i16 l[4];
				if(pred_angle==32)
				{
					for (j=0;j<cu_size;j++)
					{
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;

						l[j] = sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_load_vector_u(refMain+aux_delta+1),_128_32));//_128_32 or whatever (is discarded);
					}
				}
				else
				{
					for (j=0;j<cu_size;j++)
					{
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;
						_128_pos_delta=sse_128_add_i16(_128_pos_delta,_128_pred_angle);
						_128_frac_delta = sse_128_and(_128_pos_delta, _128_32_mask);
						_128_32_minus_frac_delta = sse_128_sub_i16 (_128_32,_128_frac_delta);

						ref_main_idx = aux_delta+1;//i+aux_delta+1;
						l[j]= sse_128_convert_u8_i16(sse128_packs_i16_u8(sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx), _128_32_minus_frac_delta), sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx+1), _128_frac_delta)),_128_round),5),_128_32));//_128_32 or whatever (is discarded)
					}
				}
				TRANSPOSE_MATRIX_4x4_16BITS(l, ref_wnd, ref_wnd_stride_2D)
			}
			else
			{
				if(pred_angle==32)
				{
					for (j=0;j<cu_size;j++)
					{
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;

						for (i=0;i<cu_size;i+=8)
						{
							__m128_i16 aux = sse_128_load_vector_u(refMain+i+aux_delta+1);
							sse_128_store_vector_u(ref_wnd+j*ref_wnd_stride_2D+i, sse_128_convert_u8_i16(sse128_packs_i16_u8(aux,aux)));
						}
					}
				}
				else
				{
					for (j=0;j<cu_size;j++)
					{
						__m128_i16 aux;
						pos_delta += pred_angle;
						aux_delta   = pos_delta >> 5;
						_128_pos_delta=sse_128_add_i16(_128_pos_delta,_128_pred_angle);
						_128_frac_delta = sse_128_and(_128_pos_delta, _128_32_mask);
						_128_32_minus_frac_delta = sse_128_sub_i16 (_128_32,_128_frac_delta);

						ref_main_idx = aux_delta+1;//i+aux_delta+1;
						aux = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx), _128_32_minus_frac_delta), sse_128_mul_i16(sse_128_load_vector_u(refMain+ref_main_idx+1), _128_frac_delta)),_128_round),5);
						sse_128_store_vector_u(ref_wnd+(j*ref_wnd_stride_2D), sse_128_convert_u8_i16(sse128_packs_i16_u8(aux,aux)));//8x8 y 4x4
					}
				}
			}
		}
	}
}


void sse_create_intra_angular_prediction(henc_thread_t* et, ctu_info_t* ctu, int16_t *ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_mode, int is_luma)
{
	if(cu_size==4)
		sse_create_intra_angular_prediction_4x4(et, ctu, ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size, cu_mode, is_luma);
	else
		sse_create_intra_angular_prediction_nxn(et, ctu, ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size, cu_size, cu_mode, is_luma);
/*	else if(cu_size==8)
		sse_create_intra_angular_prediction_8x8(et, ctu, ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size, cu_mode, is_luma);
	else
		sse_create_intra_angular_prediction_nxn(et, ctu, ref_wnd, ref_wnd_stride_2D, adi_pred_buff, adi_size, cu_size, cu_mode, is_luma);
*/
}


ALIGN(16) static const int8_t shuffle_adi_filter_16_0[16] ={  0,  0,  1,  0,  2,  0,  3,  0,  4,  0,  5,  0,  6,  0,  7,  0};//
ALIGN(16) static const int8_t shuffle_adi_filter_16_1[16] ={  7,  0,  6,  0,  5,  0,  4,  0,  3,  0,  2,  0,  1,  0,  0,  0};//


void sse_adi_filter(int16_t  *ptr, int16_t  *ptr_filter, int depth, int adi_size, int partition_size, int max_cu_size_shift, int intra_smooth_enable, int bit_depth)
{
	int i;
	int left_bottom = ptr[0];
	int left_top = ptr[2*partition_size];
	int top_right = ptr[adi_size-1];
	int threshold = 1 << (bit_depth - 5);
	int bilinear_left = abs(left_bottom+left_top-2*ptr[partition_size]) < threshold;
	int bilinear_above  = abs(left_top+top_right-2*ptr[2*partition_size+partition_size]) < threshold;

	if(intra_smooth_enable && (partition_size>=32 && (bilinear_left && bilinear_above)))
	{
		int16_t  *ptr_filter2 = ADI_POINTER_MIDDLE(ptr_filter, adi_size);
		int size_shift = max_cu_size_shift-depth + 1;
		__m128_i16 _128_eight = sse_128_vector_i16(8);
		__m128_i16 _128_partition_size = sse_128_vector_i16(partition_size);
		__m128_i16 _128_partition_size_x2 = sse_128_vector_i16(2*partition_size);
		__m128_i16 _128_left_bottom = sse_128_vector_i16(left_bottom);
		__m128_i16 _128_left_top = sse_128_vector_i16(left_top);
		__m128_i16 _128_top_right = sse_128_vector_i16(top_right);
		__m128_i16 _128_interpolator_up = sse_128_load_vector_a(shuffle_adi_filter_16_0);
		__m128_i16 _128_interpolator_down = sse_128_sub_i16(_128_partition_size_x2, sse_128_load_vector_a(shuffle_adi_filter_16_0));


		for (i = 0; i < 2*partition_size; i+=8)//left
		{
			__m128_i16 _128_aux = sse_128_mul_i16(_128_interpolator_down, _128_left_bottom);
			__m128_i16 _128_aux1 = sse_128_mul_i16(_128_interpolator_up, _128_left_top);
			_128_aux = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(_128_aux, _128_aux1),_128_partition_size),size_shift);
			sse_128_store_vector_a(&ptr_filter[i], _128_aux);
			_128_aux = sse_128_mul_i16(_128_interpolator_down, _128_left_top);
			_128_aux1 = sse_128_mul_i16(_128_interpolator_up, _128_top_right);
			_128_aux = sse_128_shift_r_i16(sse_128_add_i16(sse_128_add_i16(_128_aux, _128_aux1),_128_partition_size),size_shift);
			sse_128_store_vector_u(&ptr_filter2[i], _128_aux);

			_128_interpolator_down = sse_128_sub_i16(_128_interpolator_down, _128_eight);
			_128_interpolator_up = sse_128_add_i16(_128_interpolator_up, _128_eight);
		}
		ptr_filter[0] = ptr[0];
		ptr_filter[2*partition_size] = ptr[2*partition_size];
		ptr_filter[adi_size - 1] = ptr[adi_size - 1];
	}
	else
	{
		//filter [1,2,1]
		int aux = ptr_filter[0] = ptr[0];
		__m128_i16 _128_two = sse_128_vector_i16(2);

		for(i=1;i<adi_size-1;i+=8)	//column + square + row
		{
			__m128_i16 _128_aux = sse_128_add_i16(sse_128_load_vector_u(&ptr[i-1]), sse_128_mul_i16(_128_two, sse_128_load_vector_u(&ptr[i])));
			_128_aux = sse_128_add_i16(_128_aux,sse_128_load_vector_u(&ptr[i+1]));
			_128_aux = sse_128_shift_r_i16(sse_128_add_i16(_128_aux,_128_two),2);//rounding
			sse_128_store_vector_u(&ptr_filter[i], _128_aux);
		}
		ptr_filter[adi_size-1] = ptr[adi_size-1];
	}
}
