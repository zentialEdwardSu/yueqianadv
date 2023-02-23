/**
 * @file LCDColorutils.h
 * @author EdwardSu (zentialeds@gmail.com) Xu XuChang Li HongQing
 * @brief A utiils bundle for lcd operations Licened under MIT
 * @version 0.6
 * @date 2023-02-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LCDUTILS__
#define __LCDUTILS__
#define BIT 4

/**
 * @brief Wrapper to represent a 2D point
 * 
 */
typedef struct Point{
    int x;
    int y;
}Point;

/**
 * @brief Wrapper to represent a lcd screen
 * 
 */
typedef struct Screen {
    int fd; //device fd
    int size_x; // width
    int size_y; // height
    char *display_mem; // maped display mem
}Screen;

/**
 * @brief Wrapper to represent a region
 * 
 */
typedef struct Region{
    Point start_point; //top-left point of a region
    Point end_point; // bottom-right point of a region
    int region_height; 
    int region_width;
} Region;

/**
 * @brief Color struct
 * 
 */
typedef struct RGBA {
    int A; // 0-255
    int R; // 0-255
    int G; // 0-255
    int B; // 0-255
} RGBA;

Point Point_new(int x, int y);

Screen Screen_new(char *device,int width, int height);
Region Screen_to_region(Screen s);

Region Region_new(Point start, Point end);
int Region_calc_x_len(Region *region);
int Region_calc_y_len(Region *region);

RGBA RGBA_new(int a,int r,int g, int b);
int RGBA_mix_arbg(RGBA RGBA);
RGBA RGBA_gradient_next_color(RGBA c,RGBA t_c,int step);

void gradient(RGBA c1, RGBA c2,int step_length,Region region,Screen screen);
int mix_arbg(int a,int r,int g, int b);
void write_region(int *color,Region region,Screen s);
void Screen_mumap(Screen screen);
int point_to_int(Point p,Screen s);
// int* Point_to_int_arr(Point p){
//     int res[2] = {p.x, p.y};
//     return res;
// }

// /**
//  * @brief 
//  * 
//  * @param c1 
//  * @param c2 
//  * @param region 
//  */
// void liner_gradiant(int c1,int c2,int *region){

// }

#endif