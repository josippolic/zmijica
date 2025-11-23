// SNAKE – dvostruke linije, score, ime, lives, vrijeme, zvukovi + PAUSE (P)

#include <stdio.h>      // standardni I/O
#include <stdlib.h>     // rand(), srand(), exit()
#include <conio.h>      // _kbhit(), _getch() – čitanje tipkovnice bez Entera
#include <windows.h>    // Sleep(), Beep(), gotoxy()
#include <time.h>       // time()
#include <string.h>     // fgets(), strcspn()

// Dimenzije igrališta
#define WIDTH 40
#define HEIGHT 20

// Struktura za x,y poziciju
typedef struct {
    int x, y;
} Point;

// Zmija može imati max 200 segmenata
Point snake[200];
int snakeLength = 3;

// smjer kretanja (dx, dy)
int dx = 1, dy = 0;

// Pozicija hrane
Point food;

// Statistike
int score = 0;
int delay = 150;     // brzina igre
int lives = 3;

// Pauza
int paused = 0;

// vrijeme početka igre
time_t startTime;

// ime igrača
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
// Reset pozicije zmije nakon gubitka života
//-------------------------------------------------
void resetSnake() {
    snakeLength = 3;

    snake[0].x = WIDTH / 2;      // glava u sredini
    snake[0].y = HEIGHT / 2;

    // tijelo iza glave
    snake[1].x = snake[0].x - 1;
    snake[1].y = snake[0].y;

    snake[2].x = snake[0].x - 2;
    snake[2].y = snake[0].y;

    // smjer kretanja reset
    dx = 1;
    dy = 0;
}

//-------------------------------------------------
// Nasumično postavljanje hrane
//-------------------------------------------------
void placeFood() {
    while (1) {
        int ok = 1;

        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;

        // provjera da nije na zmiji
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
// Crtanje gornjeg ruba (CP437 – dvostruke linije)
//-------------------------------------------------
void drawBorderTop() {
    putchar(201); // ╔
    for (int i = 0; i < WIDTH; i++) putchar(205); // ═
    putchar(187); // ╗
    putchar('\n');
}

//-------------------------------------------------
void drawBorderBottom() {
    putchar(200); // ╚
    for (int i = 0; i < WIDTH; i++) putchar(205); // ═
    putchar(188); // ╝
    putchar('\n');
}

//-------------------------------------------------
// Glavna funkcija za crtanje igre
//-------------------------------------------------
void draw() {
    gotoxy(0, 0);

    // statusna traka
    printf("SNAKE GAME  | Score: %d | Lives: %d | Delay: %dms",
        score, lives, delay);

    if (paused) printf(" | PAUSED");
    printf("\n");

    // gornji rub
    drawBorderTop();

    // glavno igralište
    for (int y = 0; y < HEIGHT; y++) {
        putchar(186); // ║ lijeva granica

        for (int x = 0; x < WIDTH; x++) {

            if (x == food.x && y == food.y)
                putchar('*');            // hrana

            else if (snake[0].x == x && snake[0].y == y)
                putchar('O');            // glava zmije

            else {
                // provjera tijela zmije
                int printed = 0;
                for (int i = 1; i < snakeLength; i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        putchar('o');    // tijelo
                        printed = 1;
                        break;
                    }
                }
                if (!printed) putchar(' '); // prazno polje
            }
        }

        putchar(186); // ║ desna granica
        putchar('\n');
    }

    // donji rub
    drawBorderBottom();
}

//-------------------------------------------------
// Obrada tipkovnice
//-------------------------------------------------
void input() {
    if (_kbhit()) {
        char c = _getch();

        // Obrada strelica (Windows šalje -32, pa drugi kod)
        if (c == -32) {
            c = _getch();
            if (!paused) { // blokiraj kretanje kad je pauza
                if (c == 72 && dy == 0) { dx = 0; dy = -1; } // ↑
                if (c == 80 && dy == 0) { dx = 0; dy = 1; }  // ↓
                if (c == 75 && dx == 0) { dx = -1; dy = 0; } // ←
                if (c == 77 && dx == 0) { dx = 1; dy = 0; }  // → 
            }
            return;
        }

        // WASD alternative
        switch (c) {
        case 'w': if (!paused && dy == 0) { dx = 0; dy = -1; } break;
        case 's': if (!paused && dy == 0) { dx = 0; dy = 1; } break;
        case 'a': if (!paused && dx == 0) { dx = -1; dy = 0; } break;
        case 'd': if (!paused && dx == 0) { dx = 1; dy = 0; } break;

        case 'q': exit(0);  // izlaz

            // pauza (P)
        case 'p':
        case 'P':
            paused = !paused;
            if (paused) Beep(700, 150);
            else Beep(500, 150);
            break;
        }
    }
}

//-------------------------------------------------
// Spremanje rezultata u .txt datoteku
//-------------------------------------------------
void saveScore() {
    FILE* f = fopen("C:\\Users\\Dark Prince\\source\\repos\\zmija14\\x64\\Debug\\score.txt", "a");
    if (!f) return;

    int t = (int)(time(NULL) - startTime);

    fprintf(f, "%s | Score: %d | Time: %d sec | Lives left: %d\n",
        playerName, score, t, lives);

    fclose(f);
}

//-------------------------------------------------
// Logika igre (sudar, pomicanje, hrana...)
//-------------------------------------------------
void update() {

    // Pomak tijela (od repa prema glavi)
    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    // Pomak glave
    snake[0].x += dx;
    snake[0].y += dy;

    // Sudar s rubom
    if (snake[0].x < 0 || snake[0].x >= WIDTH ||
        snake[0].y < 0 || snake[0].y >= HEIGHT)
    {
        Beep(300, 200);   // zvuk smrti
        lives--;

        if (lives <= 0) {
            gotoxy(0, HEIGHT + 5);
            printf("Gotova igra! Pokusaj ponovno! Score: %d\n", score);
            saveScore();
            exit(0);
        }

        resetSnake();  // izgubi život → reset zmije
        return;
    }

    // Sudar sa samim sobom
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            Beep(200, 200);

            gotoxy(0, HEIGHT + 5);
            printf("Gotova igra! Pokusaj ponovno! Score: %d\n", score);

            saveScore();
            exit(0);
        }
    }

    // Jelo hrane
    if (snake[0].x == food.x && snake[0].y == food.y) {
        Beep(600, 80); // zvuk

        // pomak svih dijelova naprijed
        for (int i = snakeLength; i > 0; i--)
            snake[i] = snake[i - 1];

        // novi segment smješta se iza glave
        snake[1].x = snake[0].x - dx;
        snake[1].y = snake[0].y - dy;

        snakeLength++;
        score++;

        placeFood();
    }
}

//-------------------------------------------------
// MAIN
//-------------------------------------------------
int main() {
    SetConsoleOutputCP(437); // postavi CP437 za dvostruke linije
    SetConsoleCP(437);

    system("cls");

    // Unos imena igrača
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

    // Glavna petlja igre
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
