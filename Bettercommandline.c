#include"include/def_lib.h"

void load_frame();
void move_directional(int direction,int target[]);
void swap(int target[],int value,char c);
void update_selection(int direction);
void activate_func();
void null_func();
// void fresh();
// int file_exists(char *filename);
void start_video();
void continue_video();
void pause_video();

int target[2]={0,0};
char panel[y_border][x_border][50]={
        {"[start video]"},
        {"[pause video]"},
        {"[continue video]"},
};
int in_enter = 0;
int video_on = 0;
void (*func_list[y_border][x_border])()={
    {start_video},
    {pause_video},
    {continue_video},
};

void start_video(){
    video_on = 1;
    system("mplayer -loop 0 -slave -quiet -geometry 30:30 -zoom -x 800 -y 480 Never.avi &");
}

void continue_video(){
    system("killall -CONT mplayer &");
}

void pause_video(){
    system("killall -STOP mplayer &");
}

int main()
{
    set_conio_terminal_mode();
    int ch;
    // system("title betercml for mplayer from EdwardSu");
    load_frame();
    while( 1 ) /* Press ESC to quit... */
    {   
        ch = getch();
        switch(ch)
        {
            case 27:
                switch(ch=getch())
                {
                    case 91:
                        switch (ch=getch())
                        {
                            /*
                            * 65 上 66下 67 右 68 左
                            */
                            case 65:  update_selection(2); break;
                            case 66:  update_selection(1); break;
                            case 68:  update_selection(4); break;
                            case 67:  update_selection(3); break;
                            default:
                                break;
                        }
                        break;
                }
                break;
            case 0x0d:

                in_enter=1;//press enter to confirm the choice
                // printf("enter pressed\n");
                activate_func();
                in_enter=0;
                break;
            default:
                break;
        }
        // if(in_enter==1)
        // {
        //     activate_func();
        //     in_enter=0;
        // }
        if(ch==0x1B){
            if(video_on){
                system("killall -9 mplayer &");
            }
            printf("Goodbye\r\n");
            sleep(2);
            system("clear");
            reset_terminal_mode();
            // system("title Goobye");
        // system("choice /t 3 /c YN /d Y");
            break;//退出程序
        }
    }
}
void update_selection(int direction)
{
    move_directional(direction,target);
    load_frame();
}
void move_directional(int direction,int target[])//1-UP 2-Down 3-Left 4-right
{
    switch (direction)
    {
    case 1://UP
        swap(target,1,'y');
        break;
    case 2://DOWN
        swap(target,-1,'y');
        break;
    case 3://Left
        swap(target,1,'x');
        break;
    case 4://right
        swap(target,-1,'x');
        break;
    
    default:
        break;
    }
}
void swap(int target[],int value,char c)
{
    static int temp;
    int index;
    switch(c)
    {
        case 'x':
            index = 0;
            break;
        case 'y':
            index = 1;
            break;
        default:
            break;
    }
    temp = target[index];
    temp+=value;
    switch(index)
    {
        case 0:
        if (temp>=x_border)
            temp-=x_border;
        else if(temp<0)
                temp+=x_border;
        break;
        case 1:
            if (temp>=y_border)
                temp-=y_border;
            else if(temp<0)
                temp+=y_border;
            break;
        default:
            break;
    }
    target[index]=temp;
}
void load_frame()
{   
    char temp[50];
    // char up='↑';
    //printf("%s",target);
    system("clear");
    // printf(">This is the Homwwork-%d from @EdwardSu\n\n",Homeworknum);
    printf("you can use the direction key\r\n");
    printf("    [%s]\r\n","/\\");
    printf("  [%c]  [%c]\r\n",'<','>');
    printf("    [%s]\r\n","\\/");
    printf("to change the selection,than press [enter] to confirm\r\n");
    printf("You can use the [ESC] key to exit the program\r\n");
    for (int i = 0;i<y_border;i++)
    {
        for(int j = 0;j<x_border;j++)
        {
            if(i==target[1]&&j==target[0])
            {       
                printf("->");
                printf("%s",panel[i][j]);
                strcpy(temp,panel[i][j]);
                printf("<-\t"); 
            }
            else
                printf("%s\t",panel[i][j]);
        }
        printf("\r\n");
    }
    printf("\r\nCurrent selection ->[ %s ]\r\n",temp);
}
void activate_func()
{
    int tx,ty;
    tx = target[0];
    ty = target[1];
    //做个函数列表同样是二维存入待选函数名
    (*func_list[ty][tx])();
    system("pause");
    load_frame();
}
void null_func()
{
    printf("[WARN] Invaild selection\n");
}
// void fresh()
// {
//     int flag=file_exists("Auto_arrange.exe");
//     if(flag!=0){
//         system(".\\Auto_arrange");
//     }else{
//         printf("[WARN] Necessary documents missing,process interrupt\n");
//     }
// }
// int file_exists(char *filename)//判断文件是否存在
// {
//     return (access(filename,0) == 0);
// }