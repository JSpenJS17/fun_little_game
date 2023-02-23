#include "engine.hpp"
#include <iostream>

using namespace std;

#define ENTER 13
#define UP 72
#define LEFT 75
#define DOWN 80
#define RIGHT 77

class Apple;
class Player;
class Apple_Manager;

unsigned int size_x = 13;
unsigned int size_y = 13;
unsigned short bg_color = LIGHT_GRAY;
const Pixel bg = Pixel('-', bg_color, bg_color);
Board game = Board(size_x, size_y, bg);

class Apple{
    public:
        unsigned int row;
        unsigned int col;
        char kind;
        Pixel face;

        Apple(const unsigned int init_row, const unsigned int init_col, 
              char variety = 'g'){
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
            row = init_row;
            col = init_col;
            face = disp_pixel;
            length = 0;
            game.write(row, col, face);
        }

        void move(char key){
            game.write(row, col);
            switch (key){
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

        void remove_apple(const unsigned int row, const unsigned int col){
            /* deletes an apple and removes it from the game board */
            Apple apple;
            for (unsigned int i = 0; i < apple_list.size(); i++){
                apple = apple_list.at(i);
                if (apple.row == row && apple.col == col)
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
        cout << "     " << endl;
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

    color(16, 16);
}

bool is_on_arrows(const char key){
    switch (key){
        case UP:
            return true;

        case DOWN:
            return true;

        case RIGHT:
            return true;

        case LEFT:
            return true;
    }
    return false;
}

char convert_to_wasd(const char key){
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
    color(LIGHT_BLUE, LIGHT_BLUE);
    for (int i = 0; i < (size_x*2-10)/2; i++)
        cout << "-";
    
    color(BLACK, LIGHT_BLUE);
    cout << "funny";
    
    color(BLACK, LIGHT_RED);
    cout << " game";
    
    color(LIGHT_RED, LIGHT_RED);
    for (int i = 0; i < (size_x*2-10)/2; i++)
        cout << "-";
    cout << endl << endl;

    Apple display_apple = Apple(0, 0, 'g');
    draw_pixel(display_apple.face);
    cout << ": Good Apple" << endl << endl;
    
    display_apple = Apple(0, 0, 'b');
    draw_pixel(display_apple.face);
    cout << ": Bad Apple" << endl << endl;

    display_apple = Apple(0, 0, 's');
    draw_pixel(display_apple.face);
    cout << ": Special Apple" << endl << endl;

    display_apple = Apple(0, 0, 't');
    draw_pixel(display_apple.face);
    cout << ": Time Apple" << endl << endl;

    display_apple = Apple(0, 0, 'm');
    draw_pixel(display_apple.face);
    cout << ": Mystery Apple" << endl << endl;

    cout << "Press enter to play";
    

}

int main(){
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
        apples.spawn_apple(guy, 'b');
        apples.spawn_apple(guy, 'b');

        //display the main menu
        display_main_menu();
        
        //wait until enter is pressed
        char key = -1;
        while (key != ENTER)
            key = wait_for_kb_input();

        //clear the screen
        clear_screen();

        //draw the board with space for the header
        game.draw(2, 0);

        //set the total play time allowed
        int time_max_ms = 15000;
        //set highest score allowed
        int score_max = 100;

        //display the header
        display_header(0, 0, time_max_ms, 0);

        //wait until there is a key pressed...
        key = -1;
        while (key != 'w' && key != 'a' && key != 's' && key != 'd')
            key = convert_to_wasd(wait_for_kb_input());



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
                switch (apple.kind){
                    case 'g':
                        time_change = 1000;
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
                            int rand = rand_int(1);
                            if (rand){
                                time_change = 2500;
                                score_change = 5;
                            }
                            else {
                                time_change = -2000;
                                score_change = -2;
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
                //remove the apple from the list
                apples.remove_apple(guy.row, guy.col);
                //clear all apples from the list
                apples.clear_apples();
                //spawn a couple of apples
                apples.spawn_apple(guy, 'g');
                apples.spawn_apple(guy, 'b');
                apples.spawn_apple(guy, 'b');
                if (rand_int(19) == 19)
                    apples.spawn_apple(guy, 's');
                if (rand_int(9) == 9)
                    apples.spawn_apple(guy, 'm');
                if (rand_int(19) == 19)
                    apples.spawn_apple(guy, 't');
            }

            //draw the board now that all calculations are done
            game.draw(2, 0);

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
            color(BLACK, LIGHT_BLUE);
            cout << "GAME WON!";
            color(16, 16);
            cout << endl;
        }
        else {
            color(BLACK, LIGHT_RED);
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
    

    return 0;
}

