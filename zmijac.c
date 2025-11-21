#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

// Struktura točke
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

// Brisanje ekrana
void clearScreen() {
    printf("\033[2J\033[H");
}

// Postavljanje kursora
void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

// Reset zmije
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

// Generiranje hrane
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

// Crtanje
void draw() {
    clearScreen();

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

    printf("\033[0m"); // reset boja
}

// Logika tipki
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

// Logika igre
void update() {
    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0].x += dx;
    snake[0].y += dy;

    // Sudar s rubom
    if (snake[0].x < 0 || snake[0].x >= WIDTH ||
        snake[0].y < 0 || snake[0].y >= HEIGHT)
    {
        lives--;
        if (lives <= 0) {
            printf("GAME OVER! Score: %d\n", score);
            exit(0);
        }
        resetSnake();
        return;
    }

    // Sudar sa sobom
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            printf("GAME OVER! Score: %d\n", score);
            exit(0);
        }
    }

    // Hrana
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;
        score++;

        delay -= 5;
        if (delay < 40) delay = 40;

        placeFood();
    }
}

int main() {
    srand(time(NULL));

    resetSnake();
    placeFood();

    while (1) {
        input();
        update();
        draw();
        Sleep(delay);
    }

    return 0;
}
