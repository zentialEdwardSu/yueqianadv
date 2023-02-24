#include "include/LCDjpeg.h"
#include "include/LCDutils.h"
#include <jconfig.h>
#include <jerror.h>
#include <jpeglib.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

/**
 * @brief create new LCD jpeg 
 * 
 * @param h height
 * @param w width
 * @param num_channel color channel (sticked 3 now)
 * @return LCDjpeg* 
 */
LCDjpeg* LCDjpeg_new(int h,int w,int num_channel){
    LCDjpeg* j = (LCDjpeg *)malloc(sizeof(LCDjpeg));
    j -> width = w;
    j -> height = h;
    j -> num_channel = 3;// only RGB
    if(j -> width > j -> height){
        j -> img_direction = H;
    }else{
        j -> img_direction = V;
    }
    // unsigned long dataSize = w * h * num_channel;
    unsigned long dataSize = w * h * 3; //do not read A
    if(j -> width !=0 && j -> height !=0){
        j -> datas = (unsigned char *)malloc(sizeof(unsigned char) *dataSize);
    }else{
        j -> datas = NULL;
    }
    return j;
}

/**
 * @brief free img that will not be used
 * 
 * @param j 
 */
void LCDjpeg_destory(LCDjpeg* j){
    free(j->datas);
    free(j);
}


/**
 * @brief Here's the routine that will replace the standard error_exit method:
 */
METHODDEF(void)
LCDjpeg_error_exit(j_common_ptr cinfo){

    /* cinfo->err really points to a lcdjpeg_error struct, so coerce pointer */
    lcdjpeg_error myerr = (lcdjpeg_error)cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @brief 
 * 
 * @param path img path
 * @return unsigned char* 
 */
LCDjpeg *LCDjpeg_read(const char *path){

    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        return NULL;
    }

    struct jpeg_decompress_struct info; // for jpeg info

    // 	struct jpeg_error_mgr err; //the error handler
    // 	info.err = jpeg_std_error(&err);

    struct lcdjpeg_error read_err;

    info.err = jpeg_std_error(&read_err.pub);
    read_err.pub.error_exit = LCDjpeg_error_exit;

    /* Establish the setjmp return context for read_error_exit to use. */
    if (setjmp(read_err.setjmp_buffer))
    {
        // clean up the JPEG object and close input file when JPEG code has signaled an error
        printf("Error occured\n");
        jpeg_destroy_decompress(&info);
        fclose(file);
        return NULL;

    }

    jpeg_create_decompress(&info); // fills info structure
    jpeg_stdio_src(&info, file);   // void

    // read header and fill it into info
    int ret_Read_Head = jpeg_read_header(&info, TRUE); // int

    if (ret_Read_Head != JPEG_HEADER_OK)
    {
        printf("jpeg_read_header failed\n");
        fclose(file);
        jpeg_destroy_decompress(&info);
        return NULL;

    }

    // start decompress
    jpeg_start_decompress(&info);
    // if (!bStart)
    // {
    //     printf("jpeg_start_decompress failed\n");
    //     fclose(file);
    //     jpeg_destroy_decompress(&info);
    //     return LCDjpeg_new(0,0,0);

    // }

    // calc size
    LCDjpeg* j = LCDjpeg_new(info.output_height,info.output_width,info.num_components);
    printf("Readed img Header %d x %d , channels %d\n",j->width,j->height,j->num_channel);
    // int w = width = info.output_width;
    // int h = height = info.output_height;
    // int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
    // unsigned long dataSize = w * h * numChannels;

    // read RGB(A) scanlines one at a time into jdata[]
    // unsigned char *data = (unsigned char *)malloc(dataSize);
    if (!j->datas){
        LCDjpeg_destory(j);
        return NULL;
    }

    // read jpeg data into memory
    unsigned char * rowBuffer[1];
    while (info.output_scanline < info.output_height)
    {
		rowBuffer[0] = (unsigned char *)(&j->datas[3*info.output_width*info.output_scanline]);
		jpeg_read_scanlines(&info, rowBuffer, 1);
    }

    // finish compress and close file
    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);
    fclose(file);

    // #ifdef DEBUG
    // ! can't read size properly
    // printf("Read img %s, %d x %d total read size %ld\n",path,j->width,j->height,len(j->datas));
    // #endif
    
    return j;
}

