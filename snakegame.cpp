/*
link the ncurses library in command line.
$clang++ snakegame.cpp -lncurses -o a.out
$./a.out
*/

#include <iostream>
#include <ncurses.h>

using namespace std;

struct SnakeBod
{
    int x;
    int y;
    int old_x;
    int old_y;
    SnakeBod *prev;
    SnakeBod *next;
};
SnakeBod *snakeHead = NULL;
int snakeLength = 0;

bool isRunning = true;

const int Boundry_Width = 50;
const int Boundry_Height = Boundry_Width / 2;

const char Head = 'O';
const char Fruit = 'x';
const char Border = '#';
const char Blank = ' ';

char gameArr[Boundry_Height][Boundry_Width];
int fruitX, fruitY;
int score = 0;

enum movement{ Up = 0, Down, Left, Right};

movement curr_mov;

//Function declarations
void init();
void UserInput();
void DrawGame();
void GameLogic();
void AddTailToSnake();
void MoveAllOfSnake();
void EndGame();
void DrawHUD();
bool Collided();
//

void DrawHUD()
{
    char _score[15] = "Score: ";
    char _s[4] = "0";
    sprintf(_s, "%d", score);
    strcat(_score, _s);
    
    char _length[15] = "Length: ";
    char _l[4] = "0";
    sprintf(_l, "%d", snakeLength);
    strcat(_length, _l);
    
    mvaddstr(Boundry_Height + 2, 0, _score);
    mvaddstr(Boundry_Height + 3, 0, _length);
}

void DrawGame()
{
    for(int i = 0; i < Boundry_Height; i++)
        for(int j = 0; j < Boundry_Width; j++)
            gameArr[i][j] = Blank;
    
    snakeLength = 0;
    SnakeBod *temp = snakeHead;
    while(temp != NULL)
    {
        gameArr[temp->y][temp->x] = Head;
        temp = temp->next;
        ++snakeLength;
    }
    
    for(int i = 0; i < Boundry_Height; i++)
        for(int j = 0; j < Boundry_Width; j++)
        {
            if(i == 0 || j == 0 || i == Boundry_Height - 1 || j == Boundry_Width - 1)
                gameArr[i][j] = Border;
            else if(gameArr[i][j] == Head)
                gameArr[i][j] = Head;
            else if(i == fruitY && j == fruitX)
                gameArr[i][j] = Fruit;
            
            mvaddch(i, j, gameArr[i][j]);
        }
    
    DrawHUD();
    
    wrefresh(stdscr);
}

void AddTailToSnake(movement mov)
{
    SnakeBod *temp = snakeHead;
    while(temp->next != NULL)
        temp = temp->next;
    
    int tailX, tailY;
    
    switch (mov) {
        case Up:
            tailX = temp->x;
            tailY = temp->y + 1;
            break;
        case Down:
            tailX = temp->x;
            tailY = temp->y - 1;
            break;
        case Left:
            tailX = temp->x + 1;
            tailY = temp->y;
            break;
        case Right:
            tailX = temp->x - 1;
            tailY = temp->y;
            break;
        default:
            break;
    }
    
    SnakeBod *tail = new SnakeBod();
    tail->x = tailX;
    tail->y = tailY;
    tail->next = NULL;
    
    temp->next = tail;
    tail->prev = temp;
}

void MoveAllOfSnake(movement mov)
{
    snakeHead->old_x = snakeHead->x;
    snakeHead->old_y = snakeHead->y;
    
    switch (mov) {
        case Up:
            snakeHead->x = snakeHead->x;
            snakeHead->y = snakeHead->y - 1;
            break;
        case Down:
            snakeHead->x = snakeHead->x;
            snakeHead->y = snakeHead->y + 1;
            break;
        case Left:
            snakeHead->x = snakeHead->x - 1;
            snakeHead->y = snakeHead->y;
            break;
        case Right:
            snakeHead->x = snakeHead->x + 1;
            snakeHead->y = snakeHead->y;
            break;
        default:
            break;
    }
    
    SnakeBod *temp = snakeHead->next;
    while(temp != NULL)
    {
        temp->old_x = temp->x;
        temp->old_y = temp->y;
        temp->x = temp->prev->old_x;
        temp->y = temp->prev->old_y;
        temp = temp->next;
    }
}

bool Collided()
{
    int hX = snakeHead->x, hY = snakeHead->y;
    gameArr[hY][hX] = Blank;
    switch (curr_mov) {
        case Up:
            --hY;
            break;
        case Down:
            ++hY;
            break;
        case Left:
            --hX;
            break;
        case Right:
            ++hX;
            break;
        default:
            break;
    }
    
    if(hX == 0 || hX == Boundry_Width - 1 || hY == 0 || hY == Boundry_Height - 1)
    {
        return true;
    }
    else
    {
        SnakeBod *temp = snakeHead->next;
        while(temp != NULL)
        {
            if(temp->x == hX && temp->y == hY)
                return true;
            temp = temp->next;
        }
        return false;
    }
}

void GameLogic()
{
    int hX = snakeHead->x, hY = snakeHead->y;
    gameArr[hY][hX] = Blank;
    switch (curr_mov) {
        case Up:
            --hY;
            break;
        case Down:
            ++hY;
            break;
        case Left:
            --hX;
            break;
        case Right:
            ++hX;
            break;
        default:
            break;
    }
    
    if(Collided())
    {
        isRunning = false;
    }
    else
    {
        MoveAllOfSnake(curr_mov);
        
        //Eating Fruit
        if(hY == fruitY && hX == fruitX)
        {
            score++;
            AddTailToSnake(curr_mov);
            
            fruitX = rand() % (Boundry_Width - 2);
            if(fruitX == hX)
                fruitX = rand() % (Boundry_Width - 2);
            
            fruitY = rand() % (Boundry_Height - 2);
            if(fruitY == hY)
                fruitY = rand() % (Boundry_Height - 2);
            
            gameArr[fruitY][fruitX] = Fruit;
        }
    }
}

void UserInput()
{
    keypad(stdscr, true); //Can intercept special keys.
    halfdelay(1);
    
    int ch = getch();
    switch(ch)
    {
        case KEY_UP:
            curr_mov = Up;
            break;
        case KEY_DOWN:
            curr_mov = Down;
            break;
        case KEY_LEFT:
            curr_mov = Left;
            break;
        case KEY_RIGHT:
            curr_mov = Right;
            break;
        case 120: //ASCII for 'x'
            isRunning = false;
            break;
        default:
            break;
    }
}

void init()
{
    curr_mov = Left;
    
    snakeHead = new SnakeBod();
    
    snakeHead->x = Boundry_Width / 2;
    snakeHead->y = Boundry_Height / 2;
    snakeHead->old_x = snakeHead->x;
    snakeHead->old_y = snakeHead->y;
    
    fruitX = rand() % (Boundry_Width - 2);
    if(fruitX == snakeHead->x)
        fruitX = rand() % (Boundry_Width - 2);
    
    fruitY = rand() % (Boundry_Height - 2);
    if(fruitY == snakeHead->y)
        fruitY = rand() % (Boundry_Height - 2);
    
    initscr();
    clear();
    noecho(); //Doesn't display the cin key.
    cbreak(); //Disables terminal buffers.
}

void EndGame()
{
    delete snakeHead;
    endwin();
    cout << "Game Over !!!" << endl;
    cout << "Your Score: " << score << endl;
}

int main()
{
    init();
    
    while(isRunning)
    {
        DrawGame();
        UserInput();
        GameLogic();
    }
    
    EndGame();
    
    return 0;
}
