/*
 *Copyright (C), 2022-2032, 广州粤嵌通信科技股份有限公司
 *All rights reserved.
 *文件名称: font.c
 *摘要：

 *当前版本：1.1，编写者: 蒋亮；修改者：蒋亮；修改时间：2023年2月24日
 修改内容：
 *历史版本：1.0，编写者: 蒋亮；修改者：蒋亮；修改时间：2023年2月15日
 修改内容：无
 */
#include "font.h"

#define wchar s32
#define wchar_to_str(wc) ((char *)(wc))

#define color u32
#define getColor(a, b, c, d) (a|b<<8|c<<16|d<<24)
#define getA(c) ((c>> 0)&0x000000ff)
#define getR(c) ((c>> 8)&0x000000ff)
#define getG(c) ((c>>16)&0x000000ff)
#define getB(c) ((c>>24)&0x000000ff)

u32 wstrlen(wchar *ws);//获取字符长度
wchar *utf8_to_ucs2(char *code);//将UTF8编码格式转化为UCS2编码

//初始化输出框信息
bitmap *init_bitmap(u32 width, u32 height, u32 byteperpixel)
{
	bitmap *bm = (bitmap *)malloc(sizeof(bitmap));
	bzero(bm, sizeof(bitmap));
	bm->height = height;
	bm->width = width;
	bm->byteperpixel = byteperpixel;
	bm->map = (u8 *)malloc(width*height*byteperpixel);
	bzero(bm->map, width*height*byteperpixel);

	return bm;
}

//销毁输出狂信息
void destroy_bitmap(bitmap *bm)
{
	bzero(bm->map, bm->height * bm->width * bm->byteperpixel);
	free(bm->map);
	
	bzero(bm, sizeof(bitmap));
	free(bm);
}

//获取像素
static color get_pixel(bitmap *bm, u32 x, u32 y)
{
	if(bm->byteperpixel == 3)
	{
		u8 r = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 0);
		u8 g = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 1);
		u8 b = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 2);
		return getColor(0, r, g, b);
	}else if(bm->byteperpixel == 4){
		u8 r = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 0);
		u8 g = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 1);
		u8 b = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 2);
		u8 a = *(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 3);
		return getColor(a, r, g, b);
	}
	return 0;
}

//设置像素
static void set_pixel(bitmap *bm, u32 x, u32 y, color c)
{
	if(bm->byteperpixel == 3)
	{
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 0) = getR(c);
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 1) = getG(c);
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 2) = getB(c);
	}else if(bm->byteperpixel == 4){
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 0) = getR(c);
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 1) = getG(c);
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 2) = getB(c);
		*(bm->map + y*bm->width*bm->byteperpixel + x*bm->byteperpixel + 3) = getA(c);
	}
}

//在LCD屏幕上显示颜色
bitmap *create_bitmap_with_init(u32 width, u32 height, u32 byteperpixel, color c)
{
	bitmap *bm = init_bitmap(width, height, byteperpixel);
	u32 x, y;
	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			set_pixel(bm, x, y, c);
		}
	}

	return bm;
	
	//free(bm);
}

//获取字符长度
u32 wstrlen(wchar *ws)
{
	u32 len = 0;
	while(*(ws+len)!='\0')
		len++;
	return len;
}

//将UTF8编码格式转化为UCS2编码
wchar *utf8_to_ucs2(char *code)
{
	wchar *ucs2 = (wchar *)malloc((strlen(code)+2)*sizeof(wchar));
	bzero(ucs2, (strlen(code)+2)*sizeof(wchar));
	s16 com = 1<<7;
	u32 x;

	for(x=0; x<=strlen(code); x++)
	{
		char utf = code[x];
		u32 size = 0;
		u32 i = 0;
		u32 index = (utf&com) != 0;
		u16 binary[16];

		if(index == 0){///0xxxxxxx ==> 00000000 0xxxxxxxx

			for(; i < 8; ++i)
			{
				binary[i] = 0;
			}
			for(; i < 16; ++i)
			{
				binary[i] = (utf & 1 << (15 - i)) != 0;
			}

		}else if(utf&(1 << 5) == 0){// 110xxxxx 10yyyyyy ==> 00000xxx xxyyyyyy

			for(; i < 5; ++i)
			{
				binary[i] = 0;
			}

			for(; i < 10; ++i)
			{
				binary[i] = (utf&(1 << (9 - i))) != 0;
			}

			x += 1;
			utf = code[x];

			for(; i < 16; ++i)
			{
				binary[i] = (utf&(1 << (15 - i))) != 0;
			}

		}else{//1110xxxx 10yyyyyy 10zzzzzz ==> xxxxyyyy yyzzzzzz

			for(; i < 4; ++i)
			{
				binary[i] = (utf & 1 << (3 - i)) != 0;
			}
			x += 1;
			utf = code[x];
			for(; i < 10; ++i)
			{
				binary[i] = (utf & 1 << (9 - i)) != 0;
			}
			x += 1;
			utf = code[x];
			for(; i < 16; ++i)
			{
				binary[i] = (utf & 1 << (15 - i)) != 0;
			}
		}

		wchar ch = 0;
		for(i=0; i <16; i++)
		{
			ch <<= 1;
			ch |= binary[i];
		}

		u32 len = wstrlen(ucs2);
		ucs2[len] = ch;
		ucs2[len+1] = 0;
	}

	return ucs2;
}

