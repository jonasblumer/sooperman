#include "allegro5/allegro.h"
#include "math.h"
#include "time.h"

#include <stdlib.h>
#include <stdio.h>

#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define DEGTORAD(x) ((x)*(ALLEGRO_PI/180.0))

/*
**      gcc /Users/Jonas/Downloads/allegro-5.0.10/tests/tardy_bird.c -o tardy_bird -L/usr/local/lib -lallegro.5.0.10 -lallegro_main.5.0.10 -lallegro_image.5.0.10 -lallegro_dialog.5.0.10 -lallegro_ttf.5.0.10 -lallegro_font.5.0.10 -fnested-functions -lallegro_primitives.5.0.10
*/

const float FPS = 60;
const float bounceSpeed = 15;
const float GRAVITY = -.2;
const float bounceSpeedX = 2;
const float FRICTION = 1;
const float MAX_Y_SPEED = 10;
const float MAX_X_SPEED = 10;
const int xRES = 1080*1.2;
const int yRES = 720*1.2;
const int windowWidth = 28;
const int windowHeight = 40;
const int windowSpace = 12;
const float ufoSpeed = 6;
const int ufoDownTime = 10;
const float shotSpeed = 8;
int ufoX, ufoY, shotX, shotY, shotTargetX, shotTargetY;
bool ufoXDirection, ufoYDirection, ufoStart, isShooting;
double shot_direction;
 int framecount = 0;


ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *houseTop = NULL;
ALLEGRO_BITMAP *houseBottom = NULL;
ALLEGRO_BITMAP *houseMiddle = NULL;

char seconds[100];
char playMenu[10];
char quitMenu[10];
char highScoreMenu[10];

time_t start, end;


enum GAME_STATE{
  IN_MENU, IN_GAME, IN_GAME_IN_MENU, IS_DYING
};

enum MYKEYS {
   KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};
bool key[4] = { false, false, false, false };

int county;
int animCounty;



int bounding_box_collision(int b1_x, int b1_y, int b1_w, int b1_h, int b2_x, int b2_y, int b2_w, int b2_h)
{
    if ((b1_x > b2_x + b2_w - 1) || // is b1 on the right side of b2?
        (b1_y > b2_y + b2_h - 1) || // is b1 under b2?
        (b2_x > b1_x + b1_w - 1) || // is b2 on the right side of b1?
        (b2_y > b1_y + b1_h - 1))   // is b2 under b1?
    {
        // no collision
        return 0;
    }
 
    // collision
    return 1;
}



float housePosX, backgroundPosX, skyPosX, xVelocity, yVelocity, gameSpeed;
int currentX, currentY, houseHeightTop, houseHeightBottom;
bool redraw;

void drawWindows(int houseHeight){
    int windowPosX = 5;
    int windowPosY = 5;
    for(; windowPosX < 200-windowWidth; windowPosX += windowWidth+windowSpace){
        for(; windowPosY < houseHeight; windowPosY += windowHeight+windowSpace){
            int randWindowColor = rand()%200+55;
            if(randWindowColor<20) randWindowColor = 30;
            al_draw_filled_rectangle(windowPosX, windowPosY, windowPosX+windowWidth, windowPosY+windowHeight, al_map_rgb(randWindowColor, randWindowColor, randWindowColor-20));
        }
        windowPosY = 5;
    }
}

void calcRandHouseHeight(){
    //houseHeightTop= rand()%(yRES-200)+100;
    houseHeightTop = rand()%(yRES-300)+100;

    houseHeightBottom= yRES-houseHeightTop-200;
    if(houseHeightBottom < 0){
    houseHeightBottom = 0;
    }
    int randomColor = rand()%200;

    houseTop = al_create_bitmap(200, houseHeightTop);
    al_set_target_bitmap(houseTop);
    al_clear_to_color(al_map_rgb(randomColor, randomColor, randomColor));
    drawWindows(houseHeightTop);
    houseBottom = al_create_bitmap(200, houseHeightBottom);
    al_set_target_bitmap(houseBottom);
    al_clear_to_color(al_map_rgb(randomColor, randomColor, randomColor));
    drawWindows(houseHeightBottom);
    //al_set_target_bitmap(houseMiddle);
    //al_clear_to_color(al_map_rgb(200, 200, 200));
    al_set_target_bitmap(al_get_backbuffer(display));

  
}

