#ifndef PONG_H
#define PONG_H

#define PLAYER1_UP_PRESSED 'w'
#define PLAYER1_UP2_PRESSED 'W'
#define PLAYER1_DOWN_PRESSED 's'
#define PLAYER1_DOWN2_PRESSED 'S'

#define PLAYER2_UP_PRESSED 17
#define PLAYER2_DOWN_PRESSED 20

#define PLAYER1_UP_RELEASED 17 + 0x80
#define PLAYER1_DOWN_RELEASED 31 + 0x80 

#define PLAYER2_UP_RELEASED 72 + 0x80 
#define PLAYER2_DOWN_RELEASED 80 + 0x80 

#define PLAYER_WIDTH 15
#define PLAYER_HEIGHT 150

#define PLAYER1_MOVE_AMOUNT 30
#define PLAYER2_MOVE_AMOUNT 30

typedef struct Player {
    int x;
    int y;
    int score;
} Player;

void pong();

void drawBall(int x, int y, int radius, int color);
void drawPaddle(int x, int y, int width, int height, int color);
void drawScore(int score, int color);
void win(int color, int player);
void pong();
void moveBall(int * ballX, int * ballY, int ballRadius);
short tick() ;
void handleKey(unsigned char key, char moves[]) ;
void handleMovement(char moves[2]) ;

#endif