// moduli
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

#define WIDTH 40
#define HEIGHT 20  

typedef struct {
    int x, y;
} Point;

// Globalne varijable
Point snake[200];
int snakeLength = 3;
int dx = 1, dy = 0;
Point food;
int score = 0;
int delay = 150;
int lives = 3;

time_t startTime;
char playerName[100];

void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void resetSnake() {
    snakeLength = 3;

    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;

    snake[1].x = snake[0].x - 1;
    snake[1].y = snake[0].y;

    snake[2].x = snake[0].x - 2;
    snake[2].y = snake[0].y;

    dx = 1;
    dy = 0;
}

void placeFood() {
    while (1) {
        int valid = 1;
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;

        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                valid = 0;
                break;
            }
        }
        if (valid) return;
    }
}

void draw() {
    gotoxy(0, 0);

    printf("SNAKE GAME\n");
    printf("Score: %d   Lives: %d   Speed: %dms\n", score, lives, delay);

    for (int x = 0; x < WIDTH + 2; x++) printf("#");
    printf("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("#");
        for (int x = 0; x < WIDTH; x++) {
            if (x == food.x && y == food.y)
                printf("*");
            else if (snake[0].x == x && snake[0].y == y)
                printf("O");
            else {
                int printed = 0;
                for (int i = 1; i < snakeLength; i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        printf("o");
                        printed = 1;
                        break;
                    }
                }
                if (!printed) printf(" ");
            }
        }
        printf("#\n");
    }

    for (int x = 0; x < WIDTH + 2; x++) printf("#");
    printf("\n");
}

void input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'w': if (dy == 0) { dx = 0; dy = -1; } break;
        case 's': if (dy == 0) { dx = 0; dy = 1; } break;
        case 'a': if (dx == 0) { dx = -1; dy = 0; } break;
        case 'd': if (dx == 0) { dx = 1; dy = 0; } break;
        case 'q': exit(0);
        }
    }
}

void saveScore() {
    FILE* f = fopen("score.txt", "a");
    if (!f) return;

    int timeSpent = (int)(time(NULL) - startTime);

    fprintf(f, "%s | Score: %d | Vrijeme: %d sekundi\n",
        playerName, score, timeSpent);

    fclose(f);
}

void gameOver() {
    // 🎵 zvuk za game over
    Beep(300, 400);
    Beep(200, 400);
    Beep(150, 500);

    gotoxy(0, HEIGHT + 5);

    printf("Gotova igra! Score: %d\n", score);
    printf("Upisi ime i prezime: ");

    fflush(stdin);
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = 0;

    saveScore();
    exit(0);
}

void update() {
    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0].x += dx;
    snake[0].y += dy;

    // Sudar s rubom
    if (snake[0].x < 0 || snake[0].x >= WIDTH ||
        snake[0].y < 0 || snake[0].y >= HEIGHT)
    {
        // 🎵 zvuk udarca u zid
        Beep(200, 200);

        lives--;
        if (lives <= 0) {
            gameOver();
        }
        resetSnake();
        return;
    }

    // Sudar sa sobom
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {

            // 🎵 zvuk sudara sa samim sobom
            Beep(250, 300);
            Beep(180, 300);

            gameOver();
        }
    }

    // Hrana
    if (snake[0].x == food.x && snake[0].y == food.y) {

        // 🎵 zvuk kad pojede hranu
        Beep(700, 100);

        snakeLength++;
        score++;
        placeFood();
    }
}

int main() {
    srand(time(NULL));

    startTime = time(NULL);

    resetSnake();
    placeFood();

    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);

    while (1) {
        input();
        update();
        draw();
        Sleep(delay);
    }

    return 0;
}
