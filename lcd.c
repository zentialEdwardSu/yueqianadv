#include "include/LCDutils.h"
#include <unistd.h>

int main(){
    // int lcd = open("/dev/fb0", O_RDWR);
    // // 给LCD设备映射一块内存（或称显存）
    // char *p = mmap(NULL, 800 * 480 * 4, PROT_WRITE,
    //                MAP_SHARED, lcd, 0);

    Screen s = Screen_new("/dev/fb0",800, 480);
    RGBA c1 = RGBA_new(0, 0, 0, 0);
    RGBA c2 = RGBA_new(0, 255, 255, 255);
    Region region = Region_new(Point_new(100, 100), Point_new(500, 340));
    int bg = RGBA_mix_arbg(c1);

    // free_display(p, s);
    write_region(&bg,Screen_to_region(s),s);
    while (1){
        gradient(c1, c2, 5, region, s);
        sleep(5);
    }
    // write_region(p,0x00000000,Screen_to_region(s),s);

    Screen_mumap(s);
    return 0;
}