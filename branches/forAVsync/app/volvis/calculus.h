/********************************************************************************
 * Volatile - Volume Visualization Software for SAGE
 * Copyright (C) 2004 Electronic Visualization Laboratory,
 * University of Illinois at Chicago
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the University of Illinois at Chicago nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Direct questions, comments etc about Volatile to www.evl.uic.edu/cavern/forum
 *********************************************************************************/

#ifndef _CALCULUS_h
#define _CALCULUS_h
//======================== calculus ===================================

//standard scalar derivative measure
inline derivative3D(unsigned char *magV1,
					float *gradV3,
					const int sx, const int sy, const int sz,
					const unsigned char *dat)
{
	float *tmp = new float[sx*sy*sz];
	float maxm = 0;
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<2)||(k>sx-3)||(j<2)||(j>sy-3)||(i<2)||(i>sz-3)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					tmp[i*sx*sy + j*sx + k] = 0;
				}
				else {
					int dx = dat[i*sx*sy + j*sx + (k+1)] - dat[i*sx*sy + j*sx + (k-1)];
					int dy = dat[i*sx*sy + (j+1)*sx + k] - dat[i*sx*sy + (j-1)*sx + k];
					int dz = dat[(i+1)*sx*sy + j*sx + k] - dat[(i-1)*sx*sy + j*sx + k];
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (float)dx;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (float)dy;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (float)dz;
					tmp[i*sx*sy + j*sx + k] = (float)sqrt(dx*dx + dy*dy + dz*dz);
					maxm = maxm < tmp[i*sx*sy + j*sx + k] ? tmp[i*sx*sy + j*sx + k] : maxm;
				}
			}
		}
	}
	for( i = 0; i < sz; ++i){
		for(int j = 0; j < sy; ++j){
			for(int k = 0; k < sx; ++k){
				magV1[i*sx*sy + j*sx + k] = (unsigned char)((tmp[i*sx*sy + j*sx + k]/maxm)*255);
			}
		}
	}
	delete[] tmp;
}


//standard scalar derivative measure - vgh volume (value grad hessian)
inline derivative3DVGH(float *gradV3,
					   const int sx, const int sy, const int sz,
					   const unsigned char *dat)
{
	//cerr << "hi!!!!!!!!!! " << sz << " " << sy << " " << sx << endl;
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
				}
				else {
					int dx = dat[i*sx*sy*3     + j*sx*3     +(k+1)*3] - dat[i*sx*sy*3     + j*sx*3     + (k-1)*3];
					int dy = dat[i*sx*sy*3     + (j+1)*sx*3 + k*3]    - dat[i*sx*sy*3     + (j-1)*sx*3 + k*3];
					int dz = dat[(i+1)*sx*sy*3 + j*sx*3     + k*3]    - dat[(i-1)*sx*sy*3 + j*sx*3     + k*3];
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (float)dx;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (float)dy;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (float)dz;
				}
			}
		}
	}
}

inline void addDer(unsigned char *magV1, float *gradV3,
				   const int sx, const int sy, const int sz,
				   const unsigned char *dat1, const unsigned char *dat2)
{
	float *tmp = new float[sx*sy*sz];
	float maxm = 0;
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<2)||(k>sx-3)||(j<2)||(j>sy-3)||(i<2)||(i>sz-3)){
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					tmp[i*sx*sy + j*sx + k] = 0;
				}
				else {
					int dx1 = dat1[i*sx*sy + j*sx + (k+1)] - dat1[i*sx*sy + j*sx + (k-1)];
					int dy1 = dat1[i*sx*sy + (j+1)*sx + k] - dat1[i*sx*sy + (j-1)*sx + k];
					int dz1 = dat1[(i+1)*sx*sy + j*sx + k] - dat1[(i-1)*sx*sy + j*sx + k];

					int dx2 = dat2[i*sx*sy + j*sx + (k+1)] - dat2[i*sx*sy + j*sx + (k-1)];
					int dy2 = dat2[i*sx*sy + (j+1)*sx + k] - dat2[i*sx*sy + (j-1)*sx + k];
					int dz2 = dat2[(i+1)*sx*sy + j*sx + k] - dat2[(i-1)*sx*sy + j*sx + k];

					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = (float)(dx1 + dx2);
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = (float)(dy1 + dy2);
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = (float)(dz1 + dz2);
					tmp[i*sx*sy + j*sx + k] = (float)sqrt((dx1+dx2)*(dx1+dx2) + (dy1+dy2)*(dy1+dy2) + (dz1+dz2)*(dz1+dz2));
					maxm = maxm < tmp[i*sx*sy + j*sx + k] ? tmp[i*sx*sy + j*sx + k] : maxm;
				}
			}
		}
	}
	for( i = 0; i < sz; ++i){
		for(int j = 0; j < sy; ++j){
			for(int k = 0; k < sx; ++k){
				magV1[i*sx*sy + j*sx + k] = (unsigned char)((tmp[i*sx*sy + j*sx + k]/maxm)*255);
			}
		}
	}
	delete[] tmp;
}

