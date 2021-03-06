/************************************************************************
 *
 *  coder.c, main coding engine of tmn (TMN encoder)
 *  Copyright (C) 1995, 1996  Telenor R&D, Norway
 *        Karl Olav Lillevold <Karl.Lillevold@nta.no>
 *  
 *  Contacts: 
 *  Karl Olav Lillevold               <Karl.Lillevold@nta.no>, or
 *  Robert Danielsen                  <Robert.Danielsen@nta.no>
 *
 *  Telenor Research and Development  http://www.nta.no/brukere/DVC/
 *  P.O.Box 83                        tel.:   +47 63 84 84 00
 *  N-2007 Kjeller, Norway            fax.:   +47 63 81 00 76
 *  
 ************************************************************************/

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any
 * license fee or royalty on an "as is" basis.  Telenor Research and
 * Development disclaims any and all warranties, whether express,
 * implied, or statuary, including any implied warranties or
 * merchantability or of fitness for a particular purpose.  In no
 * event shall the copyright-holder be liable for any incidental,
 * punitive, or consequential damages of any kind whatsoever arising
 * from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs
 * and user's customers, employees, agents, transferees, successors,
 * and assigns.
 *
 * Telenor Research and Development does not represent or warrant that
 * the programs furnished hereunder are free of infringement of any
 * third-party patents.
 *
 * Commercial implementations of H.263, including shareware, are
 * subject to royalty fees to patent holders.  Many of these patents
 * are general enough such that they are unavoidable regardless of
 * implementation design.
 * */


/*****************************************************************
 *
 * Modified by Pat Mulroy, BT Labs to run syntax based arithmetic
 * coding.  SAC option, H.263 (Annex E).
 *
 *****************************************************************/


#include"sim.h"


/**********************************************************************
 *
 *	Name:        CodeOneOrTwo
 *	Description:	code one image normally or two images
 *                      as a PB-frame (CodeTwoPB and CodeOnePred merged)
 *	
 *	Input:        pointer to image, prev_image, prev_recon, Q
 *        
 *	Returns:	pointer to reconstructed image
 *	Side effects:	memory is allocated to recon image
 *
 *	Date: 950221	Author:	Karl.Lillevold@nta.no
 *
 ***********************************************************************/


