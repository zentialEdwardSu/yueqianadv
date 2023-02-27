/*
 *Copyright (C), 2022-2032, 广州粤嵌通信科技股份有限公司
 *All rights reserved.
 *文件名称: font.h
 *摘要：

 *当前版本：1.1，编写者: 蒋亮；修改者：蒋亮；修改时间：2023年2月24日
 修改内容：
 *历史版本：1.0，编写者: 蒋亮；修改者：蒋亮；修改时间：2023年2月15日
 修改内容：无
 */
#ifndef __FONT_H__
#define __FONT_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/ioctl.h>
#include<sys/stat.h>
#include <linux/fb.h>
#include <time.h>

#define color u32
#define getColor(a, b, c, d) (a|b<<8|c<<16|d<<24)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef struct stbtt_fontinfo
{
   void *userdata;
   unsigned char * data;              // pointer to .ttf file
   int fontstart;                     // offset of start of font
   int numGlyphs;                     // number of glyphs, needed for range checking
   int loca,head,glyf,hhea,hmtx,kern; // table locations as offset from start of .ttf
   int index_map;                     // a cmap mapping for our chosen character encoding
   int indexToLocFormat;              // format needed to map from glyph index to glyph

}stbtt_fontinfo;

typedef struct{
	u32 height;
	u32 width;
	u32 byteperpixel;
	u8 *map;
}bitmap;

typedef struct{
   stbtt_fontinfo *info;
   u8 *buffer;
   float scale;
}font;

//初始化字库 
extern font *font_load(char *fontPath);

//关闭字体库
extern void font_unload(font *f);

//设置字体的大小 
extern void font_set_size(font *f, s32 pixels);

//初始化输出框信息
extern bitmap *init_bitmap(u32 width, u32 height, u32 byteperpixel);

//设置输出框信息
extern bitmap *create_bitmap_with_init(u32 width, u32 height, u32 byteperpixel, color c);

//把字体输出到输出框中
extern void font_print(font *f, bitmap *screen, s32 x, s32 y, char *text, color c, s32 maxWidth);

//销毁输出狂信息
extern void destroy_bitmap(bitmap *bm);

//显示字体/输出框颜色
extern void show_bitmap(bitmap *bm, int x, int y,char *p);

#if 1  //truetype接口
extern int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset);
extern int stbtt_FindGlyphIndex(const stbtt_fontinfo *info, int unicode_codepoint);
extern float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float pixels);
extern float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo *info, float pixels);
extern void stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap);
extern void stbtt_GetFontBoundingBox(const stbtt_fontinfo *info, int *x0, int *y0, int *x1, int *y1);
extern void stbtt_GetCodepointHMetrics(const stbtt_fontinfo *info, int codepoint, int *advanceWidth, int *leftSideBearing);
extern int  stbtt_GetCodepointKernAdvance(const stbtt_fontinfo *info, int ch1, int ch2);
extern int stbtt_GetCodepointBox(const stbtt_fontinfo *info, int codepoint, int *x0, int *y0, int *x1, int *y1);
extern void stbtt_GetGlyphHMetrics(const stbtt_fontinfo *info, int glyph_index, int *advanceWidth, int *leftSideBearing);
extern int  stbtt_GetGlyphKernAdvance(const stbtt_fontinfo *info, int glyph1, int glyph2);
extern int  stbtt_GetGlyphBox(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1);
extern void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
extern void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);
extern void stbtt_MakeCodepointBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint);
#endif

#endif
