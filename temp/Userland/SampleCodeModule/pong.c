#include <usyscalls.h>
#include <colors.h>
#include <userio.h>
#include <pong.h>
#include <sounds.h>

// PLAYER_DISTANCE_FROM_BORDER
#define PDFB 25
// Player min height
#define P_MIN_H height/10
// Plyer max height
#define P_MAX_H 580

#define LINE_WIDTH 5
#define SPACE_TO_LINE 10
#define LINE_TOP height/12
#define LINE_BOTTOM (height-5)

uint16_t width;
uint16_t height;

Player player1;
Player player2;

int ballXDirection = 1;
int ballYDirection = -1;

int ballRadius = 10;
int ballSpeed = 10;

int goalMade = 0;

void setPlayer(int playerNumber, int x, int y, int score) {
    if (playerNumber == 1) {
        player1.x = x;
        player1.y = y;
        player1.score = score;
    } else if (playerNumber == 2) {
        player2.x = x;
        player2.y = y;
        player2.score = score;
    }
}

static int checkTopAndBottomBorder(int y, int j) {
    return y+j > LINE_TOP + LINE_WIDTH && y+j < LINE_BOTTOM;
}

static int checkLeftAndRightBorder(int x, int i) {
    return (x+i > SPACE_TO_LINE + LINE_WIDTH && x+i < width - SPACE_TO_LINE - LINE_WIDTH);
}

static char shouldDraw(int x, int y, int radius, int i, int j) {
    return 
        i*i + j*j <= radius*radius && // Only draw circle of the square
        // !(x+i >= PDFB && x+i <= PDFB + PLAYER_WIDTH) && 
        // !(x+i >= width - PDFB - PLAYER_WIDTH && x+i <= width - PDFB ) && 
        x+i != width / 2 && 
        checkTopAndBottomBorder(y, j) &&
        checkLeftAndRightBorder(x, i) && 
        !(x+i >= player1.x && x+i <= player1.x + PLAYER_WIDTH - 1 && y+j >= player1.y && y+j <= player1.y + PLAYER_HEIGHT) &&
        !(x+i >= player2.x && x+i <= player2.x + PLAYER_WIDTH - 1 && y+j >= player2.y && y+j <= player2.y + PLAYER_HEIGHT);
}

void drawBall(int x, int y, int radius, int color) {
    // TODO: Add logic to see if is in player
    int i, j;
    for (i = -radius; i <= radius; i++) {
        for (j = -radius; j <= radius; j++) {
            if (shouldDraw(x, y, radius, i, j)) {
                sys_draw_rectangle(x + i, y + j, 1, 1, color);
            }
        }
    }
}

void drawPaddle(int x, int y, int width, int height, int color) {
    int i, j;
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            sys_draw_rectangle(x + i, y + j, 1, 1, color);
        }
    }
}

void handleMovement(char moves[2]) {
    for (int i = 0; i < 2 ; i++) {
        if (moves[i] == PLAYER1_UP_PRESSED) {
            if (player1.y > P_MIN_H) {
                drawPaddle(player1.x, player1.y + PLAYER_HEIGHT - PLAYER1_MOVE_AMOUNT, PLAYER_WIDTH, PLAYER1_MOVE_AMOUNT, BLACK);
                player1.y -= PLAYER1_MOVE_AMOUNT;
                drawPaddle(player1.x, player1.y, PLAYER_WIDTH, PLAYER1_MOVE_AMOUNT, WHITE);
            }
        } else if (moves[i] == PLAYER1_DOWN_PRESSED) {
            if (player1.y < P_MAX_H) {
                drawPaddle(player1.x, player1.y, PLAYER_WIDTH, PLAYER1_MOVE_AMOUNT, BLACK);
                player1.y += PLAYER1_MOVE_AMOUNT;
                drawPaddle(player1.x, player1.y + PLAYER_HEIGHT - PLAYER1_MOVE_AMOUNT, PLAYER_WIDTH, PLAYER1_MOVE_AMOUNT, WHITE);
            }
        } else if (moves[i] == PLAYER2_UP_PRESSED) {
            if (player2.y > P_MIN_H) {
                drawPaddle(player2.x, player2.y + PLAYER_HEIGHT - PLAYER2_MOVE_AMOUNT, PLAYER_WIDTH, PLAYER2_MOVE_AMOUNT, BLACK);
                player2.y -= PLAYER2_MOVE_AMOUNT;
                drawPaddle(player2.x, player2.y, PLAYER_WIDTH, PLAYER2_MOVE_AMOUNT, WHITE);
            }
        } else if (moves[i] == PLAYER2_DOWN_PRESSED) {
            if (player2.y < P_MAX_H) {
                drawPaddle(player2.x, player2.y, PLAYER_WIDTH, PLAYER2_MOVE_AMOUNT, BLACK);
                player2.y += PLAYER2_MOVE_AMOUNT;
                drawPaddle(player2.x, player2.y + PLAYER_HEIGHT - PLAYER2_MOVE_AMOUNT, PLAYER_WIDTH, PLAYER2_MOVE_AMOUNT, WHITE);
            }
        }
    }
}

