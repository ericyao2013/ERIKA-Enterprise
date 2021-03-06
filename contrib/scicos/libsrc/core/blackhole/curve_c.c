/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2006-2010  Simone Mannori, Roberto Bucher
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/** 
	@brief www.scicos.org, www.scicoslab.org
	@author Roberto Bucher, SUPSI- Lugano
	@author Simone Mannori, ScicosLab developer
	@date 2006-2010
*/
 
 
#include "scicos_block4.h"

/*    Masoud Najafi, August 2007 */
/*    Copyright INRIA
 *    Scicos block simulator
 *    Signal builder block
 */

#if WIN32
#define NULL    0
#endif

#define rpar     block->rpar 
#define nPoints  block->ipar[0]
#define Order    block->ipar[1]
#define Periodic block->ipar[2]
#define T        rpar[nPoints-1]-rpar[0]

int Myevalhermite(double *t, double *xa, double *xb, double *ya, double *yb, double *da, double *db, double *h, double *dh, double *ddh, double *dddh, int *i);

void curve_c(scicos_block *block,int flag)
{
  double t,a,b,c,y1,y2,t1,t2;
  int *ind,i,inow;
  double *y;
  double  d1,d2,h, dh, ddh, dddh;

  
  switch(flag)
  {
   /* init */
   case 4  : {/* the workspace is used to store discrete counter value */
              if ((*block->work=scicos_malloc(4*sizeof(int)))==NULL) {
                set_block_error(-16);
                return;
              }
              ind=*block->work;
	      ind[0]=nPoints-1;
	      ind[1]=nPoints;
	      for (i=0;i<nPoints;i++){
		if (rpar[i]>=0 ) {
		  ind[0]=i-1;
		  ind[1]=i;
		  break;
		}
	      }
	      ind[0]=-1;
	      ind[1]=0;
	      ind[2]=0; /* event index */
	      ind[3]=0; /* event counter */
	      return;

              break;
             }
   /* event date computation */
  case 1  : { 
              y=GetRealOutPortPtrs(block,1);
              ind=*block->work;
              t=get_scicos_time();
	      if (Periodic==1) {
		t=t-(ind[3]-1)*T;
	      }

	      inow=nPoints-1;
	      for (i=ind[0];i<nPoints;i++){		
		if (i==-1) continue;
		if (t<rpar[i]) {
		  inow=i-1;
		  if (inow<ind[1]){
		    ind[1]=inow;
		  }else{
		    ind[0]=ind[1];
		    ind[1]=inow;
		  }
		  break;
		}
	      }

	      if (inow<0) {y[0]=0.0;	break;}
	      if (inow>=nPoints-1) {y[0]=rpar[nPoints*2-1];break;}

	      if (Order==0) {
		y[0]=rpar[nPoints+inow];
		break;
	      }
	      if(Order==1) {
		t1=rpar[inow];
		t2=rpar[inow+1];
		y1=rpar[nPoints+inow];
		y2=rpar[nPoints+inow+1];
		y[0]=(y2-y1)*(t-t1)/(t2-t1)+y1;
		break;
	      }

	      if((Order==2)&&(nPoints>2)) {
		t1=rpar[inow];
		a=rpar[2*nPoints+inow];
		b=rpar[2*nPoints+inow+nPoints-1];
		c=rpar[2*nPoints+inow+2*nPoints-2];
		y[0]=a*(t-t1)*(t-t1)+b*(t-t1)+c;
		break;
	      }	     

	      if((Order>=3)) {
		t1=rpar[inow];
		t2=rpar[inow+1];
		y1=rpar[nPoints+inow];
		y2=rpar[nPoints+inow+1];
		d1=rpar[2*nPoints+inow];
		d2=rpar[2*nPoints+inow+1];
		Myevalhermite(&t, &t1,&t2, &y1,&y2, &d1,&d2, &h, &dh, &ddh, &dddh, &inow);
		y[0]=h;
		break;
	      }

              break;
             }
   /* event date computation */
  case 3  : {
              ind=*block->work;

	      /*---------*/
	      if ((Order==1)||(Order==0)){
		i=ind[2];
		if (i==nPoints-1){ 
		  if (Periodic==1) {
		    i=0;
		    ind[0]=-1;
		    ind[1]=0;
		  }
		}
		if (i<nPoints-1) {
		  block->evout[0]=rpar[i+1]-rpar[i];

		  ind[2]=i+1;
		}
		if (ind[2]==1)  ind[3]++;
	      }
	      /*-------------------*/
	      if (Order>=2){
		if ( Periodic) {
		  block->evout[0]=T;
		}else{
		  if (ind[3]==0) {
		    block->evout[0]=T;
		  }
		}
		ind[3]++;
		ind[0]=-1;
		ind[1]=0;
		    
	      }
	      break;
             }

    /* finish */
   case 5  : {
              scicos_free(*block->work); /*free the workspace*/
              break;
             }

   default : break;
  }
}

int Myevalhermite(double *t, double *xa, double *xb, double *ya, double *yb, double *da, double *db, double *h, double *dh, double *ddh, double *dddh, int *i)
{
  double tmxa, p, c2, c3, dx;

  /*    if (old_i != *i) {*/
/*        compute the following Newton form : */
/*           h(t) = ya + da*(t-xa) + c2*(t-xa)^2 + c3*(t-xa)^2*(t-xb) */
	dx = 1. / (*xb - *xa);
	p = (*yb - *ya) * dx;
	c2 = (p - *da) * dx;
	c3 = (*db - p + (*da - p)) * (dx * dx);
	/*	}	 old_i = *i;*/

/*     eval h(t), h'(t), h"(t) and h"'(t), by a generalised Horner 's scheme */
    tmxa = *t - *xa;
    *h = c2 + c3 * (*t - *xb);
    *dh = *h + c3 * tmxa;
    *ddh = (*dh + c3 * tmxa) * 2.;
    *dddh = c3 * 6.;
    *h = *da + *h * tmxa;
    *dh = *h + *dh * tmxa;
    *h = *ya + *h * tmxa;
    return 0; 
} /* evalhermite_ */

