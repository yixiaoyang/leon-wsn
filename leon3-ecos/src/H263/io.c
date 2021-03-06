/************************************************************************
 *
 *  io.c, part of tmn (TMN encoder)
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


#include"sim.h"

/**********************************************************************
 *
 *	Name:        ReadImage        
 *	Description:	Reads one raw image from disk
 *	
 *	Input:        filename of sequence, frame no. to be read,
 *        headerlength of sequence
 *	Returns:	Pointer to start of raw YUV-data
 *	Side effects:	Memory allocated to image-data
 *
 *	Date: 940108	Author:	Karl.Lillevold@nta.no
 *
 ***********************************************************************/

//unsigned char *ReadImage(char *filename, int frame_no, int headerlength)
//{
//  FILE *im_file = NULL;
//  int im_size = pels*lines*3/2;
//  unsigned char *raw;
//  int status;
//
//  if ((raw = (unsigned char *)malloc(sizeof(char)*im_size)) == NULL) {
//    printf("stderr:Couldn't allocate memory to image\n");
//    exit(-1);
//  }
//  if ((im_file = fopen(filename,"rb")) == NULL) {
//    printf("stderr:Unable to open image_file: %s\n",filename);
//    exit(-1);
//  }
//  rewind(im_file);
//  ///* Find the correct image */
//  status = fseek(im_file,headerlength + (frame_no) * im_size,0);
//  if (status != 0) {
//    printf("stderr:Error in seeking image no: %d\n",frame_no);
//    printf("stderr:From file: %s\n",filename);
//    exit(-1);
//  }
//  /* Read image */
//  printf("Reading image no: %d\n",frame_no);
//  if ((status = fread(raw, sizeof(char), 
//              im_size, im_file)) != im_size) {
//    printf("stderr:Error in reading image no: %d\n",frame_no);
//    printf("stderr:From file: %s\n",filename);
//    exit(-1);
//  }
//
//  fclose(im_file);
//  return raw;
//}
/*
 * xiaoyang yi@2010.12.18
 * overwrite ReadImage()
 */
unsigned char *ReadImage(char *filename, int frame_no, int headerlength)
{
	int im_size = pels*lines*3/2;
	unsigned char *raw;
	int status;
	int pos;

	if ((raw = (unsigned char *)malloc(sizeof(char)*im_size)) == NULL) {
		printf("stderr:Couldn't allocate memory to image\n");
		exit(-1);
	}
	
	/* Find the correct image */
	pos = headerlength + (frame_no) * im_size;
	//status = fseek(headerlength + (frame_no) * im_size,0);
	if ( pos > mBuffer.input_size) {
		printf("stderr:Error in seeking image no: %d\n",frame_no);
		printf("stderr:From file: %s\n",filename);
		printf("debug pos=%d,input.size=%d\n",pos,mBuffer.input_size);
		exit(-1);
	}
	/* Read image */
	printf("Reading image no: %d\n",frame_no);
	//raw = mBuffer.stream_input + pos;
	memcpy(raw,mBuffer.stream_input+pos,im_size*sizeof(char));
	//if ((status = fread(raw, sizeof(char), 
	//	im_size, im_file)) != im_size) {
	//		printf("stderr:Error in reading image no: %d\n",frame_no);
	//		printf("stderr:From file: %s\n",filename);
	//		exit(-1);
	//}

	//fclose(im_file);
	return raw;
}

/**********************************************************************
 *
 *	Name:        FillImage
 *	Description:	fills Y, Cb and Cr of a PictImage struct
 *	
 *	Input:        pointer to raw image
 *        
 *	Returns:	pointer to filled PictImage
 *	Side effects:	allocates memory to PictImage
 *                      raw image is freed
 *
 *	Date: 940109	Author:	Karl.Lillevold@nta.no
 *
 ***********************************************************************/

PictImage *FillImage(unsigned char *in)
{
  PictImage *Pict;

  Pict = InitImage(pels*lines);

  memcpy(Pict->lum, in, pels*lines);
  memcpy(Pict->Cb, in + pels*lines, pels*lines/4);
  memcpy(Pict->Cr, in + pels*lines + pels*lines/4, pels*lines/4);

  free(in);
  return(Pict);
}

/**********************************************************************
 *
 *	Name:        WriteImage
 *	Description:	Writes PictImage struct to disk
 *	
 *	Input:        pointer to image data to be stored, filename
 *        to be used on the disk, image size
 *	Returns:	
 *	Side effects:	
 *
 *	Date: 930115	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void WriteImage(PictImage *image, char *filename)
{
#if 0
  int status;
  FILE *f_out;

  /* Opening file */
  if ((f_out = fopen(filename,"ab")) == NULL) {
    printf("stderr:%s%s\n","Error in opening file: ",filename);
    exit(-1);
  }

  /* Writing lum to file */
  if ((status = fwrite(image->lum,sizeof(char),pels*lines,f_out)) 
      != pels*lines) {
    printf("stderr:%s%s\n","Error in writing to file: ",filename);
    exit(-1);
  }
  /* Writing Cb to file */
  if ((status = fwrite(image->Cb,sizeof(char),pels*lines/4,f_out)) 
      != pels*lines/4) {
    printf("stderr:%s%s\n","Error in writing to file: ",filename);
    exit(-1);
  }
  /* Writing Cr to file */
  if ((status = fwrite(image->Cr,sizeof(char),pels*lines/4,f_out)) 
      != pels*lines/4) {
    printf("stderr:%s%s\n","Error in writing to file: ",filename);
    exit(-1);
  }

  fclose(f_out);
#else

#endif
  return;
}


/**********************************************************************
 *
 *	Name:        InitImage
 *	Description:	Allocates memory for structure of 4:2:0-image
 *	
 *	Input:	        image size
 *	Returns:	pointer to new structure
 *	Side effects:	memory allocated to structure
 *
 *	Date: 930115        Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

PictImage *InitImage(int size)
{
  PictImage *new;

  if ((new = (PictImage *)malloc(sizeof(PictImage))) == NULL) {
    printf("stderr:Couldn't allocate (PictImage *)\n");
    exit(-1);
  }
  if ((new->lum = (unsigned char *)malloc(sizeof(char)*size)) 
      == NULL) {
    printf("stderr:Couldn't allocate memory for luminance\n");
    exit(-1);
  }
  if ((new->Cr = (unsigned char *)malloc(sizeof(char)*size/4)) 
      == NULL) {
    printf("stderr:Couldn't allocate memory for Cr\n");
    exit(-1);
  }
  if ((new->Cb = (unsigned char *)malloc(sizeof(char)*size/4)) 
      == NULL) {
    printf("stderr:Couldn't allocate memory for Cb\n");
    exit(-1);
  }

  return new;
}


/**********************************************************************
 *
 *	Name:        FreeImage
 *	Description:	Frees memory allocated to structure of 4:2:0-image
 *	
 *	Input:        pointer to structure
 *	Returns:
 *	Side effects:	memory of structure freed
 *
 *	Date: 930115	Author: Karl.Lillevold@nta.no
 *
 ***********************************************************************/

void FreeImage(PictImage *image)

{
  free(image->lum);
  free(image->Cr);
  free(image->Cb);
  free(image);
}

