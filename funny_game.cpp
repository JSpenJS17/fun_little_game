#include "engine.hpp"
#include <iostream>
#include <string>
#include <signal.h>

using namespace std;

// #define ENTER 13
#define ESC 27
// #define UP 72
// #define LEFT 75
// #define DOWN 80
// #define RIGHT 77

class Apple;
class Player;
class Apple_Manager;

/* define global variables */
//board sizes
unsigned int size_x = 15;
unsigned int size_y = 15;

/* background color variable */
unsigned short bg_color = LIGHT_GRAY;

/* background pixel variable */
const Pixel bg = Pixel('-', bg_color, bg_color);

/* game board */
Board game = Board(size_x, size_y, bg);

class Apple{
    public:
        unsigned int row;
        unsigned int col;
        char kind;
        Pixel face;

        Apple(const unsigned int init_row, const unsigned int init_col, 
              char variety = 'g'){
            /* creates an apple object of a given variety */
            row = init_row;
            col = init_col;
            kind = variety;
            switch (kind){
                case 'g':
                    face = Pixel('g', LIGHT_BLUE, LIGHT_BLUE);
                    break;
                case 'b':
                    face = Pixel('b', LIGHT_RED, LIGHT_RED);
                    break;
                case 's':
                    face = Pixel('s', LIGHT_YELLOW, LIGHT_YELLOW);
                    break;
                case 'm':
                    face = Pixel('m', GRAY, GRAY);
                    break;
                case 't':
                    face = Pixel('t', MAGENTA, MAGENTA);
                    break;
            }
        }

        inline bool operator==(Apple o){
            return row == o.row && col == o.col 
                    && face == o.face && kind == o.kind;
        }

        Apple(){};

};

class Player{
    public:
        unsigned int row;
        unsigned int col;
        Pixel face;
        //other things the player has, like items or whatever
        int length;

        Player(const unsigned int init_row, const unsigned int init_col,
                const Pixel disp_pixel){
            /* creates a player character */
            row = init_row;
            col = init_col;
            face = disp_pixel;
            length = 0;
            game.write(row, col, face);
        }

        void move(char key){
            /* move the character based on the key pressed */
            //write background to the old position
            game.write(row, col);
            switch (key){
                //move the character but never out of bounds
                case 'w':
                    if (row > 0)
                        row--;
                    break;

                case 's':
                    if (row < size_y - 1)
                        row++;
                    break;

                case 'a':
                    if (col > 0)
                        col--;
                    break;

                case 'd':
                    if (col < size_x - 1)
                        col++;
                    break;
            }
            //write the character to the new pos
            game.write(row, col, face);
        }
};

class Apple_Manager{
    public:
        void spawn_apple(Player guy, const char kind){
            /* spawns a new apple and writes it to the game board */
            unsigned int row;
            unsigned int col;
            //keep finding new spawn positions until
            // 1. the player isn't within 2.5 units of the spawn position and
            // 2. the spawn position isn't on top of an existing apple
            do {
               row = rand_int(size_y-1);
                col = rand_int(size_x-1);
            } while (on_apple(row, col) 
                     || dist(guy.col, guy.row, col, row) <= 2.5);

            Apple new_apple = Apple(row, col, kind);
            apple_list.push_back(new_apple);
            game.write(new_apple.row, new_apple.col, new_apple.face);
        }

        void remove_apple(Apple apple){
            /* deletes an apple and removes it from the game board */
            for (unsigned int i = 0; i < apple_list.size(); i++){
                Apple o_apple = apple_list.at(i);
                if (apple == o_apple)
                    apple_list.erase(apple_list.begin() + i);
            }
            game.write(apple.row, apple.col, bg);
        }

        Apple pop_apple(){
            /* removes the latest apple added to the list */
            Apple apple = apple_list.back();
            apple_list.pop_back();
            game.write(apple.row, apple.col, bg);
            return apple;
        }

        void clear_apples(){
            /* clears all apples from the apple list and game board. */
            for (unsigned int i = 0; i < apple_list.size(); i++){
                Apple apple = apple_list.at(i);
                game.write(apple.row, apple.col, bg);
            }
            apple_list.clear();
        }

        int on_apple(unsigned const int row, unsigned const int col){
            /* returns 1 if the row, col is on an apple */
            for (unsigned int i = 0; i < apple_list.size(); i++){
                Apple apple = apple_list.at(i);
                if (apple.row == row && apple.col == col)
                    return 1;
            }
            return 0;
        }

        Apple get_apple_at(unsigned const int row, unsigned const int col){
            /* returns the apple at the row, col inputted 
             * throws an exception if there is no apple
             */
            for (unsigned int i = 0; i < apple_list.size(); i++){
                Apple apple = apple_list.at(i);
                if (apple.row == row && apple.col == col)
                    return apple;
            }
            throw invalid_argument("received invalid apple coordinates");
        }