//把字体输出到输出框中
void font_print(font *f, bitmap *screen, s32 x, s32 y, char *text, color c, s32 maxWidth)
{
	wchar *wText = utf8_to_ucs2(text);
	u8 *charRaster = NULL;
	s32 bx=0, by=0, bw=0, bh=0;
	s32 ascent=0, descent=0, lineGap=0;
	s32 sx=0, sy=0;

	stbtt_GetFontVMetrics(f->info, &ascent, &descent, &lineGap);

	ascent *= f->scale;
	descent *= f->scale;
	lineGap *= f->scale;

	u32 i = 0;
	for(i=0; i<wstrlen(wText); i++)
	{
		if(wText[i] == '\n')
		{
			sy += ascent - descent + lineGap;
			sx = 0;
			continue;
		}

		stbtt_GetCodepointBitmapBox(f->info, wText[i], f->scale, f->scale, &bx, &by, &bw, &bh);

		s32 charWidth = bw - bx;
		s32 charHeight = bh - by;
		s32 oy = ascent + by;

		if(maxWidth > 0 && sx + charWidth > maxWidth) 
		{
			sy += ascent - descent + lineGap;
			sx = 0;
		}

		charRaster = realloc(charRaster, charWidth*charHeight);
		
		stbtt_MakeCodepointBitmap(f->info, charRaster, charWidth, charHeight, charWidth, f->scale, f->scale, wText[i]);

		s32 advance;
		stbtt_GetCodepointHMetrics(f->info, wText[i], &advance, 0);
		s32 kerning = stbtt_GetCodepointKernAdvance(f->info, wText[i], wText[i+1]);
		s32 printLength = advance * f->scale + kerning * f->scale;
		
		s32 mx;
		for(mx=0; mx<printLength; mx++)
		{
			if(charWidth+mx < printLength-mx)
			{
				continue;
			}
			break;
		}
		
		s32 ix, iy;
		for(iy=0; iy<charHeight; iy++)
		{
			for(ix=0; ix<charWidth; ix++)
			{
				s32 xpos = x + sx + ix + mx;// + (printLength-charWidth)/2;
				s32 ypos = (y + sy + oy + iy) - 1;

				if(charRaster[ix+iy*charWidth]!=0 && xpos<screen->width && ypos<screen->height)
				{
					u32 alpha = charRaster[ix+iy*charWidth];
					u32 invAlpha = 255 - alpha;
					color bgc = get_pixel(screen, xpos, ypos);
					u8 bgr = getR(bgc);
					u8 bgg = getG(bgc);
					u8 bgb = getB(bgc);

					u8 r = (alpha * getR(c) + invAlpha * bgr) >> 8;
					u8 g = (alpha * getG(c) + invAlpha * bgg) >> 8;
					u8 b = (alpha * getB(c) + invAlpha * bgb) >> 8;

					set_pixel(screen, xpos, ypos, getColor(0, r, g, b));
				}
			}
		}
		
		bzero(charRaster, charWidth*charHeight);
	
		sx += printLength;
	}

	free(charRaster);
	free(wText);
}

//设置字体的大小 
void font_set_size(font *f, s32 pixels)
{
	f->scale = stbtt_ScaleForPixelHeight(f->info, pixels);
}

//初始化字库 
font *font_load(char *fontPath)
{
	//打开字体文件并读取
	s32 fd = open(fontPath, O_RDONLY);
	u32 buffer_size = lseek(fd, 0, SEEK_END);
	u8 *buffer = (u8 *)malloc(buffer_size);
	if(fd == -1)
	{
		perror("open font load fail");
		return NULL;
	}
	lseek(fd, 0, SEEK_SET);
	read(fd, buffer, buffer_size);
	close(fd);

	//从内存读取字体
	font *f = (font *)malloc(sizeof(font));
	f->info = (stbtt_fontinfo *)malloc(sizeof(stbtt_fontinfo));
	if(!buffer || buffer_size==0)
		return NULL;
	if(stbtt_InitFont(f->info, buffer, 0) != 1)
		return NULL;
	f->buffer = buffer;
	f->scale = stbtt_ScaleForPixelHeight(f->info, 16);

	//printf("%s:%s:%d:\n", __FILE__, __FUNCTION__, __LINE__);

	//返回
	return f;
}

//关闭字体库
void font_unload(font *f)
{
	free(f->info);
	free(f->buffer);
	free(f);
}

//LCD显示的位置
void show_bitmap(bitmap *bm, int x, int y,char *p)
{ 
    char *q = p + (y*800+x) * 4;
    for(int j=0; j<bm->height && j<480; j++)
    {
		for(int i=0;i<bm->width && i< 800 ;i++)   
			memcpy(q+(800*4*j)+i*4,bm->map+(bm->width*bm->byteperpixel*j)+
											bm->byteperpixel*i,bm->byteperpixel);
    } 
    
    bzero(bm->map, bm->width * bm->height * bm->byteperpixel);   
    //字体的信息：
    //长 bm->width
    //高:bm->height
    //色深:bm->byteperpixel);
    //bm->map; 指向RGB数据，没有上下颠倒，也没有一行4字节倍数的约束，就是纯粹的RGB
}

