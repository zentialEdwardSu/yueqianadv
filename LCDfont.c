#include "LCDfont.h"
#include "LCDutils.h"

// int *utf8_to_ucs2(char *code);
// int __calc__offset(bitmap *bm, int x, int y,int offset);
// u32 wstrlen(wchar *ws);

/**
 * @brief create a new bitmap
 * 
 * @param width 
 * @param height 
 * @param byte_per_pixel 
 * @return bitmap* 
 */
bitmap *bitmap_new(u32 width, u32 height, u32 byte_per_pixel){
	bitmap *bm = (bitmap *)malloc(sizeof(bitmap));
	bzero(bm, sizeof(bitmap));
	bm->height = height;
	bm->width = width;
	bm->byte_per_pixel = byte_per_pixel;
	bm->map = (unsigned char *)malloc(width*height*byte_per_pixel);
	bzero(bm->map, width*height*byte_per_pixel);

	return bm;
}

// int *utf8_to_ucs2(char *code);
// int __calc__offset(bitmap *bm, int x, int y,int offset);

/**
 * @brief destory bitmap
 * 
 * @param bm bitmap
 */
void bitmap_destroy(bitmap *bm)
{
	bzero(bm->map, bm->height * bm->width * bm->byte_per_pixel);
	free(bm->map);
	
	bzero(bm, sizeof(bitmap));
	free(bm);
}

/**
 * @brief Get the color of the pixel
 * 
 * @param bm 
 * @param x 
 * @param y 
 * @return RGBA 
 */
static RGBA get_pixel(bitmap *bm, u32 x, u32 y)
{
    unsigned char a = 0,b = 0,g = 0,r = 0;
    r = *(bm->map + __calc__offset(bm,x,y,0));
    g = *(bm->map + __calc__offset(bm,x,y,1));
    b = *(bm->map + __calc__offset(bm,x,y,2));
    if(bm->byte_per_pixel == 4){
        a = *(bm->map + __calc__offset(bm,x,y,3));
	}
	return RGBA_new(a,r,g,b);
}

/**
 * @brief Set the color for the given pixel
 * 
 * @param bm 
 * @param x 
 * @param y 
 * @param c 
 */
void set_pixel(bitmap *bm, u32 x, u32 y, RGBA c)
{
    unsigned char a = 0,b = 0,g = 0,r = 0;
    *(bm->map + __calc__offset(bm,x,y,0)) = c.R;
    *(bm->map + __calc__offset(bm,x,y,1)) = c.G;
    *(bm->map + __calc__offset(bm,x,y,2)) = c.B;
    if(bm->byte_per_pixel == 4){
        *(bm->map + __calc__offset(bm,x,y,3)) = c.A;
	}
}

int __calc__offset(bitmap *bm, int x, int y,int offset){
    return (y*bm->width*bm->byte_per_pixel + x*bm->byte_per_pixel + offset);
}

/**
 * @brief create new bit map with given color
 * 
 * @param width 
 * @param height 
 * @param byte_per_pixel 
 * @param c 
 * @return bitmap* 
 */
bitmap *bitmap_init(u32 width, u32 height, u32 byte_per_pixel, RGBA c)
{
	bitmap *bm = init_bitmap(width, height, byte_per_pixel);
	u32 x, y;
    // write_region()
	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			set_pixel(bm, x, y, c);
		}
	}

	return bm;

}

/**
 * @brief convert a utf-8encoding word to ucs2 encoding
 * 
 * @param code 
 * @return int* 
 */
int *utf8_to_ucs2(char *code)
{
	int *ucs2 = (int *)malloc((strlen(code)+2)*sizeof(int));
	bzero(ucs2, (strlen(code)+2)*sizeof(int));
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

		int ch = 0;
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

/**
 * @brief get length of the given str
 * 
 * @param ws 
 * @return u32 
 */
u32 wstrlen(wchar *ws)
{
	u32 len = 0;
	while(*(ws+len)!='\0')
		len++;
	return len;
}

/**
 * @brief write point martix genarate by fontfile and text to cache bitmap 
 * 
 * @param f font file
 * @param screen bitmap cache
 * @param x 
 * @param y 
 * @param text text to print out
 * @param c color of font
 * @param maxWidth 
 */
void font_print(font *f, bitmap *screen, int x, int y, char *text, RGBA c, int maxWidth)
{
	wchar *wText = utf8_to_ucs2(text);
	u8 *charRaster = NULL;
	int bx=0, by=0, bw=0, bh=0;
	int ascent=0, descent=0, lineGap=0;
	int sx=0, sy=0;

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

		int charWidth = bw - bx;
		int charHeight = bh - by;
		int oy = ascent + by;

		if(maxWidth > 0 && sx + charWidth > maxWidth) 
		{
			sy += ascent - descent + lineGap;
			sx = 0;
		}

		charRaster = realloc(charRaster, charWidth*charHeight);
		
		stbtt_MakeCodepointBitmap(f->info, charRaster, charWidth, charHeight, charWidth, f->scale, f->scale, wText[i]);

		int advance;
		stbtt_GetCodepointHMetrics(f->info, wText[i], &advance, 0);
		int kerning = stbtt_GetCodepointKernAdvance(f->info, wText[i], wText[i+1]);
		int printLength = advance * f->scale + kerning * f->scale;
		
		int mx;
		for(mx=0; mx<printLength; mx++)
		{
			if(charWidth+mx < printLength-mx)
			{
				continue;
			}
			break;
		}
		
		int ix, iy;
		for(iy=0; iy<charHeight; iy++)
		{
			for(ix=0; ix<charWidth; ix++)
			{
				int xpos = x + sx + ix + mx;// + (printLength-charWidth)/2;
				int ypos = (y + sy + oy + iy) - 1;

				if(charRaster[ix+iy*charWidth]!=0 && xpos<screen->width && ypos<screen->height)
				{
					u32 alpha = charRaster[ix+iy*charWidth];
					u32 invAlpha = 255 - alpha;
					RGBA bgc = get_pixel(screen, xpos, ypos);

					set_pixel(screen, xpos, ypos, RGBA_do_alpha(c,bgc,alpha));
				}
			}
		}
		
		bzero(charRaster, charWidth*charHeight);
	
		sx += printLength;
	}

	free(charRaster);
	free(wText);
}

