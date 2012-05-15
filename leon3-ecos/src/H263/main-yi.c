/*
 * yixiaoyang 2010@HIT
 *
 * modify main.c and try to run it.
 */
#include"sim.h"

FILE *streamfile;

void main(int argc, char *argv[])
{
    PictImage *prev_image = NULL;
    PictImage *curr_image = NULL;
    PictImage *curr_recon = NULL;
    PictImage *prev_recon = NULL;

    /* PB-frame specific */
    PictImage *B_recon = NULL;
    PictImage *B_image = NULL;

    Pict *pic = (Pict *)malloc(sizeof(Pict));
    unsigned char *image;
    FILE *cleared;

    int i;
    float mean_frame_rate, ref_frame_rate, frame_rate, seconds;
    int first_loop_finished=0;
    int total_frames_passed, PPFlag = 0, targetrate;
    int frames,bframes,pframes,wcopies,icopies, write_repeated,pdist=0,bdist=0;
    int start, end, frame_no, writediff;
    int first_frameskip, chosen_frameskip, orig_frameskip, frameskip;
    int QP,QPI;
    Bits *bits = (Bits *)malloc(sizeof(Bits));
    Bits *total_bits = (Bits *)malloc(sizeof(Bits));
    Bits *intra_bits = (Bits *)malloc(sizeof(Bits));
    Results *res = (Results *)malloc(sizeof(Results));
    Results *total_res = (Results *)malloc(sizeof(Results));
    Results *b_res = (Results *)malloc(sizeof(Results));
    char *seqfilename = (char *)malloc(sizeof(char)*100);
    char *streamname = (char *)malloc(sizeof(char)*100);
    char *outputfile =  (char *)malloc(sizeof(char)*100);
    char *diff_filename=DEF_DIFFILENAME;
    char *tracefile =  (char *)malloc(sizeof(char)*100);

	float DelayBetweenFramesInSeconds;
	int CommBacklog;

    PictImage *stored_image = NULL;
    int start_rate_control;
    extern int arith_used;
    fprintf (stdout,"\nTMN (H.263) coder version 2.0, Copyright (C) 1995, 1996 Telenor R&D, Norway\n");
    headerlength = DEF_HEADERLENGTH;

#ifndef FASTIDCT
    init_idctref();
#endif

    /* Default variable values */
    advanced = DEF_ADV_MODE;
    syntax_arith_coding = DEF_SAC_MODE;
    pic->unrestricted_mv_mode = DEF_UMV_MODE;
    mv_outside_frame = DEF_UMV_MODE || DEF_ADV_MODE;
    long_vectors = DEF_UMV_MODE;
    pb_frames = DEF_PBF_MODE;

    QP = DEF_INTER_QUANT;
    QPI = DEF_INTRA_QUANT;
    pic->BQUANT = DEF_BQUANT;
    pic->source_format = DEF_CODING_FORMAT;

    ref_frame_rate = (float)DEF_REF_FRAME_RATE;
    chosen_frameskip = DEF_FRAMESKIP + 1;
    orig_frameskip = DEF_ORIG_SKIP + 1;

    pic->target_frame_rate = (float)DEF_TARGET_FRAME_RATE;


    seqfilename[0] = '\0';
    strcpy(streamname, DEF_STREAMNAME);
    strcpy(outputfile, DEF_OUTFILENAME);

    writediff = DEF_WRITE_DIFF;
    trace = DEF_WRITE_TRACE;
    write_repeated = DEF_WRITE_REPEATED;
    pic->seek_dist = DEF_SEEK_DIST;
    pic->use_gobsync = DEF_INSERT_SYNC;
    start = DEF_START_FRAME;
    end = DEF_STOP_FRAME;

    targetrate = 0;
    /* default is variable bit rate (fixed quantizer) will be used */

    frames = 0;
    pframes = 0;
    bframes = 0;
    total_frames_passed = 0;
    pic->PB = 0;
    wcopies = icopies = 1;

    pic->TR = 0;
    pic->QP_mean = (float)0.0;



    /* Process arguments */
    for (i = 1; i < argc; i++)
    {
        if (*(argv[i]) == '-')
        {
            switch(*(++argv[i]))
            {
            case 'a':
                start = atoi(argv[++i]);
                break;
            case 'b':
                end = atoi(argv[++i]);
                break;
            case 'i':
                strcpy(seqfilename, argv[++i]);
                break;
            case 'B':
                strcpy(streamname, argv[++i]);
                break;
            default:
                printf("stderr:Illegal option: %c\n",*argv[i]);
                Help();
                exit(-1);
                break;
            }
        }
    }

    switch (pic->source_format)
    {
    case (SF_SQCIF):
        printf( "Encoding format: SQCIF (128x96)\n");
        pels = 128;
        lines = 96;
        break;
    case (SF_QCIF):
        printf( "Encoding format: QCIF (176x144)\n");
        pels = 176;
        lines = 144;
        break;
    default:
        printf("stderr:Illegal coding format\n");
        exit(-1);
    }
    cpels = pels/2;

    if (seqfilename[0] == '\0')
    {
        printf("stderr:Required input parameter \'-i <filename>\' missing\n");
       // Help();
        exit(-1);
    }

    if (QP == 0 || QPI == 0)
    {
        printf("stderr:Warning:");
        fprintf(stderr,"QP is zero. Bitstream will not be correctly decodable\n");
    }

    if (ref_frame_rate != 25.0 && ref_frame_rate != 30.0)
    {
        printf("stderr:Warning: Reference frame rate should be 25 or 30 fps\n");
    }

    frame_rate =  ref_frame_rate / (float)(orig_frameskip * chosen_frameskip);

    if (pic->bit_rate == 0)
        printf("Encoding frame rate  : %.2f\n", frame_rate);
    else
        printf("Encoding frame rate  : variable\n");

    printf("Reference frame rate : %.2f\n", ref_frame_rate);
    printf("Orig. seq. frame rate: %.2f\n\n",
            ref_frame_rate / (float)orig_frameskip);

    /* Open stream for writing */
    streamfile = fopen (streamname, "wb");
    if (streamname == NULL)
    {
        printf("stderr:Unable to open streamfile\n");
        exit(-1);
    }

    /* Initialize bitcounters */
    initbits ();

    /* Clear output files */
    if ((cleared = fopen(outputfile,"wb")) == NULL)
    {
        printf("stderr:Couldn't open outputfile: %s\n",outputfile);
        exit(-1);
    }
    else
        fclose(cleared);


    /* Intra image */
	/* 读取图片到内存中*/
    image = ReadImage(seqfilename,start,headerlength);
    printf("stderr:Coding...\n");
    curr_image = FillImage(image);
    pic->picture_coding_type = PCT_INTRA;
    pic->QUANT = QPI;

    curr_recon = CodeOneIntra(curr_image, QPI, bits, pic);

    if (arith_used)
    {
        bits->header += encoder_flush();
        arith_used = 0;
    }
    bits->header += alignbits (); /* pictures shall be byte aligned */

    printf("Finished INTRA\n");

    ComputeSNR(curr_image, curr_recon, res, writediff);
    AddBitsPicture(bits);
    PrintSNR(res, 1);
    PrintResult(bits, 1, 1);
    memcpy(intra_bits,bits,sizeof(Bits));
    ZeroBits(total_bits);
    ZeroRes(total_res);
    ZeroRes(b_res);

    /* number of seconds to encode */
    seconds = (end - start + chosen_frameskip) * orig_frameskip/ ref_frame_rate;

    /* compute first frameskip */

	CommBacklog = intra_bits->total -
		(int)(DelayBetweenFramesInSeconds * pic->bit_rate);

    if (pic->bit_rate == 0)
    {
        frameskip = chosen_frameskip;
    }
    else    /* rate control is used */
    {
        frameskip = 1;
        while ( (int)(DelayBetweenFramesInSeconds*pic->bit_rate) <= CommBacklog)
        {
            CommBacklog -= (int) ( DelayBetweenFramesInSeconds * pic->bit_rate );
            frameskip += 1;
        }
    }
    first_frameskip = frameskip;

    if (first_frameskip > 256)
        printf("stderr:Warning: frameskip > 256\n");

    pic->picture_coding_type = PCT_INTER;
    pic->QUANT = QP;
    bdist = chosen_frameskip;

    /* always encode the first frame after intra as P frame.
       This is not necessary, but something we chose to make
       the adaptive PB frames calculations a bit simpler */
    if (pb_frames)
    {
        pic->PB = 0;
        pdist = 2*chosen_frameskip - bdist;
    }

    if (write_repeated)
        icopies = chosen_frameskip;
    for (i = 0; i < icopies; i++)
        WriteImage(curr_recon,outputfile); /* write wcopies frames to disk */


    /***** Main loop *****/
    for (frame_no = start + first_frameskip; frame_no <= end;
            frame_no += frameskip)
    {

        prev_image = curr_image;
        prev_recon = curr_recon;

        /* Set QP to pic->QUANT from previous encoded picture */
        QP = pic->QUANT;

        if (!PPFlag)
        {
            if (pic->PB)
            {
                bdist = frameskip;
                pdist = 2*frameskip - bdist;
                pic->TRB = bdist * orig_frameskip;
                if (pic->TRB > 8)
                    printf("distance too large for B-frame\n");
                /* Read the frame to be coded as B */
                image = ReadImage(seqfilename,frame_no,headerlength);
                B_image = FillImage(image);
                first_loop_finished = 1;
                if (frame_no + pdist <= end)
                {
                    image = ReadImage(seqfilename,frame_no + pdist,headerlength);
                }
                else
                {
                    pic->PB = 0; /* end of sequence, encode as P */
                    image =  ReadImage(seqfilename,frame_no,headerlength);
                }
            }
            else
            {
                image = ReadImage(seqfilename,frame_no,headerlength);
            }
            curr_image = FillImage(image);

            if (pic->PB)
            {
                if (pic->TRB > 8 || !NextTwoPB(curr_image, B_image, prev_image,
                                               bdist, pdist, pic->seek_dist))
                {
                    /* curr_image and B_image were not suitable to be coded
                       as a PB-frame - encoding as two P-frames instead */
                    pic->PB = 0;
#ifdef OFFLINE_RATE_CONTROL
                    stored_image = curr_image;
#else
                    FreeImage(curr_image);
#endif
                    frameskip = bdist;

                    curr_image = B_image;
                    PPFlag = 1;
                }
                else
                {
                    frame_no += pdist;
                }
            }
        }
        else
        {
            /* PPFlag is set when the second of the two P-frames
               is due to be coded */
#ifdef OFFLINE_RATE_CONTROL
            curr_image = stored_image;
#else
            image =  ReadImage(seqfilename,frame_no,headerlength);
            curr_image = FillImage(image);
#endif
            pic->PB = 0;
            PPFlag = 0;
        }

        /* Temporal Reference is the distance between encoded frames compared
           the reference picture rate which is 25.0 or 30 fps */
        pic->TR += (( (frameskip+(pic->PB?pdist:0)) *orig_frameskip) % 256);
        if (frameskip+(pic->PB?pdist:0) > 256)
            printf("Warning: frameskip > 256\n");


        frames += (pic->PB ? 2: 1);
        bframes += (pic->PB ? 1 : 0);
        pframes += 1;

        if (pic->PB)   /* Code two frames as a PB-frame */
        {
            B_recon = InitImage(pels*lines);
            printf("Coding PB frames %d and %d... ",
                    frame_no - pdist, frame_no);
            fflush(stdout);
        }
        else   /* Code the next frame as a normal P-frame */
        {
            printf("Coding P frame %d... ", frame_no);
            fflush(stdout);
        }
        curr_recon = InitImage(pels*lines);


        CodeOneOrTwo(curr_image, B_image, prev_image, prev_recon,
                     QP, (bdist+pdist)*orig_frameskip, bits, pic,
                     B_recon, curr_recon);

        /*
         * yixiaoyang
         * 这里codeoneortwo完成1-2个图片帧的压缩。
         * 应该在这里设想进行网络传送
         */


        printf("done\n");
        if (targetrate != 0)
            printf("Inter QP: %d\n", QP);
        fflush(stdout);

        if (arith_used)
        {
            bits->header += encoder_flush();
            arith_used = 0;
        }

        bits->header += alignbits ();  /* pictures shall be byte aligned */
        AddBitsPicture(bits);
        AddBits(total_bits, bits);


        /* Aim for the targetrate with a once per frame rate control scheme */
        if (targetrate != 0)
            if (frame_no - start > (end - start) * start_rate_control/100.0)
                /* when generating the MPEG-4 anchors, rate control was started
                   after 70% of the sequence was finished.
                   Set start_rate_control with option "-R <n>" */

                pic->QUANT = FrameUpdateQP(total_bits->total + intra_bits->total,
                                           bits->total / (pic->PB?2:1),
                                           (end-frame_no) / chosen_frameskip + PPFlag,
                                           QP, targetrate, seconds);
        frameskip = chosen_frameskip;

        if (pic->PB)
        {
            if (write_repeated)
                wcopies = pdist;
            for (i = 0; i < wcopies; i++)
                WriteImage(B_recon,outputfile); /* write wcopies frames to disk */
            ComputeSNR(B_image, B_recon, res, writediff);
            /*
             * yixiaoyang add
             * ComputeSNR 将会被去掉。计算信噪比不出现在实际应用代码里面。
             */
            printf("Results for B-frame:\n");
            AddRes(b_res,res,pic);
            PrintSNR(res, 1);
            FreeImage(B_image);
            FreeImage(B_recon);
        }

        if (write_repeated)
            wcopies = (pb_frames ? bdist : frameskip);
        for (i = 0; i < wcopies; i++)
            WriteImage(curr_recon,outputfile); /* write wcopies frames to disk */

        if (pb_frames)
            pic->PB = 1;

        ComputeSNR(curr_image, curr_recon, res, writediff);
        printf("Results for P-frame:\n");
        AddRes(total_res,res,pic);
        PrintSNR(res, 1);
        PrintResult(bits, 1, 1);
        FreeImage(prev_image);
        FreeImage(prev_recon);
        fflush(stdout);
    }
    /***** end of main loop *****/

    /* Closing files */
    /*
     * yixiaoyang add
     * 在实际应用中，文件流生成将会被去掉。
     * 直接在内存上存放压缩数据。
     */

    fclose (streamfile);

    /* Print total results */
    total_frames_passed = frame_no - start - first_frameskip;

    printf("\n==== TOTAL ====\n");
    printf("for %d images of %s\n", frames, seqfilename);

    /*
     * yixiaoyang add
     * if里面的这一段仅仅进行数据的统计，可删除。
     */
    if (frames != 0)
    {
        if (write_repeated)
            printf("Frames saved : %d predicted + %d intra\n",
                    total_frames_passed,icopies);

        printf("--------------\n");

        if (pb_frames && bframes != 0)
        {
            printf("SNR for %d B-frames:\n",bframes);
            PrintSNR(b_res,bframes);
        }

        printf("SNR for %d P-frames:\n",pframes);
        PrintSNR(total_res,pframes);

        PrintResult(total_bits, pframes, frames);

        if (targetrate != 0 || pic->bit_rate != 0)
            printf("Original seq time: %.2f (%.2f) sec\n",
                    (total_frames_passed + first_frameskip) /
                    ref_frame_rate * orig_frameskip,
                    total_frames_passed /
                    ref_frame_rate * orig_frameskip);

        printf("Mean quantizer   : %.2f\n", total_res->QP_mean/pframes);

    /* Free memory */
    FreeImage(curr_recon);
    FreeImage(curr_image);
    free(streamname);
    free(seqfilename);
    free(outputfile);
    free(tracefile);
    free(bits);
    free(total_bits);
    free(intra_bits);
    free(res);
    free(total_res);
    free(b_res);
    free(pic);
    exit(0);
}

/**********************************************************************
 *
 *	Name:        NextTwoPB
 *	Description:    Decides whether or not to code the next
 *                      two images as PB
 *      Speed:          This is not a very smart solution considering
 *                      the encoding speed, since motion vectors
 *                      have to be calculation several times. It
 *                      can be done together with the normal
 *                      motion vector search, or a tree search
 *                      instead of a full search can be used.
 *
 *	Input:	        pointers to previous image, potential B-
 *                      and P-image, frame distances
 *	Returns:        1 for yes, 0 otherwise
 *	Side effects:
 *
 *	Date: 950824	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

int NextTwoPB(PictImage *next2, PictImage *next1, PictImage *prev,
              int bskip, int pskip, int seek_dist)
{
    int adv_is_on = 0, mof_is_on = 0, lv_is_on = 0;
    int psad1, psad2, bsad, psad;
    MotionVector *MV[6][MBR+1][MBC+2];
    MotionVector *mvp, *mvbf, *mvbb;
    int x,y;
    int i,j,k,tmp;

    /* Temporarily disable some options to simplify motion estimation */
    if (advanced)
    {
        advanced = OFF;
        adv_is_on = ON;
    }
    if (mv_outside_frame)
    {
        mv_outside_frame = OFF;
        mof_is_on = ON;
    }
    if (long_vectors)
    {
        long_vectors = OFF;
        lv_is_on = ON;
    }

    for (j = 1; j <= (lines>>4); j++)
        for (i = 1; i <= (pels>>4); i++)
            for (k = 0; k < 3; k++)
            {
                MV[k][j][i] = (MotionVector *)calloc(1,sizeof(MotionVector));
                /* calloc to avoid Checker warnings about reading of
                   unitizalized memory in the memcpy's below */
            }

    mvbf = (MotionVector *)malloc(sizeof(MotionVector));
    mvbb = (MotionVector *)malloc(sizeof(MotionVector));

    psad = 0;
    psad1 = 0;
    psad2 = 0;
    bsad = 0;

    /* Integer motion estimation */
    for ( j = 1; j < lines/MB_SIZE - 1; j++)
    {
        for ( i = 1; i < pels/MB_SIZE - 1 ; i++)
        {
            x = i*MB_SIZE;
            y = j*MB_SIZE;

            /* picture order: prev -> next1 -> next2 */
            /* next1 and next2 can be coded as PB or PP */
            /* prev is the previous encoded picture */

            /* computes vectors (prev <- next2) */
            MotionEstimation(next2->lum,prev->lum,x,y,0,0,seek_dist,MV,&tmp);
            if (MV[0][j+1][i+1]->x == 0 && MV[0][j+1][i+1]->y == 0)
                MV[0][j+1][i+1]->min_error += PREF_NULL_VEC;
            /* not necessary to prefer zero vector here */
            memcpy(MV[2][j+1][i+1],MV[0][j+1][i+1],sizeof(MotionVector));

            /* computes sad(prev <- next1) */
            MotionEstimation(next1->lum,prev->lum,x,y,0,0,seek_dist,MV,&tmp);
            if (MV[0][j+1][i+1]->x == 0 && MV[0][j+1][i+1]->y == 0)
                MV[0][j+1][i+1]->min_error += PREF_NULL_VEC;
            memcpy(MV[1][j+1][i+1],MV[0][j+1][i+1],sizeof(MotionVector));

            /* computes vectors for (next1 <- next2) */
            MotionEstimation(next2->lum,next1->lum,x,y,0,0,seek_dist,MV,&tmp);
            if (MV[0][j+1][i+1]->x == 0 && MV[0][j+1][i+1]->y == 0)
                MV[0][j+1][i+1]->min_error += PREF_NULL_VEC;

            /* scales vectors for (prev <- next2 ) */
            mvp = MV[2][j+1][i+1];
            mvbf->x =   bskip * mvp->x / (bskip + pskip);
            mvbb->x = - pskip * mvp->x / (bskip + pskip);
            mvbf->y =   bskip * mvp->y / (bskip + pskip);
            mvbb->y = - pskip * mvp->y / (bskip + pskip);

            psad1 += MV[0][j+1][i+1]->min_error;
            psad2 += MV[1][j+1][i+1]->min_error;
            psad +=  mvp->min_error;

            /* computes sad(prev <- next1 -> next2) */
            bsad += SAD_MB_Bidir(next1->lum + x + y*pels,
                                 next2->lum + x + mvbb->x + (y + mvbb->y)*pels,
                                 prev->lum + x + mvbf->x + (y + mvbf->y)*pels,
                                 pels, INT_MAX);
        }
    }

    for (j = 1; j <= (lines>>4); j++)
        for (i = 1; i <= (pels>>4); i++)
            for (k = 0; k < 3; k++)
                free(MV[k][j][i]);
    free(mvbf);
    free(mvbb);

    /* restore advanced parameters */
    advanced = adv_is_on;
    mv_outside_frame = mof_is_on;
    long_vectors = lv_is_on;

    /* do the decision */
    if (bsad < (psad1+psad2)/2)
        printf("Chose PB - bsad %d, psad %d\n",
                bsad, (psad1+psad2)/2);
    else
        printf("Chose PP  - bsad %d, psad %d\n",
                bsad, (psad1+psad2)/2);

    if (bsad < (psad1 + psad2)/2)
        return 1;
    else
        return 0;
}



