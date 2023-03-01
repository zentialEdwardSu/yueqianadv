/**
 * @file LCDfont.h
 * @author Jia Liang
 * @brief utils for display fonts
 * @version 2.0-su
 * @date 2023-02-27
 *
 * @copyright Copyright (c) 2023
 *
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
#include <sys/stat.h>
#include <linux/fb.h>
#include <time.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef int wchar;

/**
 * @brief Wrapper for ttf font file
 * 
 */
typedef struct stbtt_fontinfo {
   void *userdata;
   unsigned char * data;              // pointer to .ttf file
   int fontstart;                     // offset of start of font
   int numGlyphs;                     // number of glyphs, needed for range checking
   int loca,head,glyf,hhea,hmtx,kern; // table locations as offset from start of .ttf
   int index_map;                     // a cmap mapping for our chosen character encoding
   int indexToLocFormat;              // format needed to map from glyph index to glyph
}stbtt_fontinfo;

typedef struct{
	int height;
	int width;
	int byte_per_pixel;
	unsigned char *map;
}bitmap;

typedef struct{
   stbtt_fontinfo *info;
   u8 *buffer;
   float scale;
}font;


bitmap *bitmap_new(u32 width, u32 height, u32 byte_per_pixel);
bitmap *bitmap_init(u32 width, u32 height, u32 byte_per_pixel, RGBA c);
void bitmap_destroy(bitmap *bm);
static RGBA get_pixel(bitmap *bm, u32 x, u32 y);
void set_pixel(bitmap *bm, u32 x, u32 y, RGBA c);
u32 wstrlen(wchar *ws);
int __calc__offset(bitmap *bm, int x, int y,int offset);
int *utf8_to_ucs2(char *code);

font *font_load(char *fontPath);
void font_unload(font *f);
void font_set_size(font *f, int pixels);
void font_print(font *f, bitmap *screen, int x, int y, char *text, RGBA c, int maxWidth);
void font_subtitle_scrolling(char *text,char *_font,char rolling_speed,Screen *s,Region *show_r,RGBA bg_color,RGBA font_color);

void show_bitmap(bitmap *bm, int x, int y,char *p);
#endif