void CodeOneOrTwo(PictImage *curr, PictImage *B_image, PictImage *prev, 
          PictImage *pr, int QP, int frameskip, Bits *bits,
          Pict *pic, PictImage *B_recon, PictImage *recon)
{
  unsigned char *prev_ipol,*pi_edge=NULL,*pi,*orig_lum;
  PictImage *prev_recon=NULL, *pr_edge=NULL;
  MotionVector *MV[6][MBR+1][MBC+2];
  MotionVector ZERO = {0,0,0,0,0};
  MB_Structure *recon_data_P; 
  MB_Structure *recon_data_B=NULL; 
  MB_Structure *diff; 
  int *qcoeff_P;
  int *qcoeff_B=NULL;

  int Mode,B;  
  int CBP, CBPB=0;
  int bquant[] = {5,6,7,8};
  int QP_B;
  int newgob;

  int i,j,k;

  /* buffer control vars */
  float QP_cumulative = (float)0.0;
  int abs_mb_num = 0, QuantChangePostponed = 0;
  int QP_new, QP_prev, dquant, QP_xmitted=QP;

  ZeroBits(bits);

  /* interpolate image */
  if (mv_outside_frame) {
    if (long_vectors) {
      /* If the Extended Motion Vector range is used, motion vectors
         may point further out of the picture than in the normal range,
         and the edge images will have to be made larger */
      B = 16;
    }
    else {
      /* normal range */
      B = 8;
    }
    pi_edge = (unsigned char *)malloc(sizeof(char)*(pels+4*B)*(lines+4*B));
    if (pi_edge == NULL) {
      printf("stderr:Couldn't allocate memory for pi_edge\n");
      exit(-1);
    }
    MakeEdgeImage(pr->lum,pi_edge + (pels + 4*B)*2*B+2*B,pels,lines,2*B);
    pi = InterpolateImage(pi_edge, pels+4*B, lines+4*B);
    free(pi_edge);
    prev_ipol = pi + (2*pels + 8*B) * 4*B + 4*B; 
    
    /* luma of non_interpolated image */
    pr_edge = InitImage((pels+4*B)*(lines+4*B));
    MakeEdgeImage(prev->lum, pr_edge->lum + (pels + 4*B)*2*B+2*B,
          pels,lines,2*B);
    orig_lum = pr_edge->lum + (pels + 4*B)*2*B+2*B;
    
    /* non-interpolated image */
    MakeEdgeImage(pr->lum,pr_edge->lum + (pels+4*B)*2*B + 2*B,pels,lines,2*B);
    MakeEdgeImage(pr->Cr,pr_edge->Cr + (pels/2 + 2*B)*B + B,pels/2,lines/2,B);
    MakeEdgeImage(pr->Cb,pr_edge->Cb + (pels/2 + 2*B)*B + B,pels/2,lines/2,B);
    
    prev_recon = (PictImage *)malloc(sizeof(PictImage));
    prev_recon->lum = pr_edge->lum + (pels + 4*B)*2*B + 2*B;
    prev_recon->Cr = pr_edge->Cr + (pels/2 + 2*B)*B + B;
    prev_recon->Cb = pr_edge->Cb + (pels/2 + 2*B)*B + B;
  }
  else {
    pi = InterpolateImage(pr->lum,pels,lines);
    prev_ipol = pi;
    prev_recon = pr;
    orig_lum = prev->lum;
  }

  /* mark PMV's outside the frame */
  for (i = 1; i < (pels>>4)+1; i++) {
    for (k = 0; k < 6; k++) {
      MV[k][0][i] = (MotionVector *)malloc(sizeof(MotionVector));
      MarkVec(MV[k][0][i]);
    }
    MV[0][0][i]->Mode = MODE_INTRA;
  }
  /* zero out PMV's outside the frame */
  for (i = 0; i < (lines>>4)+1; i++) {
    for (k = 0; k < 6; k++) {
      MV[k][i][0] = (MotionVector *)malloc(sizeof(MotionVector));
      ZeroVec(MV[k][i][0]);
      MV[k][i][(pels>>4)+1] = (MotionVector *)malloc(sizeof(MotionVector));
      ZeroVec(MV[k][i][(pels>>4)+1]);
    }
    MV[0][i][0]->Mode = MODE_INTRA;
    MV[0][i][(pels>>4)+1]->Mode = MODE_INTRA;
  }

  /* Integer and half pel motion estimation */
  MotionEstimatePicture(curr->lum,prev_recon->lum,prev_ipol,
        pic->seek_dist,MV, pic->use_gobsync);
  /* note: integer pel motion estimation is now based on previous
     reconstructed image, not the previous original image. We have
     found that this works better for some sequences and not worse for
     others.  Note that it can not easily be changed back by
     substituting prev_recon->lum with orig_lum in the line above,
     because SAD for zero vector is not re-calculated in the half
     pel search. The half pel search has always been based on the
     previous reconstructed image */

#ifndef OFFLINE_RATE_CONTROL
  if (pic->bit_rate != 0) {
    /* Initialization routine for Rate Control */
    QP_new = InitializeQuantizer(PCT_INTER, (float)pic->bit_rate, 
               (pic->PB ? pic->target_frame_rate/2 : pic->target_frame_rate),
               pic->QP_mean);
    QP_xmitted = QP_prev = QP_new; 
  }
  else {
    QP_new = QP_xmitted = QP_prev = QP; /* Copy the passed value of QP */
  }
#else
    QP_new = QP_xmitted = QP_prev = QP; /* Copy the passed value of QP */
#endif
  dquant = 0; 

  for ( j = 0; j < lines/MB_SIZE; j++) {

#ifndef OFFLINE_RATE_CONTROL
    if (pic->bit_rate != 0) {
      /* QP updated at the beginning of each row */
      AddBitsPicture(bits);
      
      QP_new =  UpdateQuantizer(abs_mb_num, pic->QP_mean, PCT_INTER, 
           (float)pic->bit_rate, pels/MB_SIZE, lines/MB_SIZE, 
           bits->total);
    }
#endif

    newgob = 0;

    if (j == 0) {
      pic->QUANT = QP_new;
      bits->header += CountBitsPicture(pic);
      QP_xmitted = QP_prev = QP_new;
    }
    else if (pic->use_gobsync && j%pic->use_gobsync == 0) {
      bits->header += CountBitsSlice(j,QP_new); /* insert gob sync */
      QP_xmitted = QP_prev = QP_new;
      newgob = 1;
    }

    for ( i = 0; i < pels/MB_SIZE; i++) {

      /* Update of dquant, check and correct its limit */
      dquant = QP_new - QP_prev;
      if (dquant != 0 && i != 0 && MV[0][j+1][i+1]->Mode == MODE_INTER4V) {
        /* It is not possible to change the quantizer and at the same
           time use 8x8 vectors. Turning off 8x8 vectors is not
           possible at this stage because the previous macroblock
           encoded assumed this one should use 8x8 vectors. Therefore
           the change of quantizer is postponed until the first MB
           without 8x8 vectors */
        dquant = 0;
        QP_xmitted = QP_prev;
        QuantChangePostponed = 1;
      }
      else {
        QP_xmitted = QP_new;
        QuantChangePostponed = 0;
      }
      if (dquant > 2)  { dquant =  2; QP_xmitted = QP_prev + dquant;}
      if (dquant < -2) { dquant = -2; QP_xmitted = QP_prev + dquant;}

      pic->DQUANT = dquant;
      /* modify mode if dquant != 0 (e.g. MODE_INTER -> MODE_INTER_Q) */
      Mode = ModifyMode(MV[0][j+1][i+1]->Mode,pic->DQUANT);
      MV[0][j+1][i+1]->Mode = Mode;

      pic->MB = i + j * (pels/MB_SIZE);

      if (Mode == MODE_INTER || Mode == MODE_INTER_Q || Mode==MODE_INTER4V) {
        /* Predict P-MB */
        diff = Predict_P(curr,prev_recon,prev_ipol,
         i*MB_SIZE,j*MB_SIZE,MV,pic->PB);

      }
      else {
        diff = (MB_Structure *)malloc(sizeof(MB_Structure));
        FillLumBlock(i*MB_SIZE, j*MB_SIZE, curr, diff);
        FillChromBlock(i*MB_SIZE, j*MB_SIZE, curr, diff);
      }

      /* P or INTRA Macroblock */
      qcoeff_P = MB_Encode(diff, QP_xmitted, Mode);
      CBP = FindCBP(qcoeff_P, Mode, 64);
      if (CBP == 0 && (Mode == MODE_INTER || Mode == MODE_INTER_Q)) 
        ZeroMBlock(diff);
      else
        MB_Decode(qcoeff_P, diff, QP_xmitted, Mode);
      recon_data_P = MB_Recon_P(prev_recon, prev_ipol,diff, 
        i*MB_SIZE,j*MB_SIZE,MV,pic->PB);
      Clip(recon_data_P);
      free(diff);
        

      /* Predict B-MB using reconstructed P-MB and prev. recon. image */
      if (pic->PB) {
        diff = Predict_B(B_image, prev_recon, prev_ipol,i*MB_SIZE, j*MB_SIZE,
         MV, recon_data_P, frameskip, pic->TRB);
        if (QP_xmitted == 0)  
          QP_B = 0;  /* (QP = 0 means no quantization) */
        else 
          QP_B = mmax(1,mmin(31,bquant[pic->BQUANT]*QP_xmitted/4));
        qcoeff_B = MB_Encode(diff, QP_B, MODE_INTER);
        CBPB = FindCBP(qcoeff_B, MODE_INTER, 64);
        if (CBPB)
          MB_Decode(qcoeff_B, diff, QP_B, MODE_INTER);
        else
          ZeroMBlock(diff);
        recon_data_B = MB_Recon_B(prev_recon, diff,prev_ipol,i*MB_SIZE,
          j*MB_SIZE,MV,recon_data_P,frameskip,
          pic->TRB);
        Clip(recon_data_B);

        /* decide MODB */ 

        if (CBPB) {
          pic->MODB = PBMODE_CBPB_MVDB;
        }
        else {
          if (MV[5][j+1][i+1]->x == 0 && MV[5][j+1][i+1]->y == 0)
            pic->MODB = PBMODE_NORMAL;
          else
            pic->MODB = PBMODE_MVDB;
        }

        free(diff);

      }
      else
        ZeroVec(MV[5][j+1][i+1]); /* Zero out PB deltas */

      if ((CBP==0) && (CBPB==0) && (EqualVec(MV[0][j+1][i+1],&ZERO)) && 
          (EqualVec(MV[5][j+1][i+1],&ZERO)) &&
          (Mode == MODE_INTER || Mode == MODE_INTER_Q)) {
        /* Skipped MB : both CBP and CBPB are zero, 16x16 vector is zero,
           PB delta vector is zero and Mode = MODE_INTER */
        if (Mode == MODE_INTER_Q) {
          /* DQUANT != 0 but not coded anyway */
          QP_xmitted = QP_prev;
          pic->DQUANT = 0;
          Mode = MODE_INTER;
        }
        if (!syntax_arith_coding)
          CountBitsMB(Mode,1,CBP,CBPB,pic,bits);
        else
          Count_sac_BitsMB(Mode,1,CBP,CBPB,pic,bits);
      }
      else {
        /* Normal MB */
        if (!syntax_arith_coding) { /* VLC */
          CountBitsMB(Mode,0,CBP,CBPB,pic,bits);

          if (Mode == MODE_INTER  || Mode == MODE_INTER_Q) {
            bits->no_inter++;
            CountBitsVectors(MV, bits, i, j, Mode, newgob, pic);
          }
          else if (Mode == MODE_INTER4V) {
            bits->no_inter4v++;
            CountBitsVectors(MV, bits, i, j, Mode, newgob, pic);
          }
          else {
            /* MODE_INTRA or MODE_INTRA_Q */
            bits->no_intra++;
            if (pic->PB)
              CountBitsVectors(MV, bits, i, j, Mode, newgob, pic);
          }
          
          if (CBP || Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
            CountBitsCoeff(qcoeff_P, Mode, CBP, bits, 64);
          if (CBPB)
            CountBitsCoeff(qcoeff_B, MODE_INTER, CBPB, bits, 64);
        } /* end VLC */

        else { /* SAC */
          Count_sac_BitsMB(Mode,0,CBP,CBPB,pic,bits);
 
          if (Mode == MODE_INTER  || Mode == MODE_INTER_Q) {
            bits->no_inter++;
            Count_sac_BitsVectors(MV, bits, i, j, Mode, newgob, pic);
          }
          else if (Mode == MODE_INTER4V) {
            bits->no_inter4v++;
            Count_sac_BitsVectors(MV, bits, i, j, Mode, newgob, pic);
          }
          else {
            /* MODE_INTRA or MODE_INTRA_Q */
            bits->no_intra++;
            if (pic->PB)
              Count_sac_BitsVectors(MV, bits, i, j, Mode, newgob, pic);
          }
          
          if (CBP || Mode == MODE_INTRA || Mode == MODE_INTRA_Q)
            Count_sac_BitsCoeff(qcoeff_P, Mode, CBP, bits, 64);
          if (CBPB)
            Count_sac_BitsCoeff(qcoeff_B, MODE_INTER, CBPB, bits, 64);
        } /* end SAC */

        QP_prev = QP_xmitted;
      }

      abs_mb_num++;
      QP_cumulative += QP_xmitted;     
#ifdef PRINTQ 
      /* most useful when quantizer changes within a picture */
      if (QuantChangePostponed)
        printf("@%2d",QP_xmitted);
      else
        printf(" %2d",QP_xmitted);
#endif

      if (pic->PB) 
        ReconImage(i,j,recon_data_B,B_recon);

      ReconImage(i,j,recon_data_P,recon);
      free(recon_data_P);
      free(qcoeff_P);
      if (pic->PB) {
        free(qcoeff_B);
        free(recon_data_B);
      }
    }
#ifdef PRINTQ
    printf("\n");
#endif
  }

  pic->QP_mean = QP_cumulative/(float)abs_mb_num;

  /* Free memory */
  free(pi);
  if (mv_outside_frame) {
    free(prev_recon);
    FreeImage(pr_edge);
  }
  for (j = 0; j < (lines>>4)+1; j++)
    for (i = 0; i < (pels>>4)+2; i++) 
      for (k = 0; k < 6; k++)
        free(MV[k][j][i]);
  return;
}


/**********************************************************************
 *
 *	Name:        CodeOneIntra
 *	Description:	codes one image intra
 *	
 *	Input:        pointer to image, QP
 *        
 *	Returns:	pointer to reconstructed image
 *	Side effects:	memory is allocated to recon image
 *
 *	Date: 940110	Author:	Karl.Lillevold@nta.no
 *
 ***********************************************************************/


PictImage *CodeOneIntra(PictImage *curr, int QP, Bits *bits, Pict *pic)
{
  PictImage *recon;
  MB_Structure *data = (MB_Structure *)malloc(sizeof(MB_Structure));
  int *qcoeff;
  int Mode = MODE_INTRA;
  int CBP,COD;
  int i,j;

  recon = InitImage(pels*lines);
  ZeroBits(bits);
  
  pic->QUANT = QP;
  bits->header += CountBitsPicture(pic);

  COD = 0; /* Every block is coded in Intra frame */
  for ( j = 0; j < lines/MB_SIZE; j++) {

    /* insert sync in *every* slice if use_gobsync is chosen */
    if (pic->use_gobsync && j != 0)
      bits->header += CountBitsSlice(j,QP);
    for ( i = 0; i < pels/MB_SIZE; i++) {

      pic->MB = i + j * (pels/MB_SIZE);
      bits->no_intra++;
      FillLumBlock(i*MB_SIZE, j*MB_SIZE, curr, data);
      FillChromBlock(i*MB_SIZE, j*MB_SIZE, curr, data);
      qcoeff = MB_Encode(data, QP, Mode);
      CBP = FindCBP(qcoeff,Mode,64);

      if (!syntax_arith_coding) {
        CountBitsMB(Mode,COD,CBP,0,pic,bits);
        CountBitsCoeff(qcoeff, Mode, CBP,bits,64);
      } else {
        Count_sac_BitsMB(Mode,COD,CBP,0,pic,bits);
        Count_sac_BitsCoeff(qcoeff, Mode, CBP,bits,64);
      }

      MB_Decode(qcoeff, data, QP, Mode);
      Clip(data);
      ReconImage(i,j,data,recon);
      free(qcoeff);
    }
  }
  pic->QP_mean = (float)QP;


  free(data);
  return recon;
}

/**********************************************************************
 *
 *	Name:        MB_Encode
 *	Description:	DCT and quantization of Macroblocks
 *
 *	Input:        MB data struct, mquant (1-31, 0 = no quant),
 *        MB info struct
 *	Returns:	Pointer to quantized coefficients 
 *	Side effects:	
 *
 *	Date: 930128	Author: Robert.Danielsen@nta.no
 *
 **********************************************************************/


int *MB_Encode(MB_Structure *mb_orig, int QP, int I)
{
  int        i, j, k, l, row, col;
  int        fblock[64];
  int        coeff[384];
  int        *coeff_ind;
  int                *qcoeff;
  int        *qcoeff_ind;

  if ((qcoeff=(int *)malloc(sizeof(int)*384)) == 0) {
    printf("stderr:mb_encode(): Couldn't allocate qcoeff.\n");
    exit(-1);
  }

  coeff_ind = coeff;
  qcoeff_ind = qcoeff;
  for (k=0;k<16;k+=8) {
    for (l=0;l<16;l+=8) {
      for (i=k,row=0;row<64;i++,row+=8) {
        for (j=l,col=0;col<8;j++,col++) {
          *(fblock+row+col) = mb_orig->lum[i][j];
        }
      }
      Dct(fblock,coeff_ind);
      Quant(coeff_ind,qcoeff_ind,QP,I);
      coeff_ind += 64;
      qcoeff_ind += 64;
    }
  }
  for (i=0;i<8;i++) {
    for (j=0;j<8;j++) {
      *(fblock+i*8+j) = mb_orig->Cb[i][j];
    }
  }
  Dct(fblock,coeff_ind);
  Quant(coeff_ind,qcoeff_ind,QP,I); 
  coeff_ind += 64;
  qcoeff_ind += 64;

  for (i=0;i<8;i++) {
    for (j=0;j<8;j++) {
      *(fblock+i*8+j) = mb_orig->Cr[i][j];
    }
  }
  Dct(fblock,coeff_ind);
  Quant(coeff_ind,qcoeff_ind,QP,I); 
  
  return qcoeff;
}

/**********************************************************************
 *
 *	Name:        MB_Decode
 *	Description:	Reconstruction of quantized DCT-coded Macroblocks
 *
 *	Input:        Quantized coefficients, MB data
 *        QP (1-31, 0 = no quant), MB info block
 *	Returns:	int (just 0)
 *	Side effects:	
 *
 *	Date: 930128	Author: Robert.Danielsen@nta.no
 *
 **********************************************************************/

     
int MB_Decode(int *qcoeff, MB_Structure *mb_recon, int QP, int I)
{
  int	i, j, k, l, row, col;
  int	*iblock;
  int	*qcoeff_ind;
  int	*rcoeff, *rcoeff_ind;

  if ((iblock = (int *)malloc(sizeof(int)*64)) == NULL) {
    printf("stderr:MB_Coder: Could not allocate space for iblock\n");
    exit(-1);
  }
  if ((rcoeff = (int *)malloc(sizeof(int)*384)) == NULL) {
    printf("stderr:MB_Coder: Could not allocate space for rcoeff\n");
    exit(-1);
  }  

  /* For control purposes */
  /* Zero data */
  for (i = 0; i < 16; i++)
    for (j = 0; j < 16; j++)
      mb_recon->lum[j][i] = 0;
  for (i = 0; i < 8; i++) 
    for (j = 0; j < 8; j++) {
      mb_recon->Cb[j][i] = 0;
      mb_recon->Cr[j][i] = 0;
    }

  qcoeff_ind = qcoeff;
  rcoeff_ind = rcoeff;

  for (k=0;k<16;k+=8) {
    for (l=0;l<16;l+=8) {
      Dequant(qcoeff_ind,rcoeff_ind,QP,I);
#ifndef FASTIDCT
      idctref(rcoeff_ind,iblock); 
#else
      idct(rcoeff_ind,iblock); 
#endif
      qcoeff_ind += 64;
      rcoeff_ind += 64;
      for (i=k,row=0;row<64;i++,row+=8) {
        for (j=l,col=0;col<8;j++,col++) {
          mb_recon->lum[i][j] = *(iblock+row+col);
        }
      }
    }
  }
  Dequant(qcoeff_ind,rcoeff_ind,QP,I);
#ifndef FASTIDCT
  idctref(rcoeff_ind,iblock); 
#else
  idct(rcoeff_ind,iblock); 
#endif
  qcoeff_ind += 64;
  rcoeff_ind += 64;
  for (i=0;i<8;i++) {
    for (j=0;j<8;j++) {
      mb_recon->Cb[i][j] = *(iblock+i*8+j);
    }
  }
  Dequant(qcoeff_ind,rcoeff_ind,QP,I);
#ifndef FASTIDCT
  idctref(rcoeff_ind,iblock); 
#else
  idct(rcoeff_ind,iblock); 
#endif
  for (i=0;i<8;i++) {
    for (j=0;j<8;j++) {
      mb_recon->Cr[i][j] = *(iblock+i*8+j);
    }
  }
  free(iblock);
  free(rcoeff);
  return 0;
}


/**********************************************************************
 *
 *	Name:        FillLumBlock
 *	Description:        Fills the luminance of one block of PictImage
 *	
 *	Input:        Position, pointer to PictImage, array to fill
 *	Returns:        
 *	Side effects:	fills array
 *
 *	Date: 930129	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void FillLumBlock( int x, int y, PictImage *image, MB_Structure *data)
{
  int n;
  register int m;

  for (n = 0; n < MB_SIZE; n++)
    for (m = 0; m < MB_SIZE; m++)
      data->lum[n][m] = 
        (int)(*(image->lum + x+m + (y+n)*pels));
  return;
}

/**********************************************************************
 *
 *	Name:        FillChromBlock
 *	Description:        Fills the chrominance of one block of PictImage
 *	
 *	Input:        Position, pointer to PictImage, array to fill
 *	Returns:        
 *	Side effects:	fills array
 *                      128 subtracted from each
 *
 *	Date: 930129	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void FillChromBlock(int x_curr, int y_curr, PictImage *image,
            MB_Structure *data)
{
  int n;
  register int m;

  int x, y;

  x = x_curr>>1;
  y = y_curr>>1;

  for (n = 0; n < (MB_SIZE>>1); n++)
    for (m = 0; m < (MB_SIZE>>1); m++) {
      data->Cr[n][m] = 
        (int)(*(image->Cr +x+m + (y+n)*cpels));
      data->Cb[n][m] = 
        (int)(*(image->Cb +x+m + (y+n)*cpels));
    }
  return;
}


/**********************************************************************
 *
 *	Name:        ZeroMBlock
 *	Description:        Fills one MB with Zeros
 *	
 *	Input:        MB_Structure to zero out
 *	Returns:        
 *	Side effects:	
 *
 *	Date: 940829	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void ZeroMBlock(MB_Structure *data)
{
  int n;
  register int m;

  for (n = 0; n < MB_SIZE; n++)
    for (m = 0; m < MB_SIZE; m++)
      data->lum[n][m] = 0;
  for (n = 0; n < (MB_SIZE>>1); n++)
    for (m = 0; m < (MB_SIZE>>1); m++) {
      data->Cr[n][m] = 0;
      data->Cb[n][m] = 0;
    }
  return;
}

/**********************************************************************
 *
 *	Name:        ReconImage
 *	Description:	Puts together reconstructed image
 *	
 *	Input:        position of curr block, reconstructed
 *        macroblock, pointer to recontructed image
 *	Returns:
 *	Side effects:
 *
 *	Date: 930123        Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void ReconImage (int i, int j, MB_Structure *data, PictImage *recon)
{
  int n;
  register int m;

  int x_curr, y_curr;

  x_curr = i * MB_SIZE;
  y_curr = j * MB_SIZE;

  /* Fill in luminance data */
  for (n = 0; n < MB_SIZE; n++)
    for (m= 0; m < MB_SIZE; m++) {
      *(recon->lum + x_curr+m + (y_curr+n)*pels) = data->lum[n][m];
    }

  /* Fill in chrominance data */
  for (n = 0; n < MB_SIZE>>1; n++)
    for (m = 0; m < MB_SIZE>>1; m++) {
      *(recon->Cr + (x_curr>>1)+m + ((y_curr>>1)+n)*cpels) = data->Cr[n][m];
      *(recon->Cb + (x_curr>>1)+m + ((y_curr>>1)+n)*cpels) = data->Cb[n][m];
    }
  return;
}