/**
 * @brief Load font file
 * 
 * @param fontPath 
 * @return font* 
 */
font *font_load(char *fontPath)
{
	//open .ttf and read
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

	// read font from memory
	font *f = (font *)malloc(sizeof(font));
	f->info = (stbtt_fontinfo *)malloc(sizeof(stbtt_fontinfo));
	if(!buffer || buffer_size==0)
		return NULL;
	if(stbtt_InitFont(f->info, buffer, 0) != 1)
		return NULL;
	f->buffer = buffer;
	f->scale = stbtt_ScaleForPixelHeight(f->info, 16);

	//返回
	return f;
}

/**
 * @brief Close ttf file
 * 
 * @param f 
 */
void font_unload(font *f)
{
	free(f->info);
	free(f->buffer);
	free(f);
}

/**
 * @brief set font size
 * 
 * @param f font fiile
 * @param pixels ?px for font
 */
void font_set_size(font *f, int pixels)
{
	f->scale = stbtt_ScaleForPixelHeight(f->info, pixels);
}

/**
 * @brief flush bitmap cache to display memory
 * 
 * @param bm bitmap cache
 * @param x start x
 * @param y start y
 * @param p displaymemory
 */
void show_bitmap(bitmap *bm, int x, int y,char *p)
{ 
    char *q = p + (y*800+x) * 4;
    for(int j=0; j<bm->height && j<480; j++)
    {
		for(int i=0;i<bm->width && i< 800 ;i++)   
			memcpy(q+(800*4*j)+i*4,bm->map+(bm->width*bm->byte_per_pixel*j)+
											bm->byte_per_pixel*i,bm->byte_per_pixel);
    } 
    
    bzero(bm->map, bm->width * bm->height * bm->byte_per_pixel);   
    //字体的信息：
    //长 bm->width
    //高:bm->height
    //色深:bm->byte_per_pixel);
    //bm->map; 指向RGB数据，没有上下颠倒，也没有一行4字节倍数的约束，就是纯粹的RGB
}

/**
 * @brief create scrolling text
 * 
 * @param text text to be disply
 * @param _font font path 
 * @param rolling_speed how many pixels per 10us
 * @param s Screen
 * @param show_r Region for rolling text
 * @param bg_color background color for suubtitle
 * @param font_color font color
 */
void font_subtitle_scrolling(char *text,char *_font,char rolling_speed,Screen *s,Region *show_r,RGBA bg_color,RGBA font_color)
{   
    // //打开lcd屏幕
    // int lcd = open("/dev/fb0",O_RDWR);
    // if(lcd == -1)
    // {
    //     perror("open /dev/fb0 fail");
    //     return -1;
    // }

    // //申请一块虚拟区大小的映射内存
    // struct fb_var_screeninfo fixinfo; // 固定属性
    // ioctl(lcd,FBIOGET_VSCREENINFO, &fixinfo); //获取固定属性
    // unsigned long VWIDTH  = fixinfo.xres; //可见区宽度（单位：像素）
    // unsigned long VHEIGHT = fixinfo.yres; //可见区高度（单位：像素）
    // unsigned long BPP = fixinfo.bits_per_pixel;  //色深  
    // char *p = mmap(NULL, VWIDTH * VHEIGHT * BPP/8, PROT_WRITE, MAP_SHARED, lcd, 0); 
    // if(p == MAP_FAILED)
    // {
    //     perror("create mmap fail");
    //     return -1;
    // }
    // bzero(p, VWIDTH*VHEIGHT*BPP/8);
	// use given font_path to load font
    font *f2 = font_load(_font);

    //  set font size to fit display region
    font_set_size(f2, show_r->region_height -1);
     
    // display cache for text
    //                            长   高  色深   颜色（RGBA）    
    bitmap *bm2 = bitmap_init(show_r->region_width,show_r->region_height,4,bg_color);    
    // bm2 = create_bitmap_with_init(show_r->region_width, show_r->region_height, 4,  0x00ff0000);//设置输出框信息  
    show_bitmap(bm2, 0, 0,  s->display_mem);// pass display mem to set black
    sleep(1);

    //字幕滚动
    int roll = show_r->region_width;
    while(1)
    { 
        /*         指定字库文件     映射指针    控制字体显示的起始坐标  字体的文本信息   字体颜色rgba   字体宽度设置，默认为0
            fontPrint(font *f, bitmap *screen, s32 x, s32 y,           char *text,   color c,      s32 maxWidth)
        */
        font_print(f2, bm2, 0, 0, text, font_color, 0);  // print display content to bitmap

        //printf("%s:%s:%d:%d\n", __FILE__, __FUNCTION__, __LINE__, roll);
        roll += rolling_speed;
        if(roll <=0) //move right
        {
            // break;
			roll = show_r->region_width;
        }

        //显示字体/输出框颜色
        show_bitmap(bm2, roll, s->size_y - 1 - show_r -> region_height,  s->display_mem); 

        usleep(10);
    } 

	bitmap_destroy(bm2);
	// cclose font file
	font_unload(f2);

}
