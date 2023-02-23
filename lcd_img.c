#include"include/LCDjpeg.h"
#include"include/LCDutils.h"
#include<unistd.h>

int main(){
    LCDjpeg j = LCDjpeg_read("1.jpg");

    Screen s = Screen_new("/dev/fb0",800, 480);

    while (1)
    {
        /* code */
        LCDjpeg_print_to_screen(j,s);
        sleep(3);
    }
    
    LCDjpeg_destory(j);
    return 0;


}