/**********************************************************************
 *
 *	Name:        InterpolateImage
 *	Description:    Interpolates a complete image for easier half
 *                      pel prediction
 *	
 *	Input:	        pointer to image structure
 *	Returns:        pointer to interpolated image
 *	Side effects:   allocates memory to interpolated image
 *
 *	Date: 950207        Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/


unsigned char *InterpolateImage(unsigned char *image, int width, int height)
{
  unsigned char *ipol_image, *ii, *oo;
  int i,j;

  ipol_image = (unsigned char *)malloc(sizeof(char)*width*height*4);
  ii = ipol_image;
  oo = image;

  /* main image */
  for (j = 0; j < height-1; j++) {
    for (i = 0; i  < width-1; i++) {
      *(ii + (i<<1)) = *(oo + i);
      *(ii + (i<<1)+1) = (*(oo + i) + *(oo + i + 1) + 1)>>1;
      *(ii + (i<<1)+(width<<1)) = (*(oo + i) + *(oo + i + width) + 1)>>1;
      *(ii + (i<<1)+1+(width<<1)) = (*(oo+i) + *(oo+i+1) + 
         *(oo+i+width) + *(oo+i+1+width) + 2)>>2;
    }
    /* last pels on each line */
    *(ii+ (width<<1) - 2) = *(oo + width - 1);
    *(ii+ (width<<1) - 1) = *(oo + width - 1);
    *(ii+ (width<<1)+ (width<<1)-2) = (*(oo+width-1)+*(oo+width+width-1)+1)>>1;
    *(ii+ (width<<1)+ (width<<1)-1) = (*(oo+width-1)+*(oo+width+width-1)+1)>>1;
    ii += (width<<2);
    oo += width;
  }

  /* last lines */
  for (i = 0; i < width-1; i++) {
    *(ii+ (i<<1)) = *(oo + i);    
    *(ii+ (i<<1)+1) = (*(oo + i) + *(oo + i + 1) + 1)>>1;
    *(ii+ (width<<1)+ (i<<1)) = *(oo + i);    
    *(ii+ (width<<1)+ (i<<1)+1) = (*(oo + i) + *(oo + i + 1) + 1)>>1;
          
  }

  /* bottom right corner pels */
  *(ii + (width<<1) - 2) = *(oo + width -1);
  *(ii + (width<<1) - 1) = *(oo + width -1);
  *(ii + (width<<2) - 2) = *(oo + width -1);
  *(ii + (width<<2) - 1) = *(oo + width -1);

  return ipol_image;
}


/**********************************************************************
 *
 *	Name:        MotionEstimatePicture
 *	Description:    Finds integer and half pel motion estimation
 *                      and chooses 8x8 or 16x16 
 *	
 *	Input:	       current image, previous image, interpolated
 *                     reconstructed previous image, seek_dist,
 *                     motion vector array
 *	Returns:       
 *	Side effects: allocates memory for MV structure
 *
 *	Date: 950209	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/


void MotionEstimatePicture(unsigned char *curr, unsigned char *prev, 
           unsigned char *prev_ipol, int seek_dist, 
           MotionVector *MV[6][MBR+1][MBC+2], int gobsync)
           
{
  int i,j,k;
  int pmv0,pmv1,xoff,yoff;
  int curr_mb[16][16];
  int sad8 = INT_MAX, sad16, sad0;
  int newgob;
  MotionVector *f0,*f1,*f2,*f3,*f4;

  /* Do motion estimation and store result in array */
  for ( j = 0; j < lines/MB_SIZE; j++) {

    newgob = 0;
    if (gobsync && j%gobsync == 0) {
      newgob = 1;
    }

    for ( i = 0; i < pels/MB_SIZE; i++) {
      for (k = 0; k < 6; k++)
        MV[k][j+1][i+1] = (MotionVector *)malloc(sizeof(MotionVector));

      /* Integer pel search */
      f0 = MV[0][j+1][i+1];
      f1 = MV[1][j+1][i+1];
      f2 = MV[2][j+1][i+1];
      f3 = MV[3][j+1][i+1];
      f4 = MV[4][j+1][i+1];


      /* Here the PMV's are found using integer motion vectors */
      /* (NB should add explanation for this )*/
      FindPMV(MV,i+1,j+1,&pmv0,&pmv1,0,newgob,0);

      if (long_vectors) {
        xoff = pmv0/2; /* always divisable by two */
        yoff = pmv1/2;
      }
      else {
        xoff = yoff = 0;
      }
      
      MotionEstimation(curr, prev, i*MB_SIZE, j*MB_SIZE, 
               xoff, yoff, seek_dist, MV, &sad0);

      sad16 = f0->min_error;
      if (advanced)
        sad8 = f1->min_error + f2->min_error + f3->min_error + f4->min_error;

      f0->Mode = ChooseMode(curr,i*MB_SIZE,j*MB_SIZE, mmin(sad8,sad16));

      /* Half pel search */
      if (f0->Mode != MODE_INTRA) {
        FindMB(i*MB_SIZE,j*MB_SIZE ,curr, curr_mb);
        FindHalfPel(i*MB_SIZE,j*MB_SIZE,f0, prev_ipol, &curr_mb[0][0],16,0);
        sad16 = f0->min_error;

        if (advanced) {
          FindHalfPel(i*MB_SIZE,j*MB_SIZE,f1, prev_ipol, &curr_mb[0][0],8,0);
          FindHalfPel(i*MB_SIZE,j*MB_SIZE,f2, prev_ipol, &curr_mb[0][8],8,1);
          FindHalfPel(i*MB_SIZE,j*MB_SIZE,f3, prev_ipol, &curr_mb[8][0],8,2);
          FindHalfPel(i*MB_SIZE,j*MB_SIZE,f4, prev_ipol, &curr_mb[8][8],8,3);

          sad8 = f1->min_error +f2->min_error +f3->min_error +f4->min_error;
          sad8 += PREF_16_VEC;
          
          /* Choose Zero Vector, 8x8 or 16x16 vectors */
          if (sad0 < sad8 && sad0 < sad16) {
            f0->x = f0->y = 0;
            f0->x_half = f0->y_half = 0;
          }
          else {
            if (sad8 < sad16) 
              f0->Mode = MODE_INTER4V;
          }
        }
        else {
          /* Choose Zero Vector or 16x16 vectors */
          if (sad0 < sad16) {
            f0->x = f0->y = 0;
            f0->x_half = f0->y_half = 0;
          }
        }

      }
      else 
        for (k = 0; k < 5; k++)
          ZeroVec(MV[k][j+1][i+1]);

    }
  }

