/**
 * @file LCDjpeg.h
 * @author EdwardSu (zentialeds@gmail.com)
 * @brief func utils to display img
 * @version 0.7
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LCDJPEG__
#define __LCDJPEG__
// #define DEBUG
#include <jconfig.h>
#include <jerror.h>
#include <setjmp.h>
#include <jpeglib.h>
#include "LCDutils.h"

typedef enum direct{
    H,
    V,
}Direct;

typedef enum BGcolor{
    WHITE,
    BLACK,
}BGcolor;

typedef struct LCDjpeg{
    int height;
    int width;
    int num_channel;
    Direct img_direction;
    unsigned char *datas;
}LCDjpeg;

/**
 * @brief error struct for jpeg
 * 
 */
struct lcdjpeg_error{

    struct jpeg_error_mgr pub; /* "public" fields */

    jmp_buf setjmp_buffer; /* for return to caller */
};

typedef struct lcdjpeg_error *lcdjpeg_error;


LCDjpeg *LCDjpeg_new(int h,int w,int num_channel);
void LCDjpeg_destory(LCDjpeg *j);
LCDjpeg *LCDjpeg_read(const char *path);
unsigned char *LCDJPEG_Stretch_Linear(int w_Dest, int h_Dest, int bit_depth, unsigned char *src, int w_Src, int h_Src);
int LCDjpeg_calc_size(LCDjpeg *j,Region r,Direct d);
LCDjpeg *LCDjpeg_resize_fit(LCDjpeg *inputFile,Region r);
void __write_img(LCDjpeg *j,Region r,Screen s);
void LCDjpeg_print_to_screen(LCDjpeg *j,Screen s,Region r);
void __get_next_line(unsigned char * buffer, unsigned char * A, int line_count,int line_length);

#endif



    // if (inputFile == NULL || outputFile == NULL)
    //     return false;

    // // 读取jpeg图片像素数组
    // int w = 0, h = 0;
    // unsigned char *buff = LCDjpeg_read(inputFile, w, h);
    // if (buff == NULL)
    // {
    //     printf("ReadJpeg Failed\n");
    //     return false;
    // }

    // // 缩放图片，缩放后的大小为(tb_w,tb_h)
    // int tb_w = 160, tb_h = 160;
    // unsigned char *img_buf = LCDJPEG_Stretch_Linear(tb_w, tb_h, 24, buff, w, h);
    // free(buff);

    // // 将缩放后的像素数组保存到jpeg文件
    // bool bRetWrite = write_JPEG_file(outputFile, img_buf, 10, tb_w, tb_h);
    // delete[] img_buf;

    // if (bRetWrite)
    // {
    //     return true;
    // }
    // else
    // {
    //     printf("GenerateImageThumbnail: write failed\n");
    //     return true;
    // }
// bool write_JPEG_file(const char *filename, unsigned char *image_buffer, int quality, int image_height, int image_width)
// {

//     if (filename == NULL || image_buffer == NULL)
//         return false;

//     struct jpeg_compress_struct cinfo;
//     struct jpeg_error_mgr jerr;
//     FILE *outfile;           /* target file */
//     JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */
//     int row_stride;          /* physical row width in image buffer */
//     cinfo.err = jpeg_std_error(&jerr);
//     /* Now we can initialize the JPEG compression object. */
//     jpeg_create_compress(&cinfo);

//     if ((outfile = fopen(filename, "wb")) == NULL)
//     {
//         fprintf(stderr, "can't open %s\n", filename);
//         return false;
//     }
//     jpeg_stdio_dest(&cinfo, outfile);

//     cinfo.image_width = image_width; /* image width and height, in pixels */
//     cinfo.image_height = image_height;
//     cinfo.input_components = 3;     /* # of color components per pixel */
//     cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

//     jpeg_set_defaults(&cinfo);
//     jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

//     jpeg_start_compress(&cinfo, TRUE);

//     row_stride = image_width * 3; /* JSAMPLEs per row in image_buffer */

//     while (cinfo.next_scanline < cinfo.image_height)
//     {
//         row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
//         (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
//     }

//     jpeg_finish_compress(&cinfo);
//     fclose(outfile);

//     jpeg_destroy_compress(&cinfo);

//     return true;
// }