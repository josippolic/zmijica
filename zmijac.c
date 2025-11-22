// SNAKE – dvostruke linije, score, ime, lives, vrijeme, zvukovi

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

Point snake[200];
int snakeLength = 3;
int dx = 1, dy = 0;
Point food;
int score = 0;
int delay = 150;
int lives = 3;

time_t startTime;
char playerName[100];

// -----------------------------
void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
// -----------------------------

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
        int ok = 1;
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;

        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                ok = 0;
                break;
            }
        }
        if (ok) return;
    }
}

// -----------------------------
// CRTANJE RUBA CP437
// -----------------------------
void drawBorderTop() {
    putchar(201);             // ╔
    for (int i = 0; i < WIDTH; i++) putchar(205);  // ═
    putchar(187);             // ╗
    putchar('\n');
}

void drawBorderBottom() {
    putchar(200);             // ╚
    for (int i = 0; i < WIDTH; i++) putchar(205);  // ═
    putchar(188);             // ╝
    putchar('\n');
}
// -----------------------------

void draw() {
    gotoxy(0, 0);

    printf("SNAKE GAME  | Score: %d | Lives: %d | Delay: %dms\n",
        score, lives, delay);

    drawBorderTop();

    for (int y = 0; y < HEIGHT; y++) {
        putchar(186); // ║

        for (int x = 0; x < WIDTH; x++) {

            if (x == food.x && y == food.y)
                putchar('*');

            else if (snake[0].x == x && snake[0].y == y)
                putchar('O');

            else {
                int printed = 0;
                for (int i = 1; i < snakeLength; i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        putchar('o');
                        printed = 1;
                        break;
                    }
                }
                if (!printed) putchar(' ');
            }
        }

        putchar(186); // ║
        putchar('\n');
    }

    drawBorderBottom();
}

// -------------------------------------------------------

void input() {
    if (_kbhit()) {
        char c = _getch();

        // strelice
        if (c == -32) {
            c = _getch();
            if (c == 72 && dy == 0) { dx = 0; dy = -1; } // ↑
            if (c == 80 && dy == 0) { dx = 0; dy = 1; }  // ↓
            if (c == 75 && dx == 0) { dx = -1; dy = 0; } // ←
            if (c == 77 && dx == 0) { dx = 1; dy = 0; }  // →
            return;
        }

        // WASD
        switch (c) {
        case 'w': if (dy == 0) { dx = 0; dy = -1; } break;
        case 's': if (dy == 0) { dx = 0; dy = 1; } break;
        case 'a': if (dx == 0) { dx = -1; dy = 0; } break;
        case 'd': if (dx == 0) { dx = 1; dy = 0; } break;
        case 'q': exit(0);
        }
    }
}

// --------------------
// SPREMANJE SCOREA
// --------------------
void saveScore() {
    FILE* f = fopen("C:\\Users\\Dark Prince\\source\\repos\\zmija9\\x64\\Debug\\score.txt", "a");
    if (!f) return;

    int t = (int)(time(NULL) - startTime);

    fprintf(f, "%s | Score: %d | Time: %d sec | Lives left: %d\n",
        playerName, score, t, lives);

    fclose(f);
}

// -------------------------------------------------------

void update() {
    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    snake[0].x += dx;
    snake[0].y += dy;

    // rub
    if (snake[0].x < 0 || snake[0].x >= WIDTH ||
        snake[0].y < 0 || snake[0].y >= HEIGHT)
    {
        Beep(300, 200);
        lives--;

        if (lives <= 0) {
            gotoxy(0, HEIGHT + 5);
            printf("GAME OVER! Score: %d\n", score);
            saveScore();
            exit(0);
        }
        resetSnake();
        return;
    }

    // sudar sa sobom
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            Beep(200, 200);
            gotoxy(0, HEIGHT + 5);
            printf("GAME OVER! Score: %d\n", score);
            saveScore();
            exit(0);
        }
    }

    // hrana
    if (snake[0].x == food.x && snake[0].y == food.y) {
        Beep(600, 80);
        snakeLength++;
        score++;
        placeFood();
    }
}

// -------------------------------------------------------

int main() {
    SetConsoleOutputCP(437);
    SetConsoleCP(437);

    system("cls");

    printf("Unesite ime i prezime: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = 0;

    srand(time(NULL));
    startTime = time(NULL);

    resetSnake();
    placeFood();

    // sakrij kursor
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
