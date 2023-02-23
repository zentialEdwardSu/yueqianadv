#include"include/LCDjpeg.h"
#include"include/LCDutils.h"
#include<unistd.h>

int main(int argc,char **args){
    LCDjpeg *j = LCDjpeg_read(args[1]);

    Screen s = Screen_new("/dev/fb0",800, 480);

    LCDjpeg_print_to_screen(j,s);
    
    LCDjpeg_destory(j);
    return 0;


}