template<class T>                     //compute an additive gradient
inline
int AGradArb(float *gradV3,           //put it in here, create your data first
			 int sx, int sy, int sz,  //size of each axis
			 int n,                   //first n elements of data to compute grad with
			 int elts,                //number of elements in the data
			 T *data)                 //the data
{
#if 0
	T **minmax = new T*[elts];
	for(int e=0; e<elts; ++e){
		minmax[e] = new T[2];
		minmax[e][0] = (T)10000000000;
		minmax[e][1] = (T)-10000000000;
	}

	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				for(int e=0; e<n; ++e){
					minmax[e][0] = MIN(data[i*sx*sy*elts + j*sx*elts + (k+1)*elts + e], minmax[e][0]);
					minmax[e][1] = MAX(data[i*sx*sy*elts + j*sx*elts + (k+1)*elts + e], minmax[e][1]);
				}
			}
		}
	}

	for(e=0; e<elts; ++e){
		cerr << "Volume " << e << " , min: " << (float)minmax[e][0] << "   max: " << (float)minmax[e][1] << endl;
	}
#endif

	for(int i = 0; i < sz; ++i)
	{
		for(int j = 0; j < (sy); ++j)
		{
			for(int k = 0; k < (sx); ++k)
			{
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2))
				{
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
				}
				else
				{
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] = 0;
					gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] = 0;
					for(int e=0; e<n; ++e)
					{
						gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0] += data[i*sx*sy*elts + j*sx*elts + (k+1)*elts + e] -
							data[i*sx*sy*elts + j*sx*elts + (k-1)*elts + e];
						gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1] += data[i*sx*sy*elts + (j+1)*sx*elts + k*elts + e] -
							data[i*sx*sy*elts + (j-1)*sx*elts + k*elts + e];
						gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2] += data[(i+1)*sx*sy*elts + j*sx*elts + k*elts + e] -
							data[(i-1)*sx*sy*elts + j*sx*elts + k*elts + e];
					}
				}
			}
		}
	}

	return 0;
}

inline
void GMag(unsigned char *gmag, int sx, int sy, int sz, float *grad){
	float maxm = 0;
	float *tmp = new float[sx*sy*sz];
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				tmp[i*sx*sy + j*sx + k] = normV3(&grad[i*sx*sy*3 + j*sx*3 + k*3]);
				maxm = MAX(tmp[i*sx*sy + j*sx + k], maxm);
			}
		}
	}
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				gmag[i*sx*sy + j*sx + k] = (unsigned char)(tmp[i*sx*sy + j*sx + k]/maxm * 255.0);
			}
		}
	}
	delete[] tmp;
}

