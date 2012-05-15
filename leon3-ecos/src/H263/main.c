/************************************************************************
 *
 *  main.c, main module of tmn (TMN encoder).
 *  tmn is an H.263 encoder somewhat based on the Test Model Near-term
 *  (TMN5) in the ITU-T LBC Experts Group.
 *
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
 *
 */

/*
 * xiaoyang yi@HIT
 * 
 * The I/O has been modified so as to make it work on embedded devices like
 * DE2-70.
 */

#include "sim.h"

#if IF_ON_PC
FILE *streamfile;
#endif
#include "image.h"


#define RAW2STATIC	YES
#if RAW2STATIC
FILE* raw;
#endif

void main_loop(int argc, char *argv[]);

/*
 * must be used after the initialization of pels and lines 
 */
void load_buffer(char* filename)
{
  char* buffer_pos = mBuffer.stream_input;
  int im_size = pels*lines*3/2;
  int status;

#if IF_ON_PC
  //FILE *im_file = NULL;
  mBuffer.output_size = 0;
  memset(mBuffer.stream_output,0,MEM_OUT_SIZE);

  mBuffer.stream_input = img_raw;
  mBuffer.input_size = 9*im_size;
  /*memset(mBuffer.stream_input,0,MEM_BUFFER_SIZE);
	if ((im_file = fopen(filename,"rb")) == NULL) {
		printf("stderr:Unable to open image_file: %s\n",filename);
		exit(-1);
	}
	rewind(im_file);
	 while ((status = fread(buffer_pos, sizeof(char), im_size, im_file)) > 0) 
	 {
		 buffer_pos += status;
		 mBuffer.input_size += status;
	 }
	
  fclose(im_file);*/
#else
  /*i have just load 9 frames.you can modify it in raw2static()*/
  mBuffer.output_size = 0;
  mBuffer.input_size = 9*im_size;
  mBuffer.stream_input = img_raw;
  memset(mBuffer.stream_output,0,MEM_OUT_SIZE);
#endif
  return ;
}
/*
 * 内存数据dump到文件bb-out.263
 *
 */
void achieve_output()
{
	/*
	FILE* fout = fopen ("bb-out.263", "wb");
	int i = 0;
	if (fout == NULL)
	{
		printf("stderr:Unable to open streamfile\n");
		exit(-1);
	}
	for (i = 0; i < mBuffer.output_size;i++)
	{
		putc(mBuffer.stream_output[i],fout);
	}
	fclose(fout);
	*/
}
/*
 * xiaoyang yi@2010.12.22
 * 
 * 此函数用于从yuv视频文件中提取固定帧数（10帧）数据到一个名为
 * img_raw的数组中。每一帧在数组中占pels*lines*3/2个元素。
 * 视频源格式默认为qcif：176×144
 */
void raw2static()
{
	/*
	int a_raw_size;
	int j = 0;
	unsigned char *image = NULL;
	int frameno = 0;

	raw = fopen("image_array.h","w+");
	if(raw == NULL){
		printf("stderr:raw fopen error!");
		return ;
	}
	
	fprintf(raw,"#ifndef _IMG_RAW_H \n#define _IMG_RAW_H\n");
	//fprintf(raw,"int raw_size = %d;\n");
	fprintf(raw,"unsigned char img_raw[]={\n");

	for(frameno = 1; frameno <= 10; frameno++){
		//read size:sizeof(char)*im_size
		image = ReadImage("grandma_qcif.yuv",frameno,0);
		a_raw_size = pels*lines*3/2;
		for( j = 0; j < a_raw_size; j++){
			fprintf(raw,"0x%x,",image[j]);
			if ((j+1)%0x10==0)
			{
				fprintf(raw,"\n");
			}
			
		}
		
		fprintf(raw,"\n\n\/*one img finished!*\/\n\n");
		free(image);
		image = NULL;
	}
	
	fprintf(raw,"};\n#endif");
	fclose(raw);
	*/
}

/*
 * 本函数提供任务给主模块进行调用
 * 提供的参数有2个：一个是存放源信息的内存地址，一个sahib存放压缩后的
 * 信息地址
 */
int video_task()
{
}