/**
 * @brief Linear scale 
 * 
 * @param w_Dest target.width
 * @param h_Dest target.height
 * @param bit_depth 
 * @param src src.img
 * @param w_Src src.width
 * @param h_Src src.height
 * @return unsigned char* 
 */
unsigned char *LCDJPEG_Stretch_Linear(int w_Dest, int h_Dest, int bit_depth, unsigned char *src, int w_Src, int h_Src){

    int sw = w_Src - 1, sh = h_Src - 1, dw = w_Dest - 1, dh = h_Dest - 1;
    int B, N, x, y;
    int nPixelSize = bit_depth / 8;
    unsigned char *pLinePrev, *pLineNext;
    unsigned char *pDest = (unsigned char *)malloc(w_Dest * h_Dest * bit_depth / 8);
    unsigned char *tmp;
    unsigned char *pA, *pB, *pC, *pD;

    for (int i = 0; i <= dh; ++i){
        tmp = pDest + i * w_Dest * nPixelSize;
        y = i * sh / dh;
        N = dh - i * sh % dh;
        pLinePrev = src + (y++) * w_Src * nPixelSize;
        // pLinePrev =(unsigned char *)aSrc->m_bitBuf+((y++)*aSrc->m_width*nPixelSize);
        pLineNext = (N == dh) ? pLinePrev : src + y * w_Src * nPixelSize;
        // pLineNext = ( N == dh ) ? pLinePrev : (unsigned char *)aSrc->m_bitBuf+(y*aSrc->m_width*nPixelSize);
        for (int j = 0; j <= dw; ++j){
            x = j * sw / dw * nPixelSize;
            B = dw - j * sw % dw;
            pA = pLinePrev + x;
            pB = pA + nPixelSize;
            pC = pLineNext + x;
            pD = pC + nPixelSize;
            if (B == dw){
                pB = pA;
                pD = pC;
            }

            for (int k = 0; k < nPixelSize; ++k){
                *tmp++ = (unsigned char)(int)((B * N * (*pA++ - *pB - *pC + *pD) + dw * N * *pB++ + dh * B * *pC++ + (dw * dh - dh * B - dw * N) * *pD++ + dw * dh / 2) / (dw * dh));
            }
        }
    }
    return pDest;
}

/**
 * @brief calc matched size for resize
 * 
 * @param j LCDjpeg pointer
 * @param s Screen
 * @param d jpeg direction
 * @return int 
 */
int LCDjpeg_calc_size(LCDjpeg* j,Screen s,Direct d){
    int res;
    if(d == V){
        if(j -> height > s.size_y){
            res = 1.0*s.size_y / j -> height * j -> width;
        }else{
            res = j -> height;
        }
    }else{
        if(j -> width > s.size_x){
            res = 1.0*s.size_x / j -> width * j -> height;
        }else{
            res = j -> width;
        }
    }
    return res;
}

/**
 * @brief reszie the old return new which fits given screen and return the new one
 * 
 * @param inputFile input LCDjpeg
 * @param s Screen
 * @return LCDjpeg* 
 */
LCDjpeg* LCDjpeg_resize_fit(LCDjpeg* inputFile,Screen s){

    Direct f_direction = H;
    LCDjpeg *res_j;

    //TODO change
    //judge direction
    if(inputFile -> height > inputFile -> width){
        f_direction = V;
    }

    // calc matched size 
    int x_or_y = LCDjpeg_calc_size(inputFile,s,f_direction);
    printf("other is %d\n",x_or_y);

    if(f_direction == H){
        res_j = LCDjpeg_new(x_or_y,s.size_x,inputFile -> num_channel);
    }else if(f_direction == V){
        res_j = LCDjpeg_new(s.size_y,x_or_y,inputFile -> num_channel);
    }
    // free pre ordered mem
    free(res_j ->datas);

    // write resized imgae
    res_j ->datas = LCDJPEG_Stretch_Linear(res_j ->width,res_j ->height,res_j ->num_channel*8,inputFile -> datas,inputFile -> width,inputFile -> height);

    // LCDjpeg_destory(inputFile);
    #ifdef DEBUG
    printf("Origin img %d x %d , aftered resized %d x %d\n",inputFile -> width,inputFile -> height,res_j ->width,res_j ->height);
    #endif
    return res_j;

}