inline
void GMagHess(unsigned char *gmag, unsigned char *hess, int sx, int sy, int sz, float *gradV3){

	float maxm = 0;
	//find the gradient magnitude and it's max val
	float *tmpg = new float[sx*sy*sz];
	for(int i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				tmpg[i*sx*sy + j*sx + k] = normV3(&gradV3[i*sx*sy*3 + j*sx*3 + k*3]);
				maxm = MAX(tmpg[i*sx*sy + j*sx + k], maxm);
			}
		}
	}

	float hmax = -100000000;
	float hmin = 100000000;
	float *tmph = new float[sx*sy*sz];

	//compute the 2nd derivative in the gradient direction
	//  a mask would probably help out here??
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				if((k<1)||(k>sx-2)||(j<1)||(j>sy-2)||(i<1)||(i>sz-2)){
					tmph[i*sx*sy + j*sx + k] = 0;
				}
				else {
					float h[9];
					h[0] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 0] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 0];
					h[1] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 0] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 0];
					h[2] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 0] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 0];
					h[3] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 1] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 1];
					h[4] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 1] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 1];
					h[5] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 1] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 1];
					h[6] = gradV3[i*sx*sy*3 + j*sx*3 + (k+1)*3 + 2] - gradV3[i*sx*sy*3 + j*sx*3 + (k-1)*3 + 2];
					h[7] = gradV3[i*sx*sy*3 + (j+1)*sx*3 + k*3 + 2] - gradV3[i*sx*sy*3 + (j-1)*sx*3 + k*3 + 2];
					h[8] = gradV3[(i+1)*sx*sy*3 + j*sx*3 + k*3 + 2] - gradV3[(i-1)*sx*sy*3 + j*sx*3 + k*3 + 2];
					float tv[3];
					float tg[3] = {gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 0]/tmpg[i*sx*sy + j*sx + k],
						gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 1]/tmpg[i*sx*sy + j*sx + k],
						gradV3[i*sx*sy*3 + j*sx*3 + k*3 + 2]/tmpg[i*sx*sy + j*sx + k]};

					tv[0] = tg[0] * h[0] +
						tg[1] * h[1] +
						tg[2] * h[2];
					tv[1] = tg[0] * h[3] +
						tg[1] +
						tg[2] * h[5];
					tv[2] = tg[0] * h[6] +
						tg[1] * h[7] +
						tg[2] * h[8];
					tmph[i*sx*sy + j*sx + k] = tg[0] * tv[0] +
						tg[1] * tv[1] +
						tg[2] * tv[2];
					hmax = MAX(hess[i*sx*sy + j*sx + k], hmax);
					hmin = MIN(hess[i*sx*sy + j*sx + k], hmin);

				}
			}
		}
	}

	//now quantize to chars
	for(i = 0; i < sz; ++i){
		for(int j = 0; j < (sy); ++j){
			for(int k = 0; k < (sx); ++k){
				gmag[i*sx*sy + j*sx + k] = (unsigned char)(tmpg[i*sx*sy + j*sx + k]/maxm * 255.0);
				if(hess[i*sx*sy + j*sx + k] < 0){
					float th = (float)affine(hmin, tmph[i*sx*sy + j*sx + k], 0, 0, 1);
					//th = (float)(1.0-((1.0-th)*(1.0-th)*(1.0-th)));
					hess[i*sx*sy + j*sx + k] = (unsigned char)affine(0,th,1, 0, 255/3);
				} else {
					float th = (float)affine(0, tmph[i*sx*sy + j*sx + k], hmax, 0, 1);
					//th = (float)(1.0-((1.0-th)*(1.0-th)*(1.0-th)));
					hess[i*sx*sy + j*sx + k] = (unsigned char)affine(0,th,1,255/3,255/3*2);
				}
			}
		}
	}
	delete[] tmpg;
	delete[] tmph;
}

//float vector scale and bias to 8bit unsigned char
inline void scalebias(unsigned char *ucgradV3, float *gradV3, int sx, int sy, int sz){
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				ucgradV3[i*stz + j*sty + k*stx + 0] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 0]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 1] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 1]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 2] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 2]*128 + 128);
			}
		}
	}

}

//float vector normalize, scale, and bias to 8bit unsigned char
inline void scalebiasN(unsigned char *ucgradV3, float *gradV3, int sx, int sy, int sz){
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				normalizeV3(&gradV3[i*stz + j*sty + k*stx + 0]);
				ucgradV3[i*stz + j*sty + k*stx + 0] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 0]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 1] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 1]*128 + 128);
				ucgradV3[i*stz + j*sty + k*stx + 2] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 2]*128 + 128);
			}
		}
	}

}

inline void scaleBiasHiLoUC(unsigned char *ucgradV2, float *gradV3, int sx, int sy, int sz){
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				normalizeV3(&gradV3[i*stz + j*sty + k*stx + 0]);
				if(gradV3[i*stz + j*sty + k*stx + 2] < 0){
					gradV3[i*stz + j*sty + k*stx + 0] = -gradV3[i*stz + j*sty + k*stx + 0];
					gradV3[i*stz + j*sty + k*stx + 1] = -gradV3[i*stz + j*sty + k*stx + 1];
				}
				ucgradV2[i*sx*sy*2 + j*sx*2 + k*2 + 0] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 0]*128 + 128);
				ucgradV2[i*sx*sy*2 + j*sx*2 + k*2 + 1] = (unsigned char)(gradV3[i*stz + j*sty + k*stx + 1]*128 + 128);
			}
		}
	}

}

//copy scale and add vector

inline void csaddV3(GLfloat out[3], float s, GLfloat in[3])
{
	out[0] += s*in[0];
	out[1] += s*in[1];
	out[2] += s*in[2];
}

inline void csaddVARB(GLfloat out[3], float s, GLfloat in[3], int elts)
{
	for(int e=0; e<elts; ++e){
		out[e] += s*in[e];
	}
}

inline void csaddVARB(GLfloat out[3], float s, unsigned char in[3], int elts)
{
	for(int e=0; e<elts; ++e){
		out[e] += (float)(s*(in[e]/255.0));
	}
}

