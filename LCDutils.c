#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
// #include <jpeglib.h>

#include"include/LCDutils.h"

/**
 * @brief create new Point struct
 * 
 * @param x 
 * @param y 
 * @return Point 
 */
Point Point_new(int x, int y){
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

/**
 * @brief create new Screen struct
 * 
 * @param device devicename

 * @return Screen 
 */
Screen Screen_new_ioctl(char *device){
    Screen res;
    struct fb_var_screeninfo fixinfo;
    res.fd = open(device, O_RDWR);
    ioctl(res.fd,FBIOGET_VSCREENINFO, &fixinfo);
    res.size_x = fixinfo.xres;
    res.size_y = fixinfo.yres;
    res.display_mem = mmap(NULL, res.size_x * res.size_y * fixinfo.bits_per_pixel/8, PROT_WRITE,
                   MAP_SHARED, res.fd, 0);
    return res;
}

/**
 * @brief create new Screen struct
 * 
 * @param device devicename
 * @param width 
 * @param height 
 * @return Screen 
 */
Screen Screen_new(char *device,int width, int height){
    Screen res;
    res.fd = open(device, O_RDWR);
    res.size_x = height;
    res.size_y = width;
    res.display_mem = mmap(NULL, res.size_x * res.size_y * 3, PROT_WRITE,
                   MAP_SHARED, res.fd, 0);
    return res;
}

/**
 * @brief convert Screen to a region
 * 
 * @param s 
 * @return Region 
 */
Region Screen_to_region(Screen s){
    return Region_new(Point_new(0,0),Point_new(s.size_x,s.size_y));
}

/**
 * @brief free display memory
 * 
 * @param screen
 */
void Screen_mumap(Screen screen){
    munmap(screen.display_mem, screen.size_x*screen.size_y*4);
}

/**
 * @brief create new region struct
 * 
 * @param start top-left
 * @param end bottom-right
 * @return Region 
 */
Region Region_new(Point start, Point end){
    Region res;
    res.start_point = start;
    res.end_point = end;
    res.region_height = end.y - start.y;
    res.region_width = end.x - end.y;
    return res;
}

/**
 * @brief calc length of region's x axis
 * 
 * @param region 
 * @return int 
 */
int Region_calc_x_len(Region *region){
    return region->end_point.x - region->start_point.x;
}
/**
 * @brief calc length of region's y axis
 * 
 * @param region 
 * @return int 
 */
int Region_calc_y_len(Region *region){
    return region->end_point.y - region->start_point.y;
}

/**
 * @brief create new RGBA struct
 * 
 * @param a int 0-255
 * @param r int 0-255
 * @param g int 0-255
 * @param b int 0-255
 * @return RGBA 
 */
RGBA RGBA_new(int a,int r,int g, int b){
    RGBA res;
    res.A = a;
    res.B = b;
    res.G = g;
    res.R = r;
    return res;
}

/**
 * @brief convert RGBA struct to int 0xAARRGGBB
 * 
 * @param RGBA 
 * @return int 
 */
int RGBA_mix_arbg(RGBA RGBA) {
    return RGBA.B | RGBA.G<<2*BIT | RGBA.R<<4*BIT | RGBA.A<<6*BIT;
}

/**
 * @brief convert RGBA struct to int 0xRRGGBBAA
 * 
 * @param RGBA 
 * @return int 
 */
int RGBA_mix_rgba(RGBA RGBA) {
    // return RGBA.B | RGBA.G<<2*BIT | RGBA.R<<4*BIT | RGBA.A<<6*BIT;
    return RGBA.A | RGBA.B << 2*BIT | RGBA.G<<4*BIT | RGBA.G<<6*BIT;
}

/**
 * @brief use alpha to conbine two color
 * 
 * @param c base color
 * @param rgba to mix color
 * @param alpha 0-255 alpha var
 * @return RGBA 
 */
RGBA RGBA_do_alpha(RGBA c,RGBA rgba, int alpha){
    int invAlpha = 255 - alpha;
    return RGBA_new(0,(alpha * c.R + invAlpha * rgba.R) >> 8,(alpha * c.G + invAlpha * rgba.G) >> 8,(alpha * c.B + invAlpha * rgba.B) >> 8);
}


/**
 * @brief for gradient calc new color base on given color and total step
 * 
 * @param c start color
 * @param t_c end color
 * @param step step count from c to t_c
 * @return RGBA 
 */
RGBA RGBA_gradient_next_color(RGBA c,RGBA t_c,int step){
    RGBA res;
    res.A = 0;
    res.R = (int)c.R + (t_c.R - c.R)/step; 
    res.G = (int)c.G + (t_c.G - c.G)/step; 
    res.B = (int)c.B + (t_c.B - c.B)/step; 
    return res;
}

/**
 * @brief Draw a region with a gradient from c1 to c2 with given ease step
 * 
 * @param c1 start color
 * @param c2 target color
 * @param step_length length of a single color
 * @param region region to draw
 * @param screen 
 */
void gradient(RGBA c1, RGBA c2,int step_length,Region region,Screen screen){
    int gradient_step = (int)(Region_calc_x_len(&region) / step_length);
    RGBA tmp_color = c1;
    int init_y = region.start_point.y;
    int end_y = region.start_point.y + region.region_height; 
    int init_x = region.start_point.x;
    int next_x = init_x + step_length;
    while(gradient_step --){
        int in_color = RGBA_mix_arbg(tmp_color);
        write_region(&in_color,Region_new(Point_new(init_x,init_y),Point_new(next_x,end_y)),screen);
        printf("Writing (%d,%d) (%d,%d) with color %lx\n",init_x,init_y,next_x,end_y,in_color);
        tmp_color = RGBA_gradient_next_color(tmp_color,c2,gradient_step);
        init_x = next_x;
        next_x += step_length;
    }
}

/**
 * @brief mix color
 * 
 * @param a transperant channel
 * @param r Red channel
 * @param g Green channel
 * @param b Blue channel
 * @return int colr after mix
 */
int mix_arbg(int a,int r,int g, int b) {
    return b | g<<2*BIT | r<<4*BIT | a<<6*BIT;
}

/**
 * @brief using select color to draw select region
 * 
 * @param color color interger pointer
 */
void write_region(int *color,Region region,Screen s){
    for (int y = region.start_point.y; y < region.end_point.y; y++){
        for(int i = (region.start_point.x + 800*y); i < (region.end_point.x + 800*y); i++){
            memcpy(s.display_mem+i*BIT,color,BIT);
        }
    }
}

/**
 * @brief using select color to draw select region
 * 
 * @param colors color interger pointer
 */
void write_region_colors(char *colors_three_channel,Region region,Screen s){
    for (int y = region.start_point.y; y < region.end_point.y; y++){
        int i = 0;
        for(int x = (region.start_point.x + 800*y); x < (region.end_point.x + 800*y); x++){
            int color = mix_arbg(0,colors_three_channel[i],colors_three_channel[i+1],colors_three_channel[i+2]);
            memcpy(s.display_mem+x*BIT,&color,BIT);
            i +=3;
        }
    }
}

/**
 * @brief using select color to draw select region
 * 
 * @param color color interger pointer
 */
// void write_region_img(unsigned char * line_buffer,Region region,Screen s){
//     for (int y = region.start_point.y; y < region.end_point.y; y++){
//         for(int i = (region.start_point.x + 800*y); i < (region.end_point.x + 800*y); i++){
//             memcpy(s.display_mem+i*BIT,color,BIT);
//         }
//     }
// }

/**
 * @brief convert (x,y) to int
 * 
 * @param p (x,y) point
 * @param size screen_size (x,y)
 * @return int 
 */
int point_to_int(Point p,Screen s){
    return p.x + p.y * s.size_x;
}