void printScore() {
    char score[] = {"Score: 0 - 0"};
    score[7] = player1.score + '0';
    score[11] = player2.score + '0';
    sys_draw_rectangle(455, 15, 13*8, 20, BLACK);
    sys_write_place(STDOUT, score, 13, 455, 15);
}

void handleGoal(int playerNumber, Player * player) {
    char text[] = {"Player 0 scored!!"};
    text[7] = playerNumber + '0';
    sys_draw_rectangle(440, 35, 18*8, 20, BLACK);
    sys_write_place(STDOUT, text, 17, 440, 35);

    goalSound();
    if (playerNumber == 1 ) {
        player->score++;
        if (player->score == 3) {
            win(WHITE, 1);
        }
    } else if (playerNumber == 2) {
        player->score++;
        if (player->score == 3) {
            win(WHITE, 2);
        }
    }
    goalMade = 1;
}

void moveBall(int * ballX, int * ballY, int ballRadius) {
    // Erase ball, but not players or middle line
    drawBall(*ballX, *ballY, ballRadius, BLACK);

    // Move ball
    *ballX += ballXDirection * ballSpeed;
    *ballY += ballYDirection * ballSpeed;

    // Check if ball is out of bounds
    if (*ballX < 0) {
        handleGoal(2, &player2);
        printScore();
    } else if (*ballX > width) {
        handleGoal(1, &player1);
        printScore();
    } else {
        if (*ballY < P_MIN_H) {
            ballYDirection = 1;
            wallSound();
        } else if (*ballY > LINE_BOTTOM) {
            ballYDirection = -1;
            wallSound();
        }
        // Draw ball
        drawBall(*ballX, *ballY, ballRadius, WHITE);
    }
}

void win(int color, int player) {
    sys_clear_screen();
    char * winner = "Player 1 wins!";
    if (player == 2) {
        winner = "Player 2 wins!";
    }

    while (getChar() != 0) {
        ; // Cleans buffer
    }

    sys_write_color(STDOUT, winner, 14, color);
    sys_write_color(STDOUT, "\n\nPress R to play again or any key to continue...", 49, color);
    
    char c;
    while ((c = getChar()) == 0) {
        ;
    }

    sys_toggle_cursor();

    if (c == 'r' || c == 'R') {
        pong();
        return;
    }
    goalMade = 1;
}

short tick() {
    static uint32_t lastTick = 0;
    uint32_t currentTicks;
    sys_get_ticks(&currentTicks);
    if (currentTicks != lastTick) {
        lastTick = currentTicks;
        return 1;
    }
    return 0;
}

void handleKey(unsigned char key, char moves[]) {
    if (key == PLAYER1_UP_PRESSED || key == PLAYER1_UP2_PRESSED) {
        moves[0] = PLAYER1_UP_PRESSED;
    } else if (key == PLAYER1_UP_RELEASED) {
        moves[0] = 0;
    } else if (key == PLAYER1_DOWN_PRESSED || key == PLAYER1_DOWN2_PRESSED) {
        moves[0] = PLAYER1_DOWN_PRESSED;
    } else if (key == PLAYER1_DOWN_RELEASED) {
        moves[0] = 0;
    } else if (key == PLAYER2_UP_PRESSED) {
        moves[1] = PLAYER2_UP_PRESSED;
    } else if (key == PLAYER2_UP_RELEASED) {
        moves[1] = 0;
    } else if (key == PLAYER2_DOWN_PRESSED) {
        moves[1] = PLAYER2_DOWN_PRESSED;
    } else if (key == PLAYER2_DOWN_RELEASED) {
        moves[1] = 0;
    }
}

