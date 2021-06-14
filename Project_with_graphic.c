#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define picture_size 200
#define PI 3.14
#define buttons_count 5

int dataset_counts=15;
int Clock[3]={0} ;  // clock[0]=hour ,clock[1]=minute ,clock[2]=second
int average[picture_size][picture_size][3]={0};

unsigned char pic[2000][2000][3];
unsigned char cropped_pic[2000][2000][3];
unsigned char temp_pic[2000][2000][3];
unsigned char resized_pic[2000][2000][3];
unsigned char average_pic[2000][2000][3];


int crop(int*,int*);
int recognizing();
int resize(int*,int*,double,double);
void learning();
void find_marz(int*, int *);
void find_marz_amoodi(int* ,int*);
void find_marz_ofoghi(int* ,int*);
void read_clock(char *address);

void draw_background(int);
void create_buttons();
void create_menu();
void free_draw();
void digital_clock(int hour,int minute,int second);
void draw_clock(int hour,int minute,int second);
void convert_time();
struct Menu{
     char text[20];
     int x1;
     int y1;
     int x2;
     int y2;
}button[buttons_count];

int main()
{
    int i;
    int exit=0,open=0;
    char *ch;
    create_buttons();
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(500, 500);

    ALLEGRO_FONT* font = al_create_builtin_font();
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_init_image_addon();
    bool redraw = true;
    ALLEGRO_EVENT event;
    ALLEGRO_TIMEOUT timeout;
    ALLEGRO_FILECHOOSER *filechooser;
    al_start_timer(timer);
    learning();
    while(1)
    {
        i=0;
        al_wait_for_event(queue, &event);
        ALLEGRO_MOUSE_STATE state;

        if(event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;

        if(redraw && al_is_event_queue_empty(queue)){
            create_menu();
            int on_button=0;
            while(exit==0){
                al_set_window_title(disp,"Main Menu");
                al_get_mouse_state(&state);
                al_wait_for_event(queue, &event);
                if((event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
                    return 0;

                // when mouse go on "open file" button :
                if(state.x>button[0].x1 && state.x<button[0].x2 && state.y>button[0].y1 && state.y<button[0].y2){
                    if (state.buttons==1 && open==0) {
                        filechooser = al_create_native_file_dialog("./", "Choose a  .bmp file", "*.bmp;", 1);
                        if(al_show_native_file_dialog(disp, filechooser)){
                            ch=al_get_native_file_dialog_path(filechooser,0);
                        }
                        strcpy(button[0].text,"File opened");
                        puts("File choosed");
                        open=1;
                        exit=1;
                    }else if (on_button==0){
                        al_draw_filled_rectangle(button[0].x1,button[0].y1,button[0].x2,button[0].y2,al_map_rgb(52, 152, 219));
                        al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0),button[0].x1+12,button[0].y1+15,500,button[0].text);
                        al_flip_display();
                        on_button=1;
                        i++;
                    }
                //when mouse go on "Read clock" button:
                }else if(state.x>button[1].x1 && state.x<button[1].x2 && state.y>button[1].y1 && state.y<button[1].y2){
                    if (state.buttons==1) {
                        if(open==1){
                            read_clock(ch);
                            digital_clock(Clock[0],Clock[1],Clock[2]);
                            al_resize_display(disp,500,500);
                            strcpy(button[0].text," Open File");
                            open=0;
                        }else
                            al_show_native_message_box(disp,"Error","","you don't select any picture","bashe",ALLEGRO_MESSAGEBOX_ERROR);
                        exit=1;
                    }else if (on_button==0){
                        al_draw_filled_rectangle(button[1].x1,button[1].y1,button[1].x2,button[1].y2,al_map_rgb(52, 152, 219));
                        al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0),button[1].x1+12,button[1].y1+15,500,button[1].text);
                        al_flip_display();
                        on_button=1;
                        i++;
                    }
                // "Analog Clock" :
                }else if(state.x>button[2].x1 && state.x<button[2].x2 && state.y>button[2].y1 && state.y<button[2].y2){
                    if (state.buttons==1) {
                         draw_clock(Clock[0],Clock[1],Clock[2]);
                         open=0;
                         exit=1;
                    }else if (on_button==0){
                        al_draw_filled_rectangle(button[2].x1,button[2].y1,button[2].x2,button[2].y2,al_map_rgb(52, 152, 219));
                        al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0),button[2].x1+12,button[2].y1+15,500,button[2].text);
                        al_flip_display();
                        on_button=1;
                        i++;
                    }
                // "free Draw" :
                }else if(state.x>button[3].x1 && state.x<button[3].x2 && state.y>button[3].y1 && state.y<button[3].y2){
                    if (state.buttons==1) {
                        free_draw();
                        al_resize_display(disp,500,500);
                        exit=1;
                    }else if (on_button==0){
                        al_draw_filled_rectangle(button[3].x1,button[3].y1,button[3].x2,button[3].y2,al_map_rgb(52, 152, 219));
                        al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0),button[3].x1+12,button[3].y1+15,500,button[3].text);
                        al_flip_display();
                        on_button=1;
                        i++;
                    }
                // "convert time" :
                }else if(state.x>button[4].x1 && state.x<button[4].x2 && state.y>button[4].y1 && state.y<button[4].y2){
                    if (state.buttons==1) {
                        al_resize_display(disp,1,1);
                        convert_time();
                        al_resize_display(disp,500,500);
                        exit=1;
                    }else if (on_button==0){
                        al_draw_filled_rectangle(button[4].x1,button[4].y1,button[4].x2,button[4].y2,al_map_rgb(52, 152, 219));
                        al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0),button[4].x1+12,button[4].y1+15,500,button[4].text);
                        al_flip_display();
                        on_button=1;
                        i++;
                    }
                }else if(on_button==1){
                    on_button=0;
                    create_menu();
                }
                if(i>15){
                    al_show_native_message_box(disp,"warning","","lotfan ba dokme ha bazi nakonid :D","bashe",3);
                    i=0;
                }

            }
            redraw = false;
        }
        al_set_mouse_xy(disp,280,10);
        exit=0;
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
void create_buttons(){
    int i;
    button[0].x1=238,button[0].y1=40,button[0].x2=352,button[0].y2=75;
    for(i=1;i<buttons_count;i++){
        button[i].x1=238;
        button[i].y1=40+i*55;
        button[i].x2=352;
        button[i].y2=75+i*55;
    }
    strcpy(button[0].text," Open File");
    strcpy(button[1].text," Read Clock");
    strcpy(button[2].text,"Analog Clock");
    strcpy(button[3].text," Free Draw");
    strcpy(button[4].text,"Convert time");
}
void create_menu(){
    int i;
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(al_load_bitmap("backgrounds/menu.jpg"),0,0,0);
    for(i=0;i<buttons_count;i++){
        al_draw_filled_rectangle(button[0].x1,button[0].y1+i*55,button[0].x2,button[0].y2+i*55,al_map_rgb(41, 128, 185));
        al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0),button[i].x1+12,button[i].y1+15,500,button[i].text);
    }
    al_flip_display();
}
void draw_clock(int hour,int minute,int second){
    int i,b=0;
    int r_hour=60,r_minute=80,r_second=100;
    char title[10];
    float angel_hour,angel_minute,angel_second;
    al_init_font_addon();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* display = al_get_current_display();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    draw_background(0);
    for(i=0;al_is_event_queue_empty(queue);i+=2){

        al_draw_line(250,250,250+ r_second*cos(angel_second),250+r_second*sin(angel_second),al_map_rgb(0, 0, 0),2);
        if((second)%4==0){
            b++;
            draw_background(b%6);
        }

        if((second)%60==0){
            al_draw_line(250,250,250+ r_minute*cos(angel_minute),250+r_minute*sin(angel_minute),al_map_rgb(0, 0, 0),2);
            al_draw_line(250,250,250+r_hour*cos(angel_hour),250+r_hour*sin(angel_hour),al_map_rgb(0, 0, 0),2);
            second=0;
            minute++;
        }
        if(minute%60==0){
            al_draw_line(250,250,250+r_hour*cos(angel_hour),250+r_hour*sin(angel_hour),al_map_rgb(0, 0, 0),2);
            minute=0;
            hour++;
        }


        angel_hour=((hour%12)*(PI/6)-(PI/2))+(minute/12)*(PI/30);
        al_draw_line(250,250,250+r_hour*cos(angel_hour),250+r_hour*sin(angel_hour),al_map_rgb(255, 0, 0),2);

        angel_minute=(minute*(PI/30) -(PI/2));
        al_draw_line(250,250,250+ r_minute*cos(angel_minute),250+r_minute*sin(angel_minute),al_map_rgb(0, 250, 0),2);
        angel_second=((second)*(PI/30) -(PI/2));
        al_draw_line(250,250,250+ r_second*cos(angel_second),250+r_second*sin(angel_second),al_map_rgb(2*i, i, 255-(2*i)),2);
        al_draw_circle(250,250,2,al_map_rgb(255,255,255),2);
        al_flip_display();

        sprintf(title,"%02d:%02d:%02d",hour,minute,second);
        al_set_window_title(display,title);
        second++;
        al_rest(1);

    }
}
void draw_background(int k){
            ALLEGRO_FONT* font = al_create_builtin_font();
            char address[10];
            sprintf(address,"backgrounds/b%ds.jpg",k);
            al_draw_bitmap(al_load_bitmap(address),0,0,0);
            al_draw_text(font,al_map_rgb(255, 255, 255), 200-80, 15, 100, "Press ESC for back to Main Menu");
            al_draw_filled_rounded_rectangle(105-1,100-1,400+1,400+1,50,50,al_map_rgb(0, 0, 0));
            al_draw_rounded_rectangle(105-1,100-1,400+1,400+1,50,50,al_map_rgb(255, 255, 0),4);
            al_draw_rounded_rectangle(111,106,395,395,44,44,al_map_rgb(255, 0, 0),1);

          int i;
          double angel_hour;
          for(i=1;i<=12;i++){
            angel_hour=(i*(PI/6)-(PI/2));
            al_draw_textf(font,al_map_rgb(255, 255, 255), 250+125*cos(angel_hour),250+125*sin(angel_hour), 100,"%d",i);
          }
          for(i=1;i<=60;i++){
            if(i%5!=0){
                angel_hour=(i*(PI/30)-(PI/2));
                al_draw_textf(font,al_map_rgb(255, 0, 0), 250+125*cos(angel_hour),250+125*sin(angel_hour), 100,"%c",'.');
            }
          }

}

void free_draw(){
    ALLEGRO_DISPLAY* display = al_get_current_display();
    al_clear_to_color(al_map_rgb(255,255,255));
    al_draw_bitmap(al_load_bitmap("backgrounds/freedraw.jpg"),0,0,0);
    al_draw_text(al_create_builtin_font(),al_map_rgb(0, 0, 0), 10, 50, 100, "you can draw a clock picture and right click to recognize it");
    al_flip_display();
    ALLEGRO_MOUSE_STATE state;
    ALLEGRO_KEYBOARD_STATE state2;
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_COLOR color=al_map_rgb(0,0,0);
    char pen[20] ;
    al_set_mouse_z(5);
    al_show_native_message_box(display,"Welcome!","*How to draw*","- left Click to draw\n- right click to read clock \n- scroll to change pen size\n- space: Enable\\disable Eraser\n- delete: clear all\n *press ESC for back to main menu*","OK",ALLEGRO_MESSAGEBOX_QUESTION);
    al_resize_display(display,800,500);
    al_clear_to_color(al_map_rgb(255,255,255));
    al_flip_display();

    ALLEGRO_BITMAP  *bitmap=al_create_bitmap(800,500);
    al_set_target_bitmap(bitmap);
    al_clear_to_color(al_map_rgb(255,255,255));
    al_register_event_source(queue, al_get_mouse_event_source());
   while(!al_key_down(&state2,ALLEGRO_KEY_ESCAPE)){
        al_set_target_bitmap(bitmap);
        al_get_mouse_state(&state);
        al_get_keyboard_state(&state2);
       if (al_key_down(&state2,ALLEGRO_KEY_DELETE)){
                al_clear_to_color(al_map_rgb(255,255,255));
       }
       if (al_key_down(&state2,ALLEGRO_KEY_SPACE)){
            al_set_system_mouse_cursor(display,ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION);
            if(color.r!=1)
                 al_show_native_message_box(display,"","","Eraser Enable","OK",ALLEGRO_MESSAGEBOX_WARN);
            color=al_map_rgb(255,255,255);
       }else {
            al_set_system_mouse_cursor(display,ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
            if(color.r==1)
                al_show_native_message_box(display,"","","Eraser disable","OK",ALLEGRO_MESSAGEBOX_ERROR);
            color=al_map_rgb(0,0,0);
       }
       sprintf(pen,"pen size:%0.1lf",0.2*state.z);
        al_set_window_title(display,pen);
        if(state.buttons==2){
            al_save_bitmap("out.bmp",bitmap);
            read_clock("out.bmp");
        }
        else if(state.buttons==1)
            al_draw_circle(state.x,state.y,0.1* state.z,color,10);

        al_set_target_bitmap(al_get_backbuffer(display));
        al_draw_bitmap(bitmap,0,0,4);
        al_flip_display();

    }

}

void read_clock(char *address){
    int i,n,number,width,height,width0,height0,hour,minute,second,check=1;
    Clock[0] = 0;Clock[1]=0;Clock[2]=0;
    i=1;
    check=1;
    readBMP(address,&width,&height,pic);
    width0=width;
    height0=height;
    while(check==1){
        check = resize(&width,&height,picture_size,picture_size);;
        if(check==1){
            if(i%3==0){
               printf(":");
            }else{
                number=recognizing();
                printf("%d",number);
                Clock[i/3]= Clock[i/3]*10 + number;
             }
             width=width0;
             height=height0;
             i++;
          }
    }
    printf("\n");
}

void digital_clock(int hour,int minute,int second){
    char number[20];
    ALLEGRO_DISPLAY* display = al_get_current_display();
    al_set_window_title(display,"Result");
    al_resize_display(display,750,300);
    ALLEGRO_BITMAP  *bitmap=al_load_bitmap("numbers/frame0.jpg");
    al_set_target_bitmap(bitmap);
    int x_start=90;
    int y_start=112-20;
    sprintf(number,"numbers/%d.png",hour/10);
    al_draw_bitmap(al_load_bitmap(number),x_start+0,y_start,0);
    sprintf(number,"numbers/%d.png",hour%10);
    al_draw_bitmap(al_load_bitmap(number),x_start+79,y_start,0);

    al_draw_bitmap(al_load_bitmap("numbers/colon.png"),x_start+79*2,y_start+10,0);

    sprintf(number,"numbers/%d.png",minute/10);
    al_draw_bitmap(al_load_bitmap(number),x_start+79*2 + 49,y_start,0);
    sprintf(number,"numbers/%d.png",minute%10);
    al_draw_bitmap(al_load_bitmap(number),x_start+79*3 + 49,y_start,0);

    al_draw_bitmap(al_load_bitmap("numbers/colon.png"),x_start+79*4 + 49,y_start+10,0);

    sprintf(number,"numbers/%d.png",second/10);
    al_draw_bitmap(al_load_bitmap(number),x_start+79*4 + 2*49 ,y_start,0);
    sprintf(number,"numbers/%d.png",second%10);
    al_draw_bitmap(al_load_bitmap(number),x_start+79*5 + 2*49,y_start,0);

    al_set_target_bitmap(al_get_backbuffer(display));
    al_draw_bitmap(bitmap,0,0,4);
    al_flip_display();
    al_rest(6);

}
void convert_time(){
    ALLEGRO_DISPLAY *display=al_get_current_display();
    al_show_native_message_box(display,"","","please enter name of a city in CMD Windows","OK",ALLEGRO_MESSAGEBOX_QUESTION);
    char city[20],line[80];
    int hour=Clock[0],minute=Clock[1],second=Clock[2];
    hour-=3;
    minute-=30;
    printf("Enter City name (for example : London,Tokyo,..) :\n");
    scanf("%s",city);
    int temp=-1,hr,min;
    FILE *zones=fopen("time-zones.txt","r");
    while(fgets(line,80,zones)!=NULL){
        if(strstr(line,city)!=NULL){
            sscanf(line,"%d='[GMT%d:%d]",&temp,&hr,&min);

            hour+=hr;
            if(hr<0){
                minute-=min;
            }else minute+=min;

            if(minute>=60){
                minute-=60;
                hour++;
            }
            if(minute<0){
                minute+=60;
                hour--;
            }
            char result[30];
            sprintf(result,"%02d:%02d:%02d",hour,minute,second);
            printf("%s\n",result);
             al_show_native_message_box(display,"",city,result,"OK",ALLEGRO_MESSAGEBOX_QUESTION);
            break;
        }
    }
     if(temp==-1){
        al_show_native_message_box(display,"",city,"not found","OK",ALLEGRO_MESSAGEBOX_ERROR);
        printf("cannot find this city");
     }

    fclose(zones);
}

int crop(int *width,int *height){
    int i,j,x,y,up=-1,down=-1,left=-1,right=-1;
    // shenasayi marz ha:
    find_marz(width,height);
    for(i=0;i<*height;i++){
       for(j=0;j<*width;j++){
         if(!(pic[i][j][0]==255 && pic[i][j][1]==255 &&pic[i][j][2]==255)){
            if(up==-1){
                up=i;
                left=j;
            }
            if(i>down)
                down=i;
            if(j>right){
                right=j;
            }else if(j<left)
                left = j;
         }
       }
    }

    if(up==-1)  //safhe kamel sefid va shekli peyda nashod
        return 0;

// copy kardan dar araye jadid:
    i=0;
    *width=right-left;
    *height=down-up;
    for(y=up;y<=down;y++){
        j=0;
      for (x=left;x<=right;x++){
        cropped_pic[i][j][0]=pic[y][x][0];
        cropped_pic[i][j][1]=pic[y][x][1];
        cropped_pic[i][j][2]=pic[y][x][2];
        pic[y][x][0]=255;
        pic[y][x][1]=255;
        pic[y][x][2]=255;
        j++;
      }
      i++;
    }

    return 1;
}

void find_marz(int *right_line,int *down_line){

    find_marz_amoodi(right_line,down_line);

    //find_marz_ofoghi(right_line,down_line);
}

void find_marz_amoodi(int *right_line,int *down_line){
    int x,y,i,j,start=-1,found=0;

    for(j=0; j<*right_line && start==-1 ;j++){
      for(i=0; i<*down_line && start==-1 ;i++){
         if(!(pic[i][j][0]==255 && pic[i][j][1]==255 &&pic[i][j][2]==255)){
            start=j;
         }
       }
    }


    for(y=0;y<*down_line && found!=1;y++){
        for(x=start;x<*right_line && found!=1;x++){
            if( pic[y][x][0]==255 && pic[y][x][1]==255 &&pic[y][x][2]==255 ){
                found=1;
                for(i=0;i<*down_line;i++){
                    if (pic[i][x][0]!=255 || pic[i][x][1]!=255 || pic[i][x][2]!=255){
                        found=0;
                        break;
                    }
                }
            }
        }
    }
    if(found==1){
        *right_line=x;
     }
}

void find_marz_ofoghi(int *right_line,int *down_line){
    int x,y,i,j,start=-1,found=0;
    for(i=0; i<*down_line && start==-1 ;i++){
       for(j=0; j<*right_line && start==-1 ;j++){
         if(!(pic[i][j][0]==255 && pic[i][j][1]==255 &&pic[i][j][2]==255)){
            start=i;
         }
       }
    }

    for(x=0;x<*right_line && found!=1;x++){
        for(y=start;y<*down_line && found!=1;y++){
            if( pic[y][x][0]==255 && pic[y][x][1]==255 &&pic[y][x][2]==255 ){
                found=1;
                for(i=0;i<*right_line;i++){
                    if (pic[y][i][0]!=255 || pic[y][i][1]!=255 || pic[y][i][2]!=255){
                        found=0;
                        break;
                    }
                }
            }
        }
    }
    if(found==1){
        *down_line=y;
     }
}

int resize(int *width,int *height,double new_width,double new_height){
    int x,y,i,found_pic;
    found_pic = crop(width,height);
    if(found_pic==0){
        return 0;
    }

     double height_ratio= new_height/ *height;
     double width_ratio= new_width/ *width;
     double remaining_ratio=0;
    // resize ofoghi:
     for(y=0;y<*height;y++){
       i=0;
       for(x=0;x<*width;x++){
         remaining_ratio += width_ratio;
         while(remaining_ratio>0){
            if(remaining_ratio>=1){
                temp_pic[y][i][0]=cropped_pic[y][x][0];
                temp_pic[y][i][1]=cropped_pic[y][x][1];
                temp_pic[y][i][2]=cropped_pic[y][x][2];
            }else{
                temp_pic[y][i][0]=( cropped_pic[y][x][0] * remaining_ratio)+( cropped_pic[y][x+1][0] * (1- remaining_ratio) );
                temp_pic[y][i][1]=( cropped_pic[y][x][1] * remaining_ratio)+( cropped_pic[y][x+1][1] * (1- remaining_ratio) );
                temp_pic[y][i][2]=( cropped_pic[y][x][2] * remaining_ratio)+( cropped_pic[y][x+1][2] * (1- remaining_ratio) );

            }
            remaining_ratio--;
            i++;
         }
       }
     }
    // resize amoodi:
     for(x=0;x<new_width;x++){
       i=0;
       for(y=0;y<*height;y++){
         remaining_ratio += height_ratio;
         while(remaining_ratio>0){
            if(remaining_ratio>=1){
                resized_pic[i][x][0]=temp_pic[y][x][0];
                resized_pic[i][x][1]=temp_pic[y][x][1];
                resized_pic[i][x][2]=temp_pic[y][x][2];
            }else{
                resized_pic[i][x][0]=( temp_pic[y][x][0] * remaining_ratio)+( temp_pic[y+1][x][0] * (1- remaining_ratio) );
                resized_pic[i][x][1]=( temp_pic[y][x][1] * remaining_ratio)+( temp_pic[y+1][x][1] * (1- remaining_ratio) );
                resized_pic[i][x][2]=( temp_pic[y][x][2] * remaining_ratio)+( temp_pic[y+1][x][2] * (1- remaining_ratio) );

            }
            remaining_ratio--;
            i++;
         }
       }
     }
    return 1;
}
void learning(){
    int folder,i,j,n,width,height,min0,min1,min2,d;
    char address[30],wait[30];
    printf("please wait for learning..\n");
    for(folder=0;folder<10;folder++){
        min0=255;
        min1=255;
        min2=255;
        sprintf(wait,"backgrounds/wait%d.jpg",folder);
        al_draw_bitmap(al_load_bitmap(wait),0,0,0);
        al_flip_display();
        // miangin giri :
        for(n=1;n<=dataset_counts;n++){
            sprintf(address,"DataSet/%d/%d.bmp",folder,n);
            readBMP(address,&width,&height,pic);
            resize(&width,&height,picture_size,picture_size);
            for(i=0;i<picture_size;i++){
               for(j=0;j<picture_size;j++){
                   average[i][j][0]+=resized_pic[i][j][0];
                   average[i][j][1]+=resized_pic[i][j][1];
                   average[i][j][2]+=resized_pic[i][j][2];
               }
            }
        }
        // be dast avardan por rang tarin pixel:
        for(i=0;i<picture_size;i++){
            for(j=0;j<picture_size;j++){
                average_pic[i][j][0]= average[i][j][0]/dataset_counts;
                average_pic[i][j][1]= average[i][j][1]/dataset_counts;
                average_pic[i][j][2]= average[i][j][2]/dataset_counts;
               if(average_pic[i][j][0]<min0)  min0= average_pic[i][j][0];
               if(average_pic[i][j][0]<min1)  min1= average_pic[i][j][1];
               if(average_pic[i][j][0]<min2)  min2= average_pic[i][j][2];
            }
        }
        //por rang sazi:
        for(i=0;i<picture_size;i++){
            for(j=0;j<picture_size;j++){
               if(average_pic[i][j][0]<250 )
                    average_pic[i][j][0]= average_pic[i][j][0]-min0;
                if(average_pic[i][j][1]<250 )
                    average_pic[i][j][1]= average_pic[i][j][1] -min1;
                if(average_pic[i][j][2]<250 )
                    average_pic[i][j][2]= average_pic[i][j][2] -min2;


               if(folder==4 ||folder==7 || folder==8 || folder==9){
                  if(average_pic[i][j][0]<140 )
                      average_pic[i][j][0]= 0;
                  if(average_pic[i][j][1]<140 )
                      average_pic[i][j][1]= 0;
                  if(average_pic[i][j][2]<140 )
                      average_pic[i][j][2]= 0;
               }else{
                    if(average_pic[i][j][0]<90 )
                        average_pic[i][j][0]= 0;
                    if(average_pic[i][j][1]<90 )
                        average_pic[i][j][1]= 0;
                    if(average_pic[i][j][2]<90 )
                        average_pic[i][j][2]= 0;
               }
              if(folder==0){
                 if(average_pic[i][j][0]>180 )
                    average_pic[i][j][0]= 255;
                 if(average_pic[i][j][1]>180 )
                    average_pic[i][j][1]= 255;
                 if(average_pic[i][j][2]>180 )
                    average_pic[i][j][2]= 255;
             }else{
                 if(average_pic[i][j][0]>200 )
                    average_pic[i][j][0]= 255;
                 if(average_pic[i][j][1]>200 )
                    average_pic[i][j][1]= 255;
                 if(average_pic[i][j][2]>200 )
                    average_pic[i][j][2]= 255;
             }
            }
        }
        sprintf(address,"DataSet/Learned%d.bmp",folder);
        saveBMP(average_pic,picture_size,picture_size,address);
     // clearing array "average" :
        for(i=0;i<picture_size;i++){
            for(j=0;j<picture_size;j++){
                average[i][j][0]=0;
                average[i][j][1]=0;
                average[i][j][2]=0;
            }
        }
    }
    printf("Learning Done!\n");
}
int recognizing(){
    int i,j,n,k,width,height,difference,d,min,number;
    char address[30];
    for(n=0;n<=9;n++){
      difference=0;
      sprintf(address,"DataSet/Learned%d.bmp",n);
      readBMP(address,&width,&height,temp_pic);
      for(i=0;i<picture_size;i++){
        for(j=0;j<picture_size;j++){
          for(k=0;k<3;k++){
            d= temp_pic[i][j][k]-resized_pic[i][j][k];
            if(d<0) d=-d;
            difference+=d;
          }
        }
      }
      if(difference<min ||n==0){
        min=difference;
        number=n;
      }
    }
    return number;
}