/**********************************************************************
 *
 *	Name:        PrintResult
 *	Description:	add bits and prints results
 *
 *	Input:        Bits struct
 *
 *	Returns:
 *	Side effects:
 *
 *	Date: 940116	Author:	Karl.Lillevold@nta.no
 *
 ***********************************************************************/


void PrintResult(Bits *bits,int num_units, int num)
{
    printf("# intra   : %d\n", bits->no_intra/num_units);
    printf("# inter   : %d\n", bits->no_inter/num_units);
    printf("# inter4v : %d\n", bits->no_inter4v/num_units);
    printf("--------------\n");
    printf("Coeff_Y: %d\n", bits->Y/num);
    printf("Coeff_C: %d\n", bits->C/num);
    printf("Vectors: %d\n", bits->vec/num);
    printf("CBPY   : %d\n", bits->CBPY/num);
    printf("MCBPC  : %d\n", bits->CBPCM/num);
    printf("MODB   : %d\n", bits->MODB/num);
    printf("CBPB   : %d\n", bits->CBPB/num);
    printf("COD    : %d\n", bits->COD/num);
    printf("DQUANT : %d\n", bits->DQUANT/num);
    printf("header : %d\n", bits->header/num);
    printf("==============\n");
    printf("Total  : %d\n", bits->total/num);
    printf("\n");
    return;
}

void PrintSNR(Results *res, int num)
{
    printf("SNR_Y  : %.2f\t", res->SNR_l/num);
    printf("SNR_Cb : %.2f\t", res->SNR_Cb/num);
    printf("SNR_Cr : %.2f\t", res->SNR_Cr/num);
    printf("--------------\t");
    return;
}

