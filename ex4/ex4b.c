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
  //   2D MEDIAN FILTER implementation
//     image  - input image
//     result - output image
//     N      - width of the image
//     M      - height of the image
  void _medianfilter(unsigned char* image, unsigned char* result, int N, int M)
  {
     for (int m = 2; m < M-2; ++m)
        for (int n = 2; n < N-2 ; ++n)
        {
           int k = 0;
           unsigned char window[25];
           for (int j = m - 2; j < m + 3; ++j)
              for (int i = n - 2; i < n + 3; ++i)
                 window[k++] = image[j * N + i];
           for (int j = 0; j < 13; ++j)
           {
              int min = j;
              for (int l = j + 1; l < 25; ++l)
              if (window[l] < window[min])
                 min = l;
            unsigned char temp = window[j];
              window[j] = window[min];
              window[min] = temp;
           }
           result[(m - 1) * (N) + n - 1] = window[12];
        }
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
  unsigned char *new_image;
  read_JPEG_file(argv[1], &width, &height, &channels, &new_image);
  int i;
  int len = width*height*channels;
  int c = 0;


  int histogram[256] = {0};
  for(int i = 0; i<=len; i++)
  {
    int p = image[i];
    histogram[p]++;
  }
  int threshold = otsu_threshold(histogram,len);
  printf ("%d\n",threshold);
  int count=0;
  for(i = 0;i<len;i++)
  {
    if(image[i]>threshold){
        image[i]=255;
    }
    else
      image[i]=0;
    count++;
  }
  printf ("%d\n",count);
  // this is where you'll insert your code to midify the image

  _medianfilter(image,new_image,width*channels,height);
  write_JPEG_file(argv[2], width, height, channels, new_image, 95);
  //_medianfilter(image, new_image, width, height);
  //write_JPEG_file(argv[3], width, height, channels, new_image, 95);
  return 0;
}
