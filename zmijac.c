//moduli
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
int delay = 150; // konstantna brzina
int lives = 3;

char playerName[50];
time_t startTime;

// Pomicanje kursora
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
    gotoxy(0, 0);

    // 1. red: ime i prezime
    printf("Igrac: %s\n", playerName);

    // 2. red: score, lives, speed
    printf("Score: %d   Lives: %d   Speed: %dms\n", score, lives, delay);

    // Ostatak ekrana
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

// Spremanje rezultata
void saveScore() {
    FILE* f = fopen("score.txt", "a");
    if (!f) return;

    int timeSpent = (int)(time(NULL) - startTime);

    fprintf(f, "Igrac: %s | Score: %d | Lives: %d | Vrijeme: %d sekundi\n",
        playerName, score, lives, timeSpent);

    fclose(f);
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
            gotoxy(0, HEIGHT + 5);
            printf("Gotova igra!Pokus   aj ponovno! Score: %d\n", score);
            saveScore();
            exit(0);
        }
        resetSnake();
        return;
    }

    // Sudar sa sobom
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gotoxy(0, HEIGHT + 5);
            printf("Gotova igra! Pokusaj ponovno! Score: %d\n", score);
            saveScore();
            exit(0);
        }
    }

    // Hrana
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;
        score++;

        // uklonjena promjena delay da brzina ostane konstantna

        placeFood();
    }
}

int main() {

    srand(time(NULL));

    printf("Upisi ime i prezime: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = 0;

    // Popravak ghost-character bug Visual Studia
    for (int i = 0; playerName[i] != '\0'; i++) {
        if (playerName[i] < 32 || playerName[i] > 126) {
            playerName[i] = '\0';
            break;
        }
    }

    startTime = time(NULL);

    resetSnake();
    placeFood();

    // Sakrij kursor da izgleda bolje
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
