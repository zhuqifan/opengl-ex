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


int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: ./ex1 image_in.jpg image_out.jpg\n");
    return 1;
  }

  unsigned char *image;
  int width, height, channels;
  read_JPEG_file(argv[1], &width, &height, &channels, &image);
int threshold = 128;
size_t len = strlen(image);
  for(int i = 0;i<len;i++)
  {
    if(image[i]>threshold){
        image[i]=255;
    }
    else
      image[i]=0;
  }
  // this is where you'll insert your code to midify the image


  write_JPEG_file(argv[2], width, height, channels, image, 95);

  return 0;
}
