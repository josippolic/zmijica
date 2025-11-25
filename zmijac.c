// SNAKE – dvostruke linije, score 0000, ime, lives, vrijeme, zvukovi, PAUSE (P)
// + zapis u .txt (razlog smrti: zid / sam sebe)

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>      // _kbhit(), _getch()
#include <windows.h>    // Sleep(), Beep(), gotoxy()
#include <time.h>       // time()
#include <string.h>     // fgets(), strcspn()

// Dimenzije igrališta
#define WIDTH 40
#define HEIGHT 20

// Struktura za poziciju x,y
typedef struct {
    int x, y;
} Point;

// Polje elemenata zmije
Point snake[200];
int snakeLength = 3;

// Smjer kretanja zmije
int dx = 1, dy = 0;

// Hrana
Point food;

// Statistike
int score = 0;
int delay = 150;
int lives = 3;

// Pauza igre
int paused = 0;

// Početno vrijeme igre
time_t startTime;

// Ime igrača
char playerName[100];

//-------------------------------------------------
// Pozicioniranje kursora u konzoli
//-------------------------------------------------
void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

//-------------------------------------------------
// Reset zmije nakon gubitka života
//-------------------------------------------------
void resetSnake() {
    snakeLength = 3;

    // Glava na sredini
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;

    // Tijelo iza glave
    snake[1].x = snake[0].x - 1;
    snake[1].y = snake[0].y;

    snake[2].x = snake[0].x - 2;
    snake[2].y = snake[0].y;

    // Reset smjera
    dx = 1;
    dy = 0;
}

//-------------------------------------------------
// Nasumična pozicija hrane
//-------------------------------------------------
void placeFood() {
    while (1) {
        int ok = 1;

        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;

        // Ne smije biti na zmiji
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                ok = 0;
                break;
            }
        }
        if (ok) return;
    }
}

//-------------------------------------------------
// Crtanje gornjeg ruba
//-------------------------------------------------
void drawBorderTop() {
    putchar(201);                 // ╔
    for (int i = 0; i < WIDTH; i++) putchar(205);   // ═
    putchar(187);                 // ╗
    putchar('\n');
}

//-------------------------------------------------
// Crtanje donjeg ruba
//-------------------------------------------------
void drawBorderBottom() {
    putchar(200);                 // ╚
    for (int i = 0; i < WIDTH; i++) putchar(205);   // ═
    putchar(188);                 // ╝
    putchar('\n');
}

//-------------------------------------------------
// Crtanje kompletnog igrališta
//-------------------------------------------------
void draw() {
    // Reset ekrana na vrh
    gotoxy(0, 0);

    // Statusna linija
    printf("SNAKE GAME  | Score: %04d | Lives: %d | Delay: %dms",
        score, lives, delay);

    if (paused) printf(" | PAUSED");
    printf("\n");

    drawBorderTop();

    // Crtanje svih polja
    for (int y = 0; y < HEIGHT; y++) {
        putchar(186); // lijevi rub

        for (int x = 0; x < WIDTH; x++) {

            if (x == food.x && y == food.y)
                putchar('*');          // hrana

            else if (snake[0].x == x && snake[0].y == y)
                putchar('O');          // glava

            else {
                int printed = 0;

                // provjera tijela
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

        putchar(186); // desni rub
        putchar('\n');
    }

    drawBorderBottom();
}

//-------------------------------------------------
// Tipke i upravljanje
//-------------------------------------------------
void input() {
    if (_kbhit()) {
        char c = _getch();

        // Obrada strelica
        if (c == -32) {
            c = _getch();

            if (!paused) {
                if (c == 72 && dy == 0) { dx = 0; dy = -1; } // gore
                if (c == 80 && dy == 0) { dx = 0; dy = 1; }  // dolje
                if (c == 75 && dx == 0) { dx = -1; dy = 0; } // lijevo
                if (c == 77 && dx == 0) { dx = 1; dy = 0; }  // desno
            }
            return;
        }

        // WASD alternativa
        switch (c) {
        case 'w': if (!paused && dy == 0) { dx = 0; dy = -1; } break;
        case 's': if (!paused && dy == 0) { dx = 0; dy = 1; } break;
        case 'a': if (!paused && dx == 0) { dx = -1; dy = 0; } break;
        case 'd': if (!paused && dx == 0) { dx = 1; dy = 0; } break;

        case 'q': exit(0);

            // pauza
        case 'p':
        case 'P':
            paused = !paused;
            if (paused) Beep(700, 150); else Beep(500, 150);
            break;
        }
    }
}

//-------------------------------------------------
// Zapis rezultata u tekstualnu datoteku
//-------------------------------------------------
void saveScore(const char* reason) {
    FILE* f = fopen("C:\\Users\\Dark Prince\\source\\repos\\zmija15\\x64\\Debug\\score.txt", "a");
    if (!f) return;

    int t = (int)(time(NULL) - startTime);

    fprintf(f, "%s | Score: %04d | Time: %d sec | Lives left: %d | Reason: %s\n",
        playerName, score, t, lives, reason);

    fclose(f);
}

//-------------------------------------------------
// Logika kretanja, sudara i jedenja
//-------------------------------------------------
void update() {

    // Pomicanje tijela zmije
    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    // Pomak glave
    snake[0].x += dx;
    snake[0].y += dy;

    //————————— SUDAR SA ZIDOM —————————
    if (snake[0].x < 0 || snake[0].x >= WIDTH ||
        snake[0].y < 0 || snake[0].y >= HEIGHT)
    {
        Beep(300, 200);
        lives--;

        if (lives <= 0) {
            gotoxy(0, HEIGHT + 5);
            printf("Gotova igra!Pokusaj ponovno! Score: %04d\n", score);
            saveScore("Wall collision");
            exit(0);
        }

        resetSnake();
        return;
    }

    //————————— SUDAR SA SAMIM SOBOM —————————
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            Beep(200, 200);

            gotoxy(0, HEIGHT + 5);
            printf("Udario si sam sebe! Score: %04d\n", score);

            saveScore("Self collision");
            exit(0);
        }
    }

    //————————— HRANA —————————
    if (snake[0].x == food.x && snake[0].y == food.y) {
        Beep(600, 80);

        // pomicanje prema naprijed
        for (int i = snakeLength; i > 0; i--)
            snake[i] = snake[i - 1];

        // novi segment iza glave
        snake[1].x = snake[0].x - dx;
        snake[1].y = snake[0].y - dy;

        snakeLength++;
        score++;

        placeFood();
    }
}

//-------------------------------------------------
// MAIN – ulaz u program
//-------------------------------------------------
int main() {
    SetConsoleOutputCP(437); // dvostruke linije
    SetConsoleCP(437);

    system("cls");

    // Unos imena
    printf("Unesite ime i prezime: ");
    fgets(playerName, sizeof(playerName), stdin);
    playerName[strcspn(playerName, "\n")] = 0;

    srand(time(NULL));
    startTime = time(NULL);

    resetSnake();
    placeFood();

    // Sakrij kursor
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);

    // Glavna petlja
    while (1) {
        input();

        if (!paused) {
            update();
        }
        else {
            gotoxy(0, HEIGHT + 3);
            printf("=== PAUSED (P za nastavak) ===   ");
        }

        draw();
        Sleep(delay);
    }

    return 0;
}