int main(int argc, char *argv[])
{
	char* str[9] = {
		"tmn",
		"-i",
		"grandma_qcif.yuv",
		"-a",
		"1",
		"-b",
		"9",
		"-B",
		"out-h263.avi"
	};

	main_loop(9,str);
	raw2static();
	getchar();
}
//--------------------------------------------------
void main_loop(int argc, char *argv[])
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
#if IF_ON_PC
    FILE *cleared;
#endif

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

#ifndef OFFLINE_RATE_CONTROL
    float DelayBetweenFramesInSeconds;
    int CommBacklog;
#else
    PictImage *stored_image = NULL;
    int start_rate_control;
#endif

    extern int arith_used;

    printf("\nTMN (H.263) coder version 2.0, Copyright (C) 1995, 1996 Telenor R&D, Norway\n");
	printf("argc:%d\n",argc);
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
#ifdef OFFLINE_RATE_CONTROL
    start_rate_control = DEF_START_RATE_CONTROL;
#else
    pic->target_frame_rate = (float)DEF_TARGET_FRAME_RATE;
#endif

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
            case 'S':
                chosen_frameskip = atoi(argv[++i]) + 1;
                break;
            case 'O':
                orig_frameskip = atoi(argv[++i]) + 1;
                break;
            case 's':
                pic->seek_dist = atoi(argv[++i]);
                break;
            case 'o':
                strcpy(outputfile, argv[++i]);
                break;
            case 'e':
                headerlength = atoi(argv[++i]);
                break;
            case 'm':
                write_repeated = ON;
                break;
            case 'i':
                strcpy(seqfilename, argv[++i]);
                break;
            case 'q':
                QP = atoi(argv[++i]);
                if (QP > 31 || QP < 0)
                {
                    printf("stderr:QP out of range - clipping it\n");
                    QP = mmin(31,mmax(0,QP));
                }
                break;
            case 'I':
                QPI = atoi(argv[++i]);
                if (QPI > 31 || QPI < 0)
                {
                    printf("stderr:QP out of range - clipping it\n");
                    QPI = mmin(31,mmax(0,QPI));
                }
                break;
            case 'w':
                writediff = ON;
                break;
            case 'B':
                strcpy(streamname, argv[++i]);
                break;
            case 'h':
               // Help();
                exit(0);
                break;
            case 'H':
               // AdvancedHelp();
                exit(0);
                break;
            case 't':
                trace = 1;
                break;
            case 'g':
                pic->use_gobsync = atoi(argv[++i]);;
                break;
            case 'D':
                /* note that the Unrestricted Motion Vector mode turns on
                   both long_vectors and mv_outside_frame */
                pic->unrestricted_mv_mode = ON;
                mv_outside_frame = ON;
                long_vectors = ON;
                break;
            case 'E':
                syntax_arith_coding = ON;
                break;
            case 'F':
                /* note that the Advanced Prediction mode turns on both
                   advanced (8x8 vectors and OBMC) and mv_outside_frame */
                /* the Extended Motion Vector mode is included in the
                   Unrestricted Motion Vector mode, which of course can be
                   use together with the Advanced Prediction mode */
                advanced = ON;
                mv_outside_frame = ON;
                break;
            case 'G':
                pb_frames = ON;
                break;
            case 'Q':
                pic->BQUANT = atoi(argv[++i]);
                break;
            case 'r':
                targetrate = atoi(argv[++i]);
                break;
#ifdef OFFLINE_RATE_CONTROL
            case 'R':
                start_rate_control = atoi(argv[++i]);
                break;
#else
            case 'R':
                pic->target_frame_rate = (float)atof(argv[++i]);
                break;
#endif
            case 'Z':
                ref_frame_rate = (float)atoi(argv[++i]);
                break;
            case 'x':
                pic->source_format =  atoi(argv[++i]);
                break;
            default:
                printf("stderr:Illegal option: %c\n",*argv[i]);
                //Help();
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
    case (SF_CIF):
        printf( "Encoding format: CIF (352x288)\n");
        pels = 352;
        lines = 288;
        break;
    case (SF_4CIF):
        printf( "Encoding format: 4CIF (704x576)\n");
        pels = 704;
        lines = 576;
        break;
    case (SF_16CIF):
        printf( "Encoding format: 16CIF (1408x1152)\n");
        pels = 1408;
        lines = 1152;
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

