#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <string.h>


/** Read the JPEG image at `filename` as an array of bytes.
  Data is returned through the out pointers, while the return
  value indicates success or failure.
  NOTE: 1) if image is RGB, then the bytes are concatenated in R-G-B order
        2) `image` should be freed by the user
 */
static inline int
read_JPEG_file(char *filename,
               int *width, int *height, int *channels, unsigned char *(image[]))
{
  FILE *infile;
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  (void) jpeg_read_header(&cinfo, TRUE);
  (void) jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width, *height = cinfo.output_height;
  *channels = cinfo.num_components;
  // printf("width=%d height=%d c=%d\n", *width, *height, *channels);
  *image = malloc(*width * *height * *channels * sizeof(*image));
  JSAMPROW rowptr[1];
  int row_stride = *width * *channels;

  while (cinfo.output_scanline < cinfo.output_height) {
    rowptr[0] = *image + row_stride * cinfo.output_scanline;
    jpeg_read_scanlines(&cinfo, rowptr, 1);
  }
  jpeg_finish_decompress(&cinfo);

  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  return 1;
}


/** Writes the image in the specified file.
  NOTE: works with Grayscale or RGB modes only (based on number of channels)
 */
static inline void
write_JPEG_file(char *filename, int width, int height, int channels,
                unsigned char image[], int quality)
{
  FILE *outfile;
  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }

  struct jpeg_error_mgr jerr;
  struct jpeg_compress_struct cinfo;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo,outfile);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = channels;
  cinfo.in_color_space = channels == 1 ? JCS_GRAYSCALE : JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);

  jpeg_start_compress(&cinfo, TRUE);
  JSAMPROW rowptr[1];
  int row_stride = width * channels;
  while (cinfo.next_scanline < cinfo.image_height) {
    rowptr[0] = & image[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, rowptr, 1);
  }
  jpeg_finish_compress(&cinfo);

  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
}

unsigned char otsu_threshold( int* histogram, int pixel_total )
  {
    unsigned int sumB = 0;
    unsigned int sum1 = 0;
    float wB = 0.0f;
    float wF = 0.0f;
    float mF = 0.0f;
    float max_var = 0.0f;
    float inter_var = 0.0f;
    unsigned char threshold = 0;
    unsigned short index_histo = 0;

    for ( index_histo = 1; index_histo < 256; ++index_histo )
    {
        sum1 += index_histo * histogram[ index_histo ];
    }

    for (index_histo = 1; index_histo < 256; ++index_histo)
    {
        wB = wB + histogram[ index_histo ];
        wF = pixel_total - wB;
        if ( wB == 0 || wF == 0 )
        {
            continue;
        }
        sumB = sumB + index_histo * histogram[ index_histo ];
        mF = ( sum1 - sumB ) / wF;
        inter_var = wB * wF * ( ( sumB / wB ) - mF ) * ( ( sumB / wB ) - mF );
        if ( inter_var >= max_var )
        {
            threshold = index_histo;
            max_var = inter_var;
        }
    }

    return threshold;
  }

int dilation(int width, int height, unsigned char image[],int len)
{
  int count = 0;
  int current;
  int zeros[len];
  memset( zeros, -1, len*sizeof(int) );
  for(int i = 0;i<len;i++)
  {
    if(image[i]==0)
    {
     zeros[count] = i;
     count++;
    }
  }
  for(int j = 0;j<len;j++)
  {
    if(zeros[j] != -1)
    {
      current = zeros[j];
      if((current+1)<=len)
      image[current+1]=0;
      if(current-1>=0)
      image[current-1]=0;
      if(current+width<=len)
      image[current+width]=0;
      if(current-width>=0)
      image[current-width]=0;

      if((current+1+width)<=len)
      image[current+1+width]=0;
      if(current-1+width<=len)
      image[current-1+width]=0;
      if(current-width+1>=0)
      image[current-width+1]=0;
      if(current-width-1>=0)
      image[current-width-1]=0;
    }
  }
  return 0;
}

int erosion(int width, int height, unsigned char image[],int len)
{
  int count = 0;
  int current;
  int ones[len];
  memset( ones, -1, len*sizeof(int) );
  for(int i = 0;i<len;i++)
  {
    if(image[i]==255)
    {
     ones[count] = i;
     count++;
    }
  }
  for(int j = 0;j<len;j++)
  {
    if(ones[j] != -1)
    {
      current = ones[j];
      if((current+1)<=len)
      image[current+1]=255;
      if(current-1>=0)
      image[current-1]=255;
      if(current+width<=len)
      image[current+width]=255;
      if(current-width>=0)
      image[current-width]=255;

      if((current+1+width)<=len)
      image[current+1+width]=255;
      if(current-1+width<=len)
      image[current-1+width]=255;
      if(current-width+1>=0)
      image[current-width+1]=255;
      if(current-width-1>=0)
      image[current-width-1]=255;
    }
  }
  return 0;
}
int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: ./ex1 image_in.jpg image_out.jpg\n");
    return 1;
  }

  unsigned char *image;
  int width, height, channels;
  read_JPEG_file(argv[1], &width, &height, &channels, &image);
  int i;
  size_t len = strlen(image);
  int histogram[256] = {0};
  for(int i = 0; i<=len; i++)
  {
    int p = image[i];
  //  printf("%d\n", image[i]);
    histogram[p]++;
  }
  int threshold = otsu_threshold(histogram,len);
  for(i = 0;i<len;i++)
  {
    if(image[i]>threshold){
        image[i]=255;
    //    printf("%d\n", image[i]);
    }
    else
      image[i]=0;
  }
  // this is where you'll insert your code to midify the image


  write_JPEG_file(argv[2], width, height, channels, image, 95);
  erosion(width,height,image,len);
  write_JPEG_file(argv[3], width, height, channels, image, 95);
  return 0;
}