//blurr a 3D 3 vector field using these weights
//				------------
//       /w3 /w2 /w3 /
//      /-----------/
//	   /w2 /w1 /w2 /        <-- z+1
//    /-----------/
//   /w3 /w2 /w3 /
//  /-----------/
//				------------
//       /w2 /w1 /w2 / y+1
//      /-----------/
//	   /w1 /w0 /w1 / y      <-- z
//    /-----------/
//   /w2 /w1 /w2 / y-1
//  /-----------/
//				------------
//       /w3 /w2 /w3 /
//      /-----------/
//	   /w2 /w1 /w2 /        <-- z-1
//    /-----------/
//   /w3 /w2 /w3 /
//  /-----------/
//   x-1  x  x+1

inline blurV3D(float *gradV3, float w0, float w1, float w2, float w3,
			   int sx, int sy, int sz){
	float *tmp = new float[sx*sy*sz*3];
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				zeroV3(&tmp[i*stz + j*sty + k*stx + 0]);
			}
		}
	}

	for(i = 1; i<sz-1; ++i){
		for(int j = 1; j<sy-1; ++j){
			for(int k = 1; k<sx-1; ++k){
				int idx = i*stz + j*sty + k*stx + 0;
				csaddV3(&tmp[i*stz + j*sty + k*stx + 0], w0, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + j*sty + k*stx + 0], w1, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

				csaddV3(&tmp[(i-1)*stz + j*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[(i-1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

				csaddV3(&tmp[i*stz + (j+1)*sty + k*stx + 0], w1, &gradV3[idx]);
				csaddV3(&tmp[i*stz + (j+1)*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[i*stz + (j+1)*sty + (k-1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[i*stz + (j-1)*sty + k*stx + 0], w1, &gradV3[idx]);
				csaddV3(&tmp[i*stz + (j-1)*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[i*stz + (j-1)*sty + (k-1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[i*stz + j*sty + (k+1)*stx + 0], w1, &gradV3[idx]);
				csaddV3(&tmp[i*stz + j*sty + (k-1)*stx + 0], w1, &gradV3[idx]);
			}
		}
	}

	float div = w0 + 6*w1 + 12*w2 + 8*w3;

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				gradV3[i*stz + j*sty + k*stx + 0] = tmp[i*stz + j*sty + k*stx + 0]/div;
				gradV3[i*stz + j*sty + k*stx + 1] = tmp[i*stz + j*sty + k*stx + 1]/div;
				gradV3[i*stz + j*sty + k*stx + 2] = tmp[i*stz + j*sty + k*stx + 2]/div;
			}
		}
	}

	delete[] tmp;
}

inline blurVARB(unsigned char *dataV, float w0, float w1, float w2, float w3,
				int sx, int sy, int sz, int elts){

	//cerr << "BLUR :   " << sx << "  " << sy  << "  " << sz << " * " << elts << endl;

	float *tmp = new float[sx*sy*sz*elts];
	int stx = elts;
	int sty = sx*elts;
	int stz = sy*sx*elts;
	//cerr << "   stride " << stx << " " << sty << " " << stz << endl;



	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				for(int e = 0; e< elts; ++e){
					tmp[i*stz + j*sty + k*stx + e] = 0.0;
				}
			}
		}
	}

	for(i = 1; i<sz-1; ++i){
		for(int j = 1; j<sy-1; ++j){
			for(int k = 1; k<sx-1; ++k){
				for(int e = 0; e<elts; ++ e){
					int idx = i*stz + j*sty + k*stx + e;
					float v0 = (float)(dataV[idx]/255.0*w0);
					float v1 = (float)(dataV[idx]/255.0*w1/6.0);
					float v2 = (float)(dataV[idx]/255.0*w2/12.0);
					float v3 = (float)(dataV[idx]/255.0*w3/8.0);

					tmp[i*stz + j*sty + k*stx + e] += v0; //1
					tmp[(i+1)*stz + j*sty + k*stx + e] += v1; //1
					tmp[(i+1)*stz + (j+1)*sty + k*stx + e] += v2; //1
					tmp[(i+1)*stz + (j+1)*sty + (k+1)*stx + e] += v3;	//1
					tmp[(i+1)*stz + (j+1)*sty + (k-1)*stx + e] += v3;	//2
					tmp[(i+1)*stz + (j-1)*sty + k*stx + e] += v2; //2
					tmp[(i+1)*stz + (j-1)*sty + (k+1)*stx + e] += v3; //3
					tmp[(i+1)*stz + (j-1)*sty + (k-1)*stx + e] += v3; //4
					tmp[(i+1)*stz + j*sty + (k+1)*stx + e] += v2; //3
					tmp[(i+1)*stz + j*sty + (k-1)*stx + e] += v2; //4

					tmp[(i-1)*stz + j*sty + k*stx + e] += v1; //2
					tmp[(i-1)*stz + (j+1)*sty + k*stx + e] += v2; //5
					tmp[(i-1)*stz + (j+1)*sty + (k+1)*stx + e] += v3;	//5
					tmp[(i-1)*stz + (j+1)*sty + (k-1)*stx + e] += v3;	//6
					tmp[(i-1)*stz + (j-1)*sty + k*stx + e] += v2; //6
					tmp[(i-1)*stz + (j-1)*sty + (k+1)*stx + e] += v3; //7
					tmp[(i-1)*stz + (j-1)*sty + (k-1)*stx + e] += v3; //8
					tmp[(i-1)*stz + j*sty + (k+1)*stx + e] += v2; //7
					tmp[(i-1)*stz + j*sty + (k-1)*stx + e] += v2; //8

					tmp[i*stz + (j+1)*sty + k*stx + e] += v1; //3
					tmp[i*stz + (j+1)*sty + (k+1)*stx + e] += v2; //9
					tmp[i*stz + (j+1)*sty + (k-1)*stx + e] += v2; //10
					tmp[i*stz + (j-1)*sty + k*stx + e] += v1; //4
					tmp[i*stz + (j-1)*sty + (k+1)*stx + e] += v2; //11
					tmp[i*stz + (j-1)*sty + (k-1)*stx + e] += v2; //12
					tmp[i*stz + j*sty + (k+1)*stx + e] += v1; //5
					tmp[i*stz + j*sty + (k-1)*stx + e] += v1; //6
				}
			}
		}
	}

	//float div = w0 + 6*w1 + 12*w2 + 8*w3;
	float div = w0 + w1 + w2 + w3;

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				for(int e =0; e<elts; ++e){
					dataV[i*stz + j*sty + k*stx + e] = (unsigned char)(CLAMP((tmp[i*stz + j*sty + k*stx + e]/div))*255);
				}
			}
		}
	}

	delete[] tmp;  //why does this explode?????
}