#ifndef OFFLINE_RATE_CONTROL
    /* rate control variables */
    pic->bit_rate = targetrate;
    pic->src_frame_rate = (int)(ref_frame_rate / orig_frameskip);
    DelayBetweenFramesInSeconds = (float) 1.0/(float)pic->src_frame_rate;
    InitializeRateControl();
#endif

    if (QP == 0 || QPI == 0)
    {
        printf("stderr:Warning:");
        printf("stderr:QP is zero. Bitstream will not be correctly decodable\n");
    }

    if (ref_frame_rate != 25.0 && ref_frame_rate != 30.0)
    {
        printf("stderr:Warning: Reference frame rate should be 25 or 30 fps\n");
    }

    frame_rate =  ref_frame_rate / (float)(orig_frameskip * chosen_frameskip);
#ifdef OFFLINE_RATE_CONTROL
    printf("Encoding frame rate  : %.2f\n", frame_rate);
#else
    if (pic->bit_rate == 0)
        printf("Encoding frame rate  : %.2f\n", frame_rate);
    else
        printf("Encoding frame rate  : variable\n");
#endif
    printf("Reference frame rate : %.2f\n", ref_frame_rate);
    printf("Orig. seq. frame rate: %.2f\n\n",
            ref_frame_rate / (float)orig_frameskip);

    /* Open stream for writing */
#if IF_ON_PC
    streamfile = fopen (streamname, "wb");
    if (streamname == NULL)
    {
        printf("stderr:Unable to open streamfile\n");
        exit(-1);
    }
#endif

	/*
	 * yixiaoyang add
	 */
	load_buffer(seqfilename);
    /* Initialize bitcounters */
    initbits ();
#if IF_ON_PC
    //if (trace)
    //{
    //    strcpy(tracefile, "trace.intra");
    //    /* Open trace-file for writing */
    //    if ((tf = fopen(tracefile,"w")) == NULL)
    //    {
    //        printf("stderr:Unable to open tracefile (intra)\n");
    //        exit(-1);
    //    }
    //}
    /* Clear output files */
    if ((cleared = fopen(outputfile,"wb")) == NULL)
    {
        printf("stderr:Couldn't open outputfile: %s\n",outputfile);
        exit(-1);
    }
    else
        fclose(cleared);

    if (writediff)
    {
        if ((cleared = fopen(diff_filename,"wb")) == NULL)
        {
            printf("stderr:Couldn't open diff-file: %s\n",diff_filename);
            exit(-1);
        }
        else
            fclose(cleared);
    }
#endif
    /* Intra image */
    image = ReadImage(seqfilename,start,headerlength);

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
#if IF_COMPUTE_SNR
    ComputeSNR(curr_image, curr_recon, res, writediff);
	AddBitsPicture(bits);
    PrintSNR(res, 1);
    PrintResult(bits, 1, 1);
#endif
    memcpy(intra_bits,bits,sizeof(Bits));
    ZeroBits(total_bits);
    ZeroRes(total_res);
    ZeroRes(b_res);

    /* number of seconds to encode */
    seconds = (end - start + chosen_frameskip) * orig_frameskip/ ref_frame_rate;
#if IF_ON_PC
    //if (trace)
    //{
    //    strcpy(tracefile, "trace");
    //    fclose(tf);
    //    /* Open trace-file for writing */
    //    if ((tf = fopen(tracefile,"w")) == NULL)
    //    {
    //        printf("stderr:Unable to open tracefile (non-intra)\n");
    //        exit(-1);
    //    }
    //}
#endif
    /* compute first frameskip */
#ifdef OFFLINE_RATE_CONTROL
    first_frameskip = chosen_frameskip;
    frameskip = chosen_frameskip;
#else
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
#endif

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

#ifndef OFFLINE_RATE_CONTROL
        if (pic->bit_rate != 0 && pic->PB)
            CommBacklog -= (int)
                           ( DelayBetweenFramesInSeconds*pic->bit_rate ) * pdist;

        if (pic->bit_rate != 0)
        {
            UpdateRateControl(bits->total);

            CommBacklog += bits->total;
            frameskip = 1;
            CommBacklog -= (int)
                           (frameskip * DelayBetweenFramesInSeconds *pic->bit_rate);

            while ( (int)(DelayBetweenFramesInSeconds*pic->bit_rate) <= CommBacklog)
            {
                CommBacklog -= (int) ( DelayBetweenFramesInSeconds * pic->bit_rate );
                frameskip += 1;
            }
        }
