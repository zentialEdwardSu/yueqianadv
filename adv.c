#include "include/LCDjpeg.h"
#include "include/LCDutils.h"

#include <stdlib.h>
#include <unistd.h>

void Black(Screen s);

int main()
{
    Screen s = Screen_new("/dev/fb0",800, 480); //打开显示器

    Black(s);

    LCDjpeg* j1 = LCDjpeg_read("1.jpg");//显示图片
    LCDjpeg_print_to_screen(j1 , s);
    sleep(10);

    system("mplayer  1.avi   -zoom  -x  800  -y  480");//播放视频
    sleep(10);

    LCDjpeg* j2 = LCDjpeg_read("2.jpg");//显示图片
    LCDjpeg_print_to_screen(j2 , s);
    sleep(10);

    Black(s);
    LCDjpeg_destory(j1);
    LCDjpeg_destory(j2);

    return 0;
}

void Black(Screen s)
{
    int c = mix_arbg(0,0,0,0);

    write_region(&c,Screen_to_region(s),s);
}