    private:
        vector<Apple> apple_list;

};

void display_header(const int score, const int score_change, 
                    const int time_ms, const int time_change_ms){
    set_cursor_pos(0, 0);
    char str[40];

    //print score
    sprintf(str, "Score: %3d", score);
    cout << str;

    //print out score header with score change to the side
    if (score_change < 0){
        //print score change negative
        color(BLACK, LIGHT_RED);
        sprintf(str, " %+4d", score_change);
        cout << str << endl;
    }

    else if (score_change > 0){
        //print score change positive
        color(BLACK, LIGHT_BLUE);
        sprintf(str, " %+4d", score_change);
        cout << str << endl;

    } else {   
        //print out some extra whitespace to cover old score increment
        cout << "      Last Collected" << endl;
    }

    //reset color
    color(16, 16);

    //print time
    sprintf(str, "Time: %4.1f", (double) time_ms/1000);
    cout << str;

    if (time_change_ms < 0) {
        //print time change negative
        color(BLACK, LIGHT_RED);
        sprintf(str, " %+03.1f", (double) time_change_ms/1000);
        cout << str;
    }

    else if (time_change_ms > 0){
        //print time change positive
        color(BLACK, LIGHT_BLUE);
        sprintf(str, " %+03.1f", (double) time_change_ms/1000);
        cout << str;
    
    } else {
        //print out some extra whitespace to cover old time increment
        cout << "     ";
    }

    //reset color
    color(16, 16);
}

char convert_to_wasd(const char key){
    /* converts arrow key inputs to WASD */
    switch (key){
        case UP:
            return 'w';

        case DOWN:
            return 's';

        case RIGHT:
            return 'd';

        case LEFT:
            return 'a';
    }
    return key;
}

void display_main_menu(){
    /* displays the main menu */
    color(LIGHT_BLUE, LIGHT_BLUE);
    for (int i = 0; i < (size_x*2-10)/2; i++)
        cout << "-";
    
    color(LIGHT_GRAY, LIGHT_BLUE);
    cout << "funny";
    
    color(LIGHT_GRAY, LIGHT_RED);
    cout << " game";
    
    color(LIGHT_RED, LIGHT_RED);
    for (int i = 0; i < (size_x*2-10)/2; i++)
        cout << "-";
    cout << endl << endl;

    Apple display_apple = Apple(0, 0, 'g');
    draw_pixel(display_apple.face);
    cout << " ";
    color(BLACK, LIGHT_BLUE);
    cout << ": good apple" << endl << endl;
    
    display_apple = Apple(0, 0, 'b');
    draw_pixel(display_apple.face);
    cout << " ";
    color(BLACK, LIGHT_RED);
    cout << ": bad apple" << endl << endl;

    display_apple = Apple(0, 0, 's');
    draw_pixel(display_apple.face);
    cout << " ";
    color(BLACK, LIGHT_YELLOW);
    cout << ": special apple" << endl << endl;

    display_apple = Apple(0, 0, 't');
    draw_pixel(display_apple.face);
    cout << " ";
    color(BLACK, MAGENTA);
    cout << ": time apple" << endl << endl;

    display_apple = Apple(0, 0, 'm');
    draw_pixel(display_apple.face);
    cout << " ";
    color(BLACK, LIGHT_GRAY);
    cout << ": mystery apple" << endl << endl;

    color(16, 16);
    cout << "wasd/arrows to move" << endl;
    cout << "p to pause" << endl << endl;

    color(BLACK, CYAN);
    cout << "press enter to play" << endl;
    fflush(stdout);
}

void sigint_handler(int sig) {
    /* handles the ctrl+c signal */
    cout << "\n\n";
    
    #if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
    // linux/mac specific reset
    reset_termios();
    #endif

    color(16, 16);
    show_cursor(true);
    cout << "Exiting game..." << endl;
    clear_screen();
    exit(0);
}