/**
 * @brief [internal func]write LCDjpeg to screen
 * 
 * @param j img
 * @param r region
 * @param s Screen
 */
void __write_img(LCDjpeg* j,Region r,Screen s){
    int black = mix_arbg(0,255,255,255);//black bg
    write_region(&black,r,s);
    int x = 0, i = 0;
    int line_len = j -> width * j -> num_channel;
    #ifdef DEBUG
    printf("Ready to write img %d x %d in (%d,%d) (%d,%d)\n",j -> width,j -> height,r.start_point.x,r.start_point.y,r.end_point.x,r.end_point.y);
    printf("Line len %d\n",line_len);
    #endif

    char * line_buffer = malloc(sizeof(char) * line_len);
    
    for(int y = r.start_point.y; y < r.end_point.y; y++){
        // printf("Writing line %d\n",y);

        __get_next_line(line_buffer,j -> datas,(y - r.start_point.y),line_len);
        write_region_colors(line_buffer,Region_new(Point_new(r.start_point.x,y),Point_new(r.end_point.x,y+1)),s);
        // printf("%s:%d\n", __FUNCTION__, __LINE__);
        // for(i = 0, x = r.start_point.x; x < r.end_point.x; x++){
        //     // if(j -> num_channel == 3){
        //         //for RGB
        //         printf("%#x\n", mix_arbg(0,line_buffer[i],line_buffer[i+1],line_buffer[i+2]));
        //         printf("(x,y):(%d,%d), s.size_x=%d\n",x,y,s.size_x);

        //         // *(s.display_mem + (y - 1) * s.size_x + x) = mix_arbg(0,line_buffer[i],line_buffer[i+1],line_buffer[i+2]);
        //         // (s.display_mem + (y - 1) * s.size_x + x) = line_buffer[i] << 16 |
        //         //                                                     line_buffer[i + 1] << 8 | line_buffer[i + 2] << 0;
        //         i += 3;

        //     // }else {
        //     //     //for RGBA

        //     //     *(s.display_mem + (y - 1) * s.size_x + x) = mix_arbg(line_buffer[i+3],line_buffer[i],line_buffer[i+1],line_buffer[i+2]);
        //     //                                      line_buffer[i + 1] << 8 | line_buffer[i + 2] << 0;
        //     //     i += 4;
        //     // }
        // }
    }
}

/**
 * @brief print given LCDjpeg to given screen
 * 
 * @param j 
 * @param s 
 */
void LCDjpeg_print_to_screen(LCDjpeg* j,Screen s){

    //fresh background

    int BG_color = RGBA_mix_arbg(RGBA_new(0,0,0,0));
    write_region(&BG_color,Screen_to_region(s),s);

    //resize img
    j = LCDjpeg_resize_fit(j,s);

    //calc draw region so img can be in the center
    int x_offest = 0;
    int y_offest = 0;

    if(j -> img_direction == H ){
        y_offest = 1.0 * (s.size_y - j -> height) / 2;
    }else{

        x_offest = 1.0 * (s.size_x - j -> width) / 2;
    }

    #ifdef DEBUG
    printf("x_offset %d, y offset %d\n",x_offest,y_offest);
    #endif

    //create img region
    Region to_draw = Region_new(Point_new(x_offest,y_offest),Point_new(x_offest+j -> width,y_offest+j -> height));

    __write_img(j,to_draw,s);
}

/**
 * @brief [internal func] return next line in A
 * 
 * @param buffer 
 * @param A 
 * @param line_count 
 * @param line_length 
 */
void __get_next_line(unsigned char * buffer, unsigned char * A, int line_count,int line_length){
    // printf("Get next-line was called\n");
    memcpy(buffer,A + line_count * line_length,line_length);
}