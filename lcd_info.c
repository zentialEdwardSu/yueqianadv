#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>
int lcd;
struct fb_fix_screeninfo fixinfo; // 固定属性
struct fb_var_screeninfo varinfo; // 可变属性
void get_fixinfo()
{
    if (ioctl(lcd, FBIOGET_FSCREENINFO, &fixinfo) != 0)
    {
        perror("获取LCD设备固定属性信息失败");
        return;
    }
}
void get_varinfo()
{
    if (ioctl(lcd, FBIOGET_VSCREENINFO, &varinfo) != 0)
    {
        perror("获取LCD设备可变属性信息失败");
        return;
    }
}
void show_info()
{
    // 获取LCD设备硬件fix属性
    get_fixinfo();
    printf("\n获取LCD设备固定属性信息成功：\n");
    // printf("[ID]: %s\n", fixinfo.id);
    // printf("[FB类型]: ");
    // switch (fixinfo.type)
    // {
    // case FB_TYPE_PACKED_PIXELS:
    //     printf("组合像素\n");
    //     break;
    // case FB_TYPE_PLANES:
    //     printf("非交错图层\n");
    //     break;
    // case FB_TYPE_INTERLEAVED_PLANES:
    //     printf("交错图层\n");
    //     break;
    // case FB_TYPE_TEXT:
    //     printf("文本或属性\n");
    //     break;
    // case FB_TYPE_VGA_PLANES:
    //     printf("EGA/VGA图层\n");
    //     break;
    // }
    // printf("[FB视觉]: ");
    // switch (fixinfo.visual)
    // {
    // case FB_VISUAL_MONO01:
    //     printf("灰度. 1=黑;0=白\n");
    //     break;
    // case FB_VISUAL_MONO10:
    //     printf("灰度. 0=黑;1=白\n");
    //     break;
    // case FB_VISUAL_TRUECOLOR:
    //     printf("真彩色\n");
    //     break;
    // case FB_VISUAL_PSEUDOCOLOR:
    //     printf("伪彩色\n");
    //     break;
    // case FB_VISUAL_DIRECTCOLOR:
    //     printf("直接彩色\n");
    //     break;
    // case FB_VISUAL_STATIC_PSEUDOCOLOR:
    //     printf("只读伪彩色\n");
    //     break;
    // }
    // printf("[行宽]: %d 字节\n", fixinfo.line_length);
    // 获取LCD设备硬件var属性
    get_varinfo();
    printf("\n获取LCD设备可变属性信息成功：\n");
    printf("[可见区分辨率]: %d×%d\n", varinfo.xres, varinfo.yres);
    printf("[虚拟区分辨率]: %d×%d\n", varinfo.xres_virtual, varinfo.yres_virtual);
    // printf("[从虚拟区到可见区偏移量]: (%d,%d)\n", varinfo.xoffset, varinfo.yoffset);
    // printf("[色深]: %d bits\n", varinfo.bits_per_pixel);
    // printf("[像素内颜色结构]:\n");
    // printf(" [红] 偏移量:%d, 长度:%d bits\n", varinfo.red.offset,
    //        varinfo.red.length);
    // printf(" [绿] 偏移量:%d, 长度:%d bits\n", varinfo.green.offset, varinfo.green.length);
    // printf(" [蓝] 偏移量:%d, 长度:%d bits\n", varinfo.blue.offset,
    //        varinfo.blue.length);
    // printf(" [透明度] 偏移量:%d, 长度:%d bits\n", varinfo.transp.offset,
    //        varinfo.transp.length);
    printf("\n");
}
int main()
{
    lcd = open("/dev/fb0", O_RDWR);
    if (lcd == -1)
    {
        perror("打开 /dev/fb0 失败");
        exit(0);
    }
    printf("@物联212 苏子钰 郑凯龙 胡乐涵");
    // 显示LCD设备属性信息
    show_info();
    return 0;
}