void init_vars(){
    time(&start);
    housePosX = 0;
    backgroundPosX = 0;
    skyPosX = 0;
    currentX= 200;
    currentY = 300;
    yVelocity = GRAVITY;
    xVelocity = 0;
    gameSpeed = 4;
    redraw = true;
    ufoX = xRES;
    ufoY = 0;
    shotX = -100;
    shotY = -100;
    ufoXDirection = false; // left
    ufoYDirection = false; // down
    ufoStart = false;
    isShooting = false;
    county = 0;
    animCounty = 0;
    calcRandHouseHeight();
}



void drawMenu(int selection, ALLEGRO_FONT *font, ALLEGRO_BITMAP *ship1){
    switch(selection){
        case 0:
            al_draw_text(font, al_map_rgb(200, 0, 50), xRES/2, 300, ALLEGRO_ALIGN_CENTER, "PLAY");
            al_draw_text(font, al_map_rgb(200, 200, 200), xRES/2, 400, ALLEGRO_ALIGN_CENTER, "HIGHSCORE");
            al_draw_text(font, al_map_rgb(200, 200, 200), xRES/2, 500, ALLEGRO_ALIGN_CENTER, "EXIT");
            al_draw_scaled_bitmap(ship1, 0, 0, 120, 37, 400, 300, 120, 37, 0);
        break;
        case 1:
            al_draw_text(font, al_map_rgb(200, 200, 200), xRES/2, 300, ALLEGRO_ALIGN_CENTER, "PLAY");
            al_draw_text(font, al_map_rgb(200, 0, 50), xRES/2, 400, ALLEGRO_ALIGN_CENTER, "HIGHSCORE");
            al_draw_text(font, al_map_rgb(200, 200, 200), xRES/2, 500, ALLEGRO_ALIGN_CENTER, "EXIT");
            al_draw_scaled_bitmap(ship1, 0, 0, 120, 37, 400, 400, 120, 37, 0);
        break;
        case 2:
            al_draw_text(font, al_map_rgb(200, 200, 200), xRES/2, 300, ALLEGRO_ALIGN_CENTER, "PLAY");
            al_draw_text(font, al_map_rgb(200, 200, 200), xRES/2, 400, ALLEGRO_ALIGN_CENTER, "HIGHSCORE");
            al_draw_text(font, al_map_rgb(200, 0, 50), xRES/2, 500, ALLEGRO_ALIGN_CENTER, "EXIT");
            al_draw_scaled_bitmap(ship1, 0, 0, 120, 37, 400, 500, 120, 37, 0);
        break;
    }
}


int main(int argc, char **argv){

    enum GAME_STATE game;
    game = IN_MENU;
    time(&start); // log time of start of program

    bool isAlive = true; // player starts off alive

    srand(time(NULL)); // init random seed
 

    ALLEGRO_DISPLAY_MODE disp_data;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    /*
    ** init graphics vars
    */ 
    ALLEGRO_BITMAP  *sky   = NULL;
    ALLEGRO_BITMAP  *ufo   = NULL;
    ALLEGRO_BITMAP  *background   = NULL;
    ALLEGRO_BITMAP  *shot   = NULL;
    ALLEGRO_BITMAP  *ship1   = NULL;
    ALLEGRO_BITMAP  *ship2   = NULL;
    ALLEGRO_BITMAP  *ship3   = NULL;
    ALLEGRO_BITMAP  *ship4   = NULL;
    ALLEGRO_BITMAP  *ship5   = NULL;
    ALLEGRO_BITMAP  *ship6   = NULL;
    ALLEGRO_BITMAP  *currentShip = NULL;
    ALLEGRO_FONT *font =NULL;



    /*
    ** install needed modules
    */

    al_init();
    al_install_mouse();
    al_install_keyboard();
    timer = al_create_timer(1.0 / FPS);
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();// initialize the ttf (True Type Font) addon
    al_init_image_addon();
 
   
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST); // turn on vsync
    display = al_create_display(xRES, yRES); //create the display

    /*
    **  LOAD ALL THE GRAPHICS
    */
    font = al_load_ttf_font("/Users/Jonas/Documents/GameGraphics/COMICATE.TTF", 30, ALLEGRO_TTF_NO_KERNING);
    sky = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sky.png");
    ufo = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/ufo.png");
    shot = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/shot.png");
    ship1 = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sman1.png");
    ship2 = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sman2.png");
    ship3 = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sman3.png");
    ship4 = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sman4.png");
    ship5 = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sman5.png");
    ship6 = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/sman6.png");
    currentShip = ship1;
    background = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/skyline2.png");  
    houseMiddle = al_load_bitmap("/Users/Jonas/Documents/GameGraphics/houseMiddle.png");

    event_queue = al_create_event_queue();
    if(!event_queue) {
    fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_bitmap(ship1);
        al_destroy_bitmap(houseTop);
        al_destroy_bitmap(houseBottom);
        al_destroy_bitmap(sky);
        al_destroy_bitmap(shot);
        al_destroy_bitmap(background);
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
 
    al_register_event_source(event_queue, al_get_display_event_source(display));
 
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    int selected = 0;



    do{ 

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        /*
        **  MAIN MENU SCREEN
        */

        if(game == IN_MENU){
    
        al_clear_to_color(al_map_rgb(0,0,0));

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    key[KEY_UP] = true;
                break;
                case ALLEGRO_KEY_DOWN:
                    key[KEY_DOWN] = true;
                break;
                case ALLEGRO_KEY_ENTER:
                    switch(selected){
                        case 0: 
                            init_vars();
                            game = IN_GAME;
                        break;
                        case 1:
                        break;
                        case 2: 
                            return 0;
                        break;
                    }    
                break;
            }
        }

        bool draw = false;

        if(key[KEY_DOWN] == true){
            if(selected < 2){
                selected += 1;
            }else{
                selected = 0;
            }
            key[KEY_DOWN]= false;

        }
        if(key[KEY_UP] == true){
            if(selected > 0){
                selected -= 1;
            }else{
                selected = 2;
            }
            key[KEY_UP]= false;
        }


        if(ev.type == ALLEGRO_EVENT_TIMER){
            drawMenu(selected, font, ship1);
            al_flip_display();
        }

      
    


















    /*
    **
    **
    **
    **
    **  MAIN GAME
    **
    **
    **
    */

    }else if(game == IN_GAME){
    
      

        if(ev.type == ALLEGRO_EVENT_TIMER) {
                       
        
            /*
            ** UFO MOVEMENT
            */
            if(ufoStart == true){



                if(ufoX <= 0){
                    ufoXDirection = true;
                }
                if (ufoX >= xRES-256){
                    ufoXDirection = false;
                }
                if(ufoXDirection==true){
                    ufoX += ufoSpeed;
                }else{
                    ufoX -= ufoSpeed;
                }
                if(ufoY >= 200){
                    ufoYDirection = true;
                }
                if(ufoY <= 0){
                    ufoYDirection = false;
                }
                if(ufoYDirection==true){
                    ufoY-=ufoSpeed/2;
                }else{
                    ufoY += ufoSpeed/2;
                }

     
                if(isShooting == false){
                    isShooting = true;
                    shotTargetX = currentX;
                    shotTargetY = currentY;
                    shotX = ufoX+al_get_bitmap_width(ufo)/2;
                    shotY = ufoY+al_get_bitmap_height(ufo)/4;
            
                }
                if(isShooting == true){
                    shotY += shotSpeed;
                    shotX -= gameSpeed/8;
                    if(shotY >= yRES+10){
                         isShooting = false;
                    }
                }
            }


            /*
            **  DRAW HOUSE AND BACKGROUND
            */


            if(housePosX<=-xRES-200){
                housePosX = 0;
                al_destroy_bitmap(houseTop);
                al_destroy_bitmap(houseBottom);
                calcRandHouseHeight();

            }
            if(backgroundPosX<=-xRES){
                backgroundPosX = 0;
            }
            if(skyPosX <= -xRES*1.1){
                skyPosX = 0;
            }
         

            housePosX-=gameSpeed;
            backgroundPosX -= gameSpeed/4;
            skyPosX -= gameSpeed/8;
            gameSpeed += 0.0045;
      

            /*
            ** CALCULATE PLAYER MOVEMENT & GET INPUT
            */

            if(yVelocity >= -MAX_Y_SPEED){
                yVelocity += GRAVITY;
            }else{
                yVelocity = -MAX_Y_SPEED;
            }
            if(yVelocity <= MAX_Y_SPEED*2){
                yVelocity += GRAVITY;
            }else{
                yVelocity = MAX_Y_SPEED*2;
            }

            currentY -= yVelocity;
          
         
            if(xVelocity > 0){
                if(xVelocity<=MAX_X_SPEED){
                    xVelocity -= FRICTION;
                }else{
                    xVelocity = MAX_X_SPEED;
                }
            }else if(xVelocity < 0){
        
                if(xVelocity>=-MAX_X_SPEED){
                    xVelocity += FRICTION;
                }else{
                    xVelocity = -MAX_X_SPEED+.1;
                }
            }else{
                xVelocity = 0;
            }
            currentX -= xVelocity;
            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
      
        if(key[KEY_UP] == true){
            yVelocity += bounceSpeed;
            key[KEY_UP] = false;
        }

        else if(key[KEY_RIGHT] == true){
            xVelocity -= bounceSpeedX;
        }else if(key[KEY_LEFT] == true){
            xVelocity += bounceSpeedX;
        }
      
      
        if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_SPACE:
                    key[KEY_UP] = true;
                break;
                case ALLEGRO_KEY_A: 
                    key[KEY_LEFT] = true;
                break;
                case ALLEGRO_KEY_D:
                    key[KEY_RIGHT] = true;
                break;
            }
        }else
        if(ev.type == ALLEGRO_EVENT_KEY_UP){
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_SPACE:
                    key[KEY_UP] = false;
                break;
                case ALLEGRO_KEY_A: 
                    key[KEY_LEFT] = false;
                break;
                case ALLEGRO_KEY_D:
                    key[KEY_RIGHT] = false;
                break;
            }
        }








        /*
        **  HANDLE COLLISION
        */

        // collision with house
        if( bounding_box_collision(currentX, currentY, al_get_bitmap_width(ship1), al_get_bitmap_height(ship1), xRES+housePosX, 0, 200, houseHeightTop) || 
            bounding_box_collision(currentX, currentY, al_get_bitmap_width(ship1), al_get_bitmap_height(ship1), xRES+housePosX, yRES-houseHeightBottom, 200, houseHeightBottom)){
            game = IS_DYING;
            key[KEY_LEFT] = false; 
            key[KEY_RIGHT] = false;
        }

        // collision with bullet
        if(bounding_box_collision(currentX, currentY, al_get_bitmap_width(ship1), al_get_bitmap_height(ship1), shotX, shotY, al_get_bitmap_width(shot), al_get_bitmap_height(shot))){
            game = IS_DYING;
            key[KEY_LEFT] = false; 
            key[KEY_RIGHT] = false;
        }

        // collision with UFO
        if(bounding_box_collision(currentX, currentY, al_get_bitmap_width(ship1), al_get_bitmap_height(ship1), ufoX, ufoY, al_get_bitmap_width(ufo), al_get_bitmap_height(ufo))){
            game = IS_DYING;
            key[KEY_LEFT] = false; 
            key[KEY_RIGHT] = false;
        }






        /*
        ** DRAW ALL THE THINGS
        */
        
        if(redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_draw_scaled_bitmap(sky, 0, 0, 620, 456, 0+skyPosX, 0, xRES*1.4, yRES*1.4, 0);
            al_draw_scaled_bitmap(sky, 0, 0, 620, 456, xRES*1.1+skyPosX, 0, xRES*1.4, yRES*1.4, 0);
            al_draw_scaled_bitmap(background,0, 0, 540, 360, 0+backgroundPosX,0,xRES, yRES, 0);
            al_draw_scaled_bitmap(background,0, 0, 540, 360, xRES+backgroundPosX,0,xRES, yRES, 0);
            al_draw_bitmap(houseTop, xRES+housePosX, 0, 0);
            al_draw_bitmap(houseBottom, xRES+housePosX, yRES-houseHeightBottom, 0);
            al_draw_bitmap(houseMiddle, xRES+housePosX, yRES-houseHeightBottom-210, 0);      
            
            if(county==5){

                if(animCounty%6==0){
                currentShip = ship1;
                //al_draw_scaled_bitmap(ship1, 0, 0, 120, 37, currentX, currentY, 120, 37, 0);
                }else if(animCounty%6==1){
                    currentShip = ship2;
                 //   al_draw_scaled_bitmap(ship2, 0, 0, 120, 37, currentX, currentY, 120, 37, 0);
                }else if(animCounty%6==2){
                    currentShip = ship3;
                   // al_draw_scaled_bitmap(ship3, 0, 0, 120, 38, currentX, currentY, 120, 38, 0);
                }else if(animCounty%6==3){
                    currentShip = ship4;
                    //al_draw_scaled_bitmap(ship4, 0, 0, 120, 41, currentX, currentY, 120, 41, 0);
                }else if(animCounty%6==4){
                    currentShip = ship5;
                    //al_draw_scaled_bitmap(ship5, 0, 0, 120, 40, currentX, currentY, 120, 40, 0);
                }else if(animCounty%6==5){
                    currentShip = ship6;
                //    al_draw_scaled_bitmap(ship6, 0, 0, 120, 41, currentX, currentY, 120, 41, 0);
                }
                animCounty++;
                county=0;
            }
            county++;

            al_draw_scaled_bitmap(currentShip, 0, 0, 120, 37, currentX, currentY, 120, 37, 0);

            al_draw_bitmap(shot, shotX, shotY, 0);
            al_draw_bitmap(ufo, ufoX, ufoY, 0);
        
            time(&end);
            double secs = end-start;
            
            if(end-start >= ufoDownTime){
                ufoStart = true;
            }
       
            sprintf(seconds, "Seconds %.0f", secs);
            al_draw_text(font, al_map_rgb(255, 255, 255), 230, 50, ALLEGRO_ALIGN_RIGHT, seconds);
            al_flip_display();
        }

    }else


        /*
        ** IN GAME MENU
        */

    if(game == IN_GAME_IN_MENU){
       // al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_filled_rectangle(200, 200, xRES-200, yRES-200, al_map_rgb(20, 10, 100));
        al_draw_text(font, al_map_rgb(255, 255, 255), 230, 50, ALLEGRO_ALIGN_RIGHT, seconds);
        


        if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    key[KEY_UP] = true;
                break;
                case ALLEGRO_KEY_DOWN:
                    key[KEY_DOWN] = true;
                break;
                case ALLEGRO_KEY_ENTER:
                    switch(selected){
                        case 0: 
                            init_vars();
                            game = IN_GAME;
                        break;
                        case 1:
                        break;
                        case 2: return 0;
                        break;
                    }
                break;    
            }
        }


        if(key[KEY_DOWN] == true){
            if(selected < 2){
                selected += 1;
            }else{
                selected = 0;
            }
            key[KEY_DOWN]= false;

        }

        if(key[KEY_UP] == true){
            if(selected > 0){
                selected -= 1;
            }else{
                selected = 2;
            }
            key[KEY_UP]= false;
        }

        if(ev.type == ALLEGRO_EVENT_TIMER){
            drawMenu(selected, font, ship1);
            al_flip_display();
            int framecountdown = al_get_timer_count(timer);
        }
    }else

    if(game == IS_DYING){

        gameSpeed = 0;
        game = IN_GAME_IN_MENU;

    }

}while(isAlive == true);


     
al_destroy_display(display);
al_destroy_bitmap(background);
return 0;
}