#else
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
#endif

        if (pic->PB)
        {
            if (write_repeated)
                wcopies = pdist;
            for (i = 0; i < wcopies; i++)
                WriteImage(B_recon,outputfile); /* write wcopies frames to disk */
             /*
             * yixiaoyang add
             * ComputeSNR 将会被去掉。计算信噪比不出现在实际应用代码里面。
             */
#if IF_COMPUTE_SNR
			ComputeSNR(B_image, B_recon, res, writediff);
            printf("Results for B-frame:\n");
            AddRes(b_res,res,pic);
            PrintSNR(res, 1);
#endif
            FreeImage(B_image);
            FreeImage(B_recon);
        }

        if (write_repeated)
            wcopies = (pb_frames ? bdist : frameskip);
        for (i = 0; i < wcopies; i++)
            WriteImage(curr_recon,outputfile); /* write wcopies frames to disk */

        if (pb_frames)
            pic->PB = 1;
#if IF_COMPUTE_SNR
        ComputeSNR(curr_image, curr_recon, res, writediff);
        printf("Results for P-frame:\n");
        AddRes(total_res,res,pic);
        PrintSNR(res, 1);
        PrintResult(bits, 1, 1);
#endif
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
#if IF_ON_PC
    fclose (streamfile);
#endif
    //if (trace)
    //{
    //    fclose(tf);
    //}

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

#if 0
        printf("Total frames     : %3d (%3d)\n",
                total_frames_passed + first_frameskip,
                total_frames_passed);
#endif

        printf("Encoded frames   : %3d (%3d)\n",
                frames + 1,
                frames);

        mean_frame_rate = frames  / (float)total_frames_passed *
                          ref_frame_rate / (float)orig_frameskip;

        printf("Mean frame rate  : %.2f Hz\n", mean_frame_rate);

        if (targetrate != 0)
            printf("Target bit rate  : %.2f kbit/sec\n",
                    targetrate/1000.0);

        printf("Obtained bit rate: %.2f (%.2f) kbit/sec\n",
                (total_bits->total + intra_bits->total) /
                ((total_frames_passed + first_frameskip) /
                 ref_frame_rate * orig_frameskip)/1000.0,
                (total_bits->total / (float)frames) * mean_frame_rate/1000.0);

        printf("============================================\n");

    }
#if 0
    printf("Total number of bits: %d (%d)\n",
            total_bits->total + intra_bits->total,
            (total_bits->total + intra_bits->total) / 8);
#endif

#if RAW2STATIC
	raw2static();
#endif
	//xiaoyang yi add
	achieve_output();
	
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
 *	Name:        Help
 *	Description:        Prints usage
 *
 *
 ***********************************************************************/
/*
 * yixiaoyang add
 * 可删掉下面四个函数
 */