//same as above but normalize too

inline blurV3DN(float *gradV3, float w0, float w1, float w2, float w3,
				int sx, int sy, int sz){
	float *tmp = new float[sx*sy*sz*3];
	int stx = 3;
	int sty = sx*3;
	int stz = sy*sx*3;
	for(int i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				zeroV3(&tmp[i*stz + j*sty + k*stx + 0]);
			}
		}
	}

	for(i = 1; i<sz-1; ++i){
		for(int j = 1; j<sy-1; ++j){
			for(int k = 1; k<sx-1; ++k){
				int idx = i*stz + j*sty + k*stx + 0;
				csaddV3(&tmp[i*stz + j*sty + k*stx + 0], w0, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + j*sty + k*stx + 0], w1, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i+1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

				csaddV3(&tmp[(i-1)*stz + j*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[(i-1)*stz + (j+1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j+1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j-1)*sty + k*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k+1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + (j-1)*sty + (k-1)*stx + 0], w3, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + j*sty + (k+1)*stx + 0], w2, &gradV3[idx]);
				csaddV3(&tmp[(i-1)*stz + j*sty + (k-1)*stx + 0], w2, &gradV3[idx]);

				csaddV3(&tmp[i*stz + (j+1)*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + (j+1)*sty + (k+1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + (j+1)*sty + (k-1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + (j-1)*sty + k*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + (j-1)*sty + (k+1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + (j-1)*sty + (k-1)*stx + 0], w2, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + j*sty + (k+1)*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);
				csaddV3(&tmp[i*stz + j*sty + (k-1)*stx + 0], w1, &gradV3[i*stz + j*sty + k*stx + 0]);

			}
		}
	}

	float div = w0 + 6*w1 + 12*w2 + 8*w3;

	for(i = 0; i<sz; ++i){
		for(int j = 0; j<sy; ++j){
			for(int k = 0; k<sx; ++k){
				scaleV3(1/div, &tmp[i*stz + j*sty + k*stx + 0]);
				normalizeV3(&tmp[i*stz + j*sty + k*stx + 0]);
				copyV3(&gradV3[i*stz + j*sty + k*stx + 0], &tmp[i*stz + j*sty + k*stx + 0]);
			}
		}
	}

	delete[] tmp;

}
#endif
