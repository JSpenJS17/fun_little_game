#include "engine.h"
#include <iostream>
#include <vector>

using namespace std;

#define ENTER 13

class Apple;
class Player;
class Apple_Manager;

unsigned int size_x = 11;
unsigned int size_y = 11;
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
                    face = Pixel('g', SKY_BLUE, SKY_BLUE);
                    break;
                case 'b':
                    face = Pixel('b', RED, RED);
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

        int is_on(const unsigned int o_row, const unsigned int o_col){
            return (row == o_row) && (col == o_col);
        }

        void eat(Apple apple){
            switch (apple.kind){
                case 'g':
                    length++;
                    break;
                case 'b':
                    length--;
                    break;
            }
        }
};

class Apple_Manager{
    public:
        void spawn_apple(Player guy, const char kind){
            /* spawns a new apple and writes it to the game board */
            unsigned int row;
            unsigned int col;
            do {
                row = rand_int(size_y-1);
                col = rand_int(size_x-1);
            } while (guy.is_on(row, col) || on_apple(row, col));

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

    if (score_change < 0){
        sprintf(str, "Score: %3d", score);
        cout << str;
        color(BLACK, RED);
        sprintf(str, " %+4d", score_change);
        cout << str << endl;
    }
    else if (score_change > 0){
        sprintf(str, "Score: %3d", score);
        cout << str;
        color(BLACK, SKY_BLUE);
        sprintf(str, " %+4d", score_change);
        cout << str << endl;
    } else {   
        sprintf(str, "Score: %3d", score);
        cout << str << endl;
    }

    color(16, 16);

    sprintf(str, "Time: %4.1f", (double) time_ms/1000);
    cout << str;

    if (time_change_ms < 0) {
        color(BLACK, RED);
        sprintf(str, " %+03.1f", (double) time_change_ms/1000);
        cout << str;
    }
    else if (time_change_ms > 0){
        color(BLACK, SKY_BLUE);
        sprintf(str, " %+03.1f", (double) time_change_ms/1000);
        cout << str;
    }

    color(16, 16);
    
}

int main(){
    //setup the screen
    clear_screen();
    cursor_off();

    //create a random seed
    srand(time(NULL));

    //create the player
    Player guy = Player(size_y/2, size_x/2, 
                        Pixel('P', LIGHT_GREEN, LIGHT_GREEN));
    //create the apple manager
    Apple_Manager apples = Apple_Manager();
    apples.spawn_apple(guy, 'g');
    apples.spawn_apple(guy, 'b');

    //wait a sec for everything to initialize
    delay(100);
    //draw the board with space for the header
    game.draw(2, 0);
    //wait another sec just in case
    delay(100);
    cout << endl << "WASD to move";

    //set the total play time allowed
    int time_ms = 15000;

    //display the header
    display_header(0, 0, time_ms, 0);

    //wait until there is a key pressed...
    char key = wait_for_kb_input();

    //...and we're off to the races!
    //get the start time and current time (which is start time right now)
    clock_t start_time = clock();
    clock_t current_time = start_time;
    short time_change = 0;
    short score_change = 0;
    
    //current_time - start_time = time elapsed, while that's < time_ms, do:
    while (current_time - start_time < time_ms){
        //move the guy based on key input
        guy.move(key);

        //if the guy is on an apple
        if (apples.on_apple(guy.row, guy.col)){
            Apple apple = apples.get_apple_at(guy.row, guy.col);
            //eat the apple
            guy.eat(apple);
            switch (apple.kind){
                case 'g':
                    time_change = 1000;
                    score_change = 1;
                    break;
                case 'b':
                    time_change = -500;
                    score_change = -1;
                    break;
            }
            time_ms += time_change;
            //remove the apple from the list
            apples.remove_apple(guy.row, guy.col);
            //remove the next apple which is the corresponding bad/good one
            apples.pop_apple();
            apples.spawn_apple(guy, 'g');
            apples.spawn_apple(guy, 'b');
        }

        //draw the board now that all calculations are done
        game.draw(2, 0);

        //grab the time
        current_time = clock();
        //display the header with current score and time information
        display_header(guy.length, score_change,
                       time_ms - (current_time - start_time), time_change);

        //get the keyboard input if there is one right now
        key = get_kb_input();

        //wait 16 ms because it gives the printing some time to catch up
        //just in case
        delay(16);
    }
    //clear the screen
    clear_screen();
    //reset color just in case
    color(16, 16);
    
    //display final score and time information
    delay(500);
    cout << "You got " ;
    color(BLACK, SKY_BLUE);
    cout << guy.length;
    color(16, 16);
    cout << " apples" << endl;

    delay(500);
    cout << "In ";
    color(BLACK, SKY_BLUE);
    cout << (float) time_ms/1000;
    color(16, 16);
    cout << " seconds" << endl << endl;
    delay(500);
    //wait for enter press to quit
    cout << "Press enter to quit...";
    do {
        key = get_kb_input();
    } while(key != ENTER);

    return 0;
}