#ifdef PRINTMV
  printf("Motion estimation\n");
  printf("16x16 vectors:\n");

  for ( j = 0; j < lines/MB_SIZE; j++) {
    for ( i = 0; i < pels/MB_SIZE; i++) {
      if (MV[0][j+1][i+1]->Mode != MODE_INTRA)
        printf(" %3d%3d",
        2*MV[0][j+1][i+1]->x + MV[0][j+1][i+1]->x_half,
        2*MV[0][j+1][i+1]->y + MV[0][j+1][i+1]->y_half);
      else
        printf("  .  . ");
    }
    printf("\n");
  }
  if (advanced) {
    printf("8x8 vectors:\n");
    for (k = 1; k < 5; k++) {
      printf("Block: %d\n", k-1);
      for ( j = 0; j < lines/MB_SIZE; j++) {
        for ( i = 0; i < pels/MB_SIZE; i++) {
          if (MV[0][j+1][i+1]->Mode != MODE_INTRA)
            printf(" %3d%3d",
            2*MV[k][j+1][i+1]->x + MV[k][j+1][i+1]->x_half,
            2*MV[k][j+1][i+1]->y + MV[k][j+1][i+1]->y_half);
          else
            printf("  .  . ");
        }
        printf("\n");
      }
    }
  }
