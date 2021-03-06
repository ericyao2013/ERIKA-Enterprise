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
 
 
# include "scicos_block4.h"
# include "machine.h"
#include <stdio.h>
#include <math.h>
extern int C2F(zlacpy)();
extern int C2F(zgetrf)();
extern double C2F(dlamch)();
extern double C2F(zlange)();
extern int C2F(zgecon)();
extern int C2F(zgetrs)();
extern int C2F(zgelsy1)();

#if WIN32
#define NULL    0
#endif

#ifndef min
#define min(a,b) ((a) <= (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif

typedef struct
{         int *ipiv;
          int *rank;
          int *jpvt;
          double *iwork;
          double *dwork;
	  double *IN1F;
	  double *IN1;
	  double *IN2X;
	  double *IN2;
} mat_bksl_struct ;
void matz_bksl(scicos_block *block,int flag)
{
 double *u1r,*u1i;
 double *u2r,*u2i;
 double *yr,*yi;
 int mu,vu,wu;
 int nu1;
 int nu2;
 int info;
 int i,j,l,lw,lu,rw,ij,k;
 mat_bksl_struct *ptr;
 double rcond, ANORM, EPS;
 vu =GetOutPortRows(block,1);
 wu =GetOutPortCols(block,1);
 mu =GetInPortRows(block,1);
 nu1 =GetInPortCols(block,1);
 nu2 =GetInPortCols(block,2);
 u1r=GetRealInPortPtrs(block,1);
 u1i=GetImagInPortPtrs(block,1);
 u2r=GetRealInPortPtrs(block,2);
 u2i=GetImagInPortPtrs(block,2);
 yr=GetRealOutPortPtrs(block,1);
 yi=GetImagOutPortPtrs(block,1);
 l=max(mu,nu1);
 lw=max(2*min(mu,nu1),nu1+1);
 lu=max(lw,min(mu,nu1)+nu2);
 lw=max(2*nu1,min(mu,nu1)+lu);
 rw=2*nu1;
             /*init : initialization*/
if (flag==4)
   {if((*(block->work)=(mat_bksl_struct*) scicos_malloc(sizeof(mat_bksl_struct)))==NULL)
	{set_block_error(-16);
	 return;}
    ptr=*(block->work);
    if((ptr->ipiv=(int*) scicos_malloc(sizeof(int)*nu1))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr);
	 return;}
    if((ptr->rank=(int*) scicos_malloc(sizeof(int)))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->jpvt=(int*) scicos_malloc(sizeof(int)*nu1))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->iwork=(double*) scicos_malloc(sizeof(double)*2*nu1))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->jpvt);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->dwork=(double*) scicos_malloc(sizeof(double)*2*lw))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->iwork);
	 scicos_free(ptr->jpvt);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->IN1F=(double*) scicos_malloc(sizeof(double)*(2*mu*nu1)))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->dwork);
	 scicos_free(ptr->iwork);
	 scicos_free(ptr->jpvt);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->IN1=(double*) scicos_malloc(sizeof(double)*(2*mu*nu1)))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->IN1F);
	 scicos_free(ptr->dwork);
	 scicos_free(ptr->iwork);
	 scicos_free(ptr->jpvt);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->IN2X=(double*) scicos_malloc(sizeof(double)*(2*l*nu2)))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->IN1);
	 scicos_free(ptr->IN1F);
	 scicos_free(ptr->dwork);
	 scicos_free(ptr->iwork);
	 scicos_free(ptr->jpvt);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
    if((ptr->IN2=(double*) scicos_malloc(sizeof(double)*(2*mu*nu2)))==NULL)
	{set_block_error(-16);
	 scicos_free(ptr->IN2);
	 scicos_free(ptr->IN1);
	 scicos_free(ptr->IN1F);
	 scicos_free(ptr->dwork);
	 scicos_free(ptr->iwork);
	 scicos_free(ptr->jpvt);
	 scicos_free(ptr->rank);
	 scicos_free(ptr->ipiv);
	 scicos_free(ptr);
	 return;}
   }

       /* Terminaison */
else if (flag==5)
   {ptr=*(block->work);
    if((ptr->IN2)!=NULL){
    	scicos_free(ptr->ipiv);
    	scicos_free(ptr->rank);
    	scicos_free(ptr->jpvt);
    	scicos_free(ptr->iwork);
    	scicos_free(ptr->IN1F);
    	scicos_free(ptr->IN1);
    	scicos_free(ptr->IN2X);
    	scicos_free(ptr->IN2);
    	scicos_free(ptr->dwork);
    	scicos_free(ptr);
    	return;}
   }

else
   {
    ptr=*(block->work);
    for (i=0;i<(mu*nu1);i++)   
	{ptr->IN1[2*i]=u1r[i];
	 ptr->IN1[2*i+1]=u1i[i];}
    for (i=0;i<(mu*nu2);i++)   
	{ptr->IN2[2*i]=u2r[i];
	 ptr->IN2[2*i+1]=u2i[i];}
    EPS=C2F(dlamch)("e",1L);
    ANORM=C2F(zlange)("1",&mu,&nu1,ptr->IN1,&mu,ptr->dwork);
    if (mu==nu1)
	{C2F(zlacpy)("F",&mu,&nu1,ptr->IN1,&mu,ptr->IN1F,&mu);
	 C2F(zgetrf)(&nu1,&nu1,ptr->IN1F,&nu1,ptr->ipiv,&info);
	 rcond=0;
 	 if (info==0)
	    {C2F(zgecon)("1",&nu1,ptr->IN1F,&nu1,&ANORM,&rcond,ptr->dwork,ptr->iwork,&info);
	     if (rcond>pow(EPS,0.5))
		{
		 C2F(zgetrs)("N",&nu1,&nu2,ptr->IN1F,&nu1,ptr->ipiv,ptr->IN2,&nu1,&info);
		 for (i=0;i<(mu*nu2);i++)
	   	 {*(yr+i)=*(ptr->IN2+2*i);
	    	  *(yi+i)=*(ptr->IN2+(2*i)+1);}
		 return;
		}
	    }
	}
    rcond=pow(EPS,0.5);
    for (i=0;i<nu1;i++)    *(ptr->jpvt+i)=0;
    C2F(zlacpy)("F",&mu,&nu2,ptr->IN2,&mu,ptr->IN2X,&l);
    C2F(zgelsy1)(&mu,&nu1,&nu2,ptr->IN1,&mu,ptr->IN2X,&l,ptr->jpvt,&rcond,ptr->rank,ptr->dwork,&lw,ptr->iwork,&info);
    if (info!=0)
	{if (flag!=6)
	    {set_block_error(-7);
             return;
	    }
	}
	k=0;
	for(j=0;j<wu;j++)
	    {for(i=0;i<vu;i++)
		{ij=i+j*l;
		 *(yr+k)=*(ptr->IN2X+2*ij);
	 	 *(yi+k)=*(ptr->IN2X+(2*ij)+1);
		k++;}}
    }
}