/*
void Help()
{
    printf("Usage:\ttmn [options] -i <filename> [more options]\n");
    printf("Options:\n");
    printf("\t-i <filename> original sequence [required parameter]\n");
    printf("\t-o <filename> reconstructed frames [%s]\n",
            DEF_OUTFILENAME);
    printf("\t-B <filename> filename for bitstream [%s]\n",
            DEF_STREAMNAME);
    printf("\t-a <n> image to start at [%d]\n",
            DEF_START_FRAME);
    printf("\t-b <n> image to stop at [%d]\n",
            DEF_STOP_FRAME);
    printf("\t-x <n> coding format [%d]\n",DEF_CODING_FORMAT);
    printf("\t-q <n> (1..31) quantization parameter QP [%d]\n",
            DEF_INTER_QUANT);
    printf("\t-I <n> (1..31) QP for first frame [%d]\n",
            DEF_INTRA_QUANT);
    printf("\t-r <n> target bitrate in bits/s, default is variable bitrate\n");
    printf("\t-S <n> frames to skip between each encoded frame [%d]\n",
            DEF_FRAMESKIP);
    printf("\t-D use unrestricted motion vector mode (annex D) [%s]\n",
            DEF_UMV_MODE ? "ON" : "OFF");
    printf("\t-E use syntax-based arithmetic coding (annex E) [%s]\n",
            DEF_SAC_MODE ? "ON" : "OFF");
    printf("\t-F use advanced prediction mode (annex F) [%s]\n",
            DEF_ADV_MODE ? "ON" : "OFF");
    printf("\t-G use PB-frames (annex G) [%s]\n",
            DEF_PBF_MODE ? "ON" : "OFF");
    printf("\t-h Prints simple help\n");
    printf("\t-H Prints advanced help\n");
    printf("\n\tDefault filenames and other options in square brackets \n\tare chosen in config.h\n");
    return;
}
*/
/*
void AdvancedHelp()
{
    printf("Usage:\ttmn [options] -i <filename> [more options]\n");
    printf("Options:\n");
    printf("\t-i <filename> original sequence [required parameter]\n");
    printf("\t-o <filename> reconstructed frames [%s]\n",
            DEF_OUTFILENAME);
    printf("\t-B <filename> filename for bitstream [%s]\n",
            DEF_STREAMNAME);
    printf("\t-a <n> image to start at [%d]\n",
            DEF_START_FRAME);
    printf("\t-b <n> image to stop at [%d]\n",
            DEF_STOP_FRAME);
    printf("\t-x <n> coding format [%d]\n",DEF_CODING_FORMAT);
    printf("\t       n=1: SQCIF n=2: QCIF n=3: CIF n=4: 4CIF n=5: 16CIF\n");
    printf("\t           128x96   176x144  352x288   704x576  1408x1152\n");
    printf("\t-s <n> (0..15) integer pel search window [%d]\n",
            DEF_SEEK_DIST);
    printf("\t-q <n> (1..31) quantization parameter QP [%d]\n",
            DEF_INTER_QUANT);
    printf("\t-I <n> (1..31) QP for first frame [%d]\n",
            DEF_INTRA_QUANT);
    printf("\t-r <n> target bitrate in bits/s, default is variable bitrate\n");
#ifdef OFFLINE_RATE_CONTROL
    printf("\t   -R <n> start rate control after n%% of sequence [%d]\n",
            DEF_START_RATE_CONTROL);
#else
    printf("\t   -R <f> target frame rate [%.2f]\n",
            DEF_TARGET_FRAME_RATE);
#endif
    printf("\t-S <n> frames to skip between each encoded frame [%d]\n",
            DEF_FRAMESKIP);
    printf("\t-Z <n> reference frame rate (25 or 30 fps) [%.1f]\n",
            DEF_REF_FRAME_RATE);
    printf("\t-O <n> frames skipped in original compared to reference frame rate [%d]\n", DEF_ORIG_SKIP);
    printf("\t-e <n> original sequence has n bytes header [%d]\n",
            DEF_HEADERLENGTH);
    printf("\t-g <n> insert sync after each n GOB (slice) [%d]\n",
            DEF_INSERT_SYNC);
    printf("\t       zero above means no extra syncs inserted\n");
    printf("\t-w write difference image to file \"%s\" [%s]\n",
            DEF_DIFFILENAME,
            DEF_WRITE_DIFF ? "ON" : "OFF");
    printf("\t-m write repeated reconstructed frames to disk [%s]\n",
            DEF_WRITE_REPEATED ? "ON" : "OFF");
    printf("\t-t write trace to tracefile trace.intra/trace [%s]\n",
            DEF_WRITE_TRACE ? "ON" : "OFF");
    printf("\t-D use unrestricted motion vector mode (annex D) [%s]\n",
            DEF_UMV_MODE ? "ON" : "OFF");
    printf("\t-E use syntax-based arithmetic coding (annex E) [%s]\n",
            DEF_SAC_MODE ? "ON" : "OFF");
    printf("\t-F use advanced prediction mode (annex F) [%s]\n",
            DEF_ADV_MODE ? "ON" : "OFF");
    printf("\t-G use PB-frames (annex G) [%s]\n",
            DEF_PBF_MODE ? "ON" : "OFF");
    printf("\t   -Q <n> (0..3) BQUANT parameter [%d]\n",DEF_BQUANT);
    printf("\t-h Prints simple help\n");
    printf("\t-H Prints advanced help\n");
    printf("\n\tDefault filenames and other options in square brackets \n\tare chosen in config.h\n");
    return;
}
*/
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
    printf("SNR_Y  : %.2f\n", res->SNR_l/num);
    printf("SNR_Cb : %.2f\n", res->SNR_Cb/num);
    printf("SNR_Cr : %.2f\n", res->SNR_Cr/num);
    printf("--------------\n");
    return;
}