#endif
  return;
}


/**********************************************************************
 *
 *	Name:        MakeEdgeImage
 *	Description:    Copies edge pels for use with unrestricted
 *                      motion vector mode
 *	
 *	Input:	        pointer to source image, destination image
 *                      width, height, edge
 *	Returns:       
 *	Side effects:
 *
 *	Date: 950219        Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void MakeEdgeImage(unsigned char *src, unsigned char *dst, int width,
           int height, int edge)
{
  int i,j;
  unsigned char *p1,*p2,*p3,*p4;
  unsigned char *o1,*o2,*o3,*o4;

  /* center image */
  p1 = dst;
  o1 = src;
  for (j = 0; j < height;j++) {
    memcpy(p1,o1,width);
    p1 += width + (edge<<1);
    o1 += width;
  }

  /* left and right edges */
  p1 = dst-1;
  o1 = src;
  for (j = 0; j < height;j++) {
    for (i = 0; i < edge; i++) {
      *(p1 - i) = *o1;
      *(p1 + width + i + 1) = *(o1 + width - 1);
    }
    p1 += width + (edge<<1);
    o1 += width;
  }    
    
  /* top and bottom edges */
  p1 = dst;
  p2 = dst + (width + (edge<<1))*(height-1);
  o1 = src;
  o2 = src + width*(height-1);
  for (j = 0; j < edge;j++) {
    p1 = p1 - (width + (edge<<1));
    p2 = p2 + (width + (edge<<1));
    for (i = 0; i < width; i++) {
      *(p1 + i) = *(o1 + i);
      *(p2 + i) = *(o2 + i);
    }
  }    

  /* corners */
  p1 = dst - (width+(edge<<1)) - 1;
  p2 = p1 + width + 1;
  p3 = dst + (width+(edge<<1))*(height)-1;
  p4 = p3 + width + 1;

  o1 = src;
  o2 = o1 + width - 1;
  o3 = src + width*(height-1);
  o4 = o3 + width - 1;
  for (j = 0; j < edge; j++) {
    for (i = 0; i < edge; i++) {
      *(p1 - i) = *o1;
      *(p2 + i) = *o2;
      *(p3 - i) = *o3;
      *(p4 + i) = *o4; 
    }
    p1 = p1 - (width + (edge<<1));
    p2 = p2 - (width + (edge<<1));
    p3 = p3 + width + (edge<<1);
    p4 = p4 + width + (edge<<1);
  }
}


/**********************************************************************
 *
 *	Name:        Clip
 *	Description:    clips recontructed data 0-255
 *	
 *	Input:	        pointer to recon. data structure
 *	Side effects:   data structure clipped
 *
 *	Date: 950718        Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void Clip(MB_Structure *data)
{
  int m,n;

  for (n = 0; n < 16; n++) {
    for (m = 0; m < 16; m++) {
      data->lum[n][m] = mmin(255,mmax(0,data->lum[n][m]));
    }
  }
  for (n = 0; n < 8; n++) {
    for (m = 0; m < 8; m++) {
      data->Cr[n][m] = mmin(255,mmax(0,data->Cr[n][m]));
      data->Cb[n][m] = mmin(255,mmax(0,data->Cb[n][m]));
    }
  }
}