void pong() {
    sys_clear_screen();
    sys_toggle_cursor();

    sys_get_screen_size(&width, &height);

    // Draw middle line, left and right line
    for (int i = LINE_TOP; i < LINE_BOTTOM; i++) {
        sys_draw_rectangle(SPACE_TO_LINE, i, LINE_WIDTH, LINE_WIDTH, WHITE);
        sys_draw_rectangle(width-LINE_WIDTH-SPACE_TO_LINE, i, LINE_WIDTH, LINE_WIDTH, WHITE);
        if (i % 10 == 0) {
            sys_draw_rectangle(width / 2, i, 1, LINE_WIDTH, WHITE);
        }
    }

    //Draw Top line and Bottom line
    for (int i = 10; i < width-10; i++) {
        sys_draw_rectangle(i, LINE_BOTTOM, 1, LINE_WIDTH, WHITE);
        sys_draw_rectangle(i, LINE_TOP, 1, LINE_WIDTH, WHITE);
    }

    sys_write_color(STDOUT, "Player 1: W and S\nPlayer 2: Up and Down\n", 41, WHITE);
    sys_write_color(STDOUT, "Press any key to start...", 25, WHITE);
    sys_write_place(STDOUT, "Press P to pause/unpause the game", 33, 740, 15);
    sys_write_place(STDOUT, "Press ESC to exit", 33, 740, 35);
    while (getChar() == 0) {
        ;
    }

    setPlayer(1, PDFB, (height - PLAYER_HEIGHT) / 2, 0);
    setPlayer(2, width - PDFB - PLAYER_WIDTH, (height - PLAYER_HEIGHT) / 2, 0);

    // Draw ball
    int ballX = width / 2;
    int ballY = height / 2;
    drawBall(ballX, ballY, ballRadius, WHITE);

    // Draw player 1
    drawPaddle(player1.x, player1.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
    // Draw player 2
    drawPaddle(player2.x, player2.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);

    printScore();

    char moves[2] = { 0 };

    while (player1.score < 3 && player2.score < 3) {
        ballX = width / 2;
        ballY = height / 2;
        drawBall(ballX, ballY, ballRadius, WHITE);

        while (!goalMade) {
            unsigned char c;
            do {
                c = getChar();
                handleKey(c, moves);
            } while (c != 0 && c != 1 && c != 'p');

            if (tick()) {
                handleMovement(moves);
                moveBall(&ballX, &ballY, ballRadius);

                // Check if ball hit player
                if (ballX - ballRadius/2 <= player1.x + 2 * PLAYER_WIDTH && ballX >= player1.x && ballY >= player1.y && ballY <= player1.y + PLAYER_HEIGHT) {
                    ballXDirection = 1;
                    paddleHitSound();
                } else if (ballX + ballRadius/2 + PLAYER_WIDTH >= player2.x && ballX <= player2.x + PLAYER_WIDTH && ballY >= player2.y && ballY <= player2.y + PLAYER_HEIGHT) {
                    ballXDirection = -1;
                    paddleHitSound();
                }
            }

            // If ESC pressed, exit
            if (c == 0x01) {
                sys_toggle_cursor();
                sys_clear_screen();
                return;
            }
            // If P pressed, pause
            if (c == 'p') {
                sys_draw_rectangle(440, 35, 18*8, 20, BLACK);
                sys_write_place(STDOUT, "PAUSED", 6, 485, 35);
                while (getChar() != 'p') {
                    ;
                }
                sys_draw_rectangle(440, 35, 18*8, 20, BLACK);
            }
        }
        moves[0] = 0; moves[1] = 0;
        goalMade = 0;
    }
    sys_clear_screen();
}