int main(){
    signal(SIGINT, sigint_handler);

    #if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
    // linux/mac specific init
    init_termios();
    #endif

    bool keep_playing = true;
    while (keep_playing){
        //setup the screen
        clear_screen();
        show_cursor(false);

        //create a random seed
        srand(time(NULL));

        //create the player
        Player guy = Player(size_y/2, size_x/2, 
                            Pixel('P', LIGHT_GREEN, LIGHT_GREEN));
        //create the apple manager
        Apple_Manager apples = Apple_Manager();
        apples.spawn_apple(guy, 'g');
        int rand = rand_int(2);
        for (int i = 0; i < rand+4; i++)
            apples.spawn_apple(guy, 'b');

        //display the main menu
        display_main_menu();

        //wait until enter is pressed
        char key = -1;
        while (key != ENTER){
            //wait for the keyboard input
            key = wait_for_kb_input();
        }

        //clear the screen
        clear_screen();

        //draw the board with space for the header
        game.draw(2, false);

        //set the total play time allowed
        //get the string of the time allowed option
        int time_max_ms = 15000;
        //set highest score allowed
        int score_max = 100;

        //display the header
        display_header(0, 0, time_max_ms, 0);

        //wait until there is a key pressed...
        key = -1;
        while (key != 'w' && key != 'a' && key != 's' && key != 'd')
            key = convert_to_wasd(get_kb_input());




        //...and we're off to the races!
        //get the start time and current time (which is start time right now)
        short time_change = 0;
        short score_change = 0;
        int spent_time_ms = 0;
        int time_slow_ms = 0;

        //current_time - start_time = time elapsed, while that's < time_ms, do:
        while (spent_time_ms < time_max_ms && guy.length < score_max 
                && guy.length >= 0){
            key = convert_to_wasd(key);
            if (key == 'p'){
                //pausing still increments the timer, fix this later
                key = -1;
                while (key != 'p')
                    key = wait_for_kb_input();
            }

            //move the guy based on key input
            guy.move(key);

            //if the guy is on an apple
            if (apples.on_apple(guy.row, guy.col)){
                Apple apple = apples.get_apple_at(guy.row, guy.col);
                cout << "       ";
                draw_pixel(apple.face);
                switch (apple.kind){
                    case 'g':
                        time_change = 500;
                        score_change = 1;
                        break;

                    case 'b':
                        time_change = -500;
                        score_change = -1;
                        break;

                    case 's':
                        time_change = 2500;
                        score_change = 5;
                        break;

                    case 'm':
                        {
                            rand = rand_int(1);
                            if (rand){
                                time_change = 2500;
                                score_change = 5;
                            }
                            else {
                                time_change = -1500;
                                score_change = -3;
                            }
                            break;
                        }

                    case 't':
                        score_change = 0;
                        time_change = 0;
                        time_slow_ms += 5000;
                        break;

                }
                //add the time from the apple to total time
                time_max_ms += time_change;
                //eat the apple
                guy.length += score_change;
                if (apple.kind == 'b'){
                    //if you got a bad apple, remove just that one
                    apples.remove_apple(apple);
                }
                else{
                    //otherwise remove all apples from the list
                    apples.clear_apples();

                    //spawn apples
                    apples.spawn_apple(guy, 'g');

                    rand = rand_int(2);
                    for (int i = 0; i < rand+4; i++)
                        apples.spawn_apple(guy, 'b');

                    //spawn special apples
                    if (rand_int(19) == 19)
                        apples.spawn_apple(guy, 's');
                    if (rand_int(9) == 9)
                        apples.spawn_apple(guy, 'm');
                    if (rand_int(19) == 19)
                        apples.spawn_apple(guy, 't');
                }
            }

            //draw the board now that all calculations are done
            game.draw(2, false);

            //display the header with current score and time information
            display_header(guy.length, score_change, time_max_ms-spent_time_ms,
                                                                  time_change);

            //get the keyboard input if there is one right now
            key = get_kb_input();

            //wait 16 ms because it gives the printing some time to catch up
            //just in case
            delay(16);
            if (time_slow_ms > 0){
                spent_time_ms += 8;
                time_slow_ms -= 16;
            } else {
                spent_time_ms += 16;
                time_slow_ms = 0;
            }
        }
        //make the character purple and wait a second as a death animation
        game.write(guy.row, guy.col, Pixel('d', PURPLE, PURPLE));
        game.draw(2, 0);
        fflush(stdout);
        delay(1500);

        //clear the screen
        clear_screen();
        //clear the board and redraw it fully next time we draw it
        game.clear_board(true);
        //reset color just in case
        color(16, 16);
        
        //display game over or game won message
        delay(500);
        if (guy.length >= score_max){
            color(GRAY, LIGHT_BLUE);
            cout << "GAME WON!";
            color(16, 16);
            cout << endl;
        }
        else {
            color(GRAY, LIGHT_RED);
            cout << "GAME OVER";
            color(16, 16);
            cout << endl;
        }

        //display final score and time information
        delay(500);
        cout << "You got " ;
        color(BLACK, LIGHT_BLUE);
        cout << guy.length;
        color(16, 16);
        cout << " apples" << endl;

        delay(500);
        cout << "In ";
        color(BLACK, LIGHT_BLUE);
        cout << (float) spent_time_ms/1000;
        color(16, 16);
        cout << " seconds" << endl << endl;delay(500);

        //wait for e/q press to quit/play again
        color(BLACK, LIGHT_RED);
        cout << "Press q to quit\n";
        color(BLACK, LIGHT_BLUE);
        cout << "Press e to play again";
        color(16, 16);
        do {
            key = wait_for_kb_input();
        } while(key != 'q' && key != 'e');

        //depending on press, assign keep_playing accordingly
        if (key == 'q')
            keep_playing = false;
        else
            keep_playing = true;
    }

    sigint_handler(0);
}
