#include <iostream>
#include <conio.h>
#include <vector>
#include <windows.h>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <string>

using namespace std;

// Model gry oraz gracza
struct Game {
    int size;
    int bestScore;
    vector<vector<string>> map;
};

struct Player {
    int isDead;
    int points;
    vector<vector<int>> bodyCoords;
};

const int ENTER = 13;
const string EMPTY = " ";
const string PLAYER = "O";
const string WALL = "#";
const string SCORE = "*";

// Zmienna czy gra już sie zakonczyla
bool isEnd = false;

// Wyswietlenie w ramce podanego napisu
void renderLabel(string title) {
    cout << (char)201 << string(title.length(), (char)205) << (char)187 << endl;
    cout << (char)186 << title << (char)186 << endl;
    cout << (char)200 << string(title.length(), (char)205) << (char)188 << endl;
}

void renderMenu(vector<string> options, string title, int &selected) {
    system("cls");

    unsigned int len = options.size();

    renderLabel(title);

    for (unsigned int i = 0; i < len; i++) {
        if (selected == i) {
            cout << options[i] << " " << (char)174 << endl;
        }
        else {
            cout << options[i] << endl;
        }
    }
}


// Zmiana wybranej opcji w menu
void changeSelected(int key, int& selected, int menuOptionsLength) {
    if (key == 80 && selected < menuOptionsLength - 1) {
        selected += 1;
    }
    else if (key == 72 && selected > 0) {
        selected -= 1;
    }
}

void exit() {
    system("cls");
    cout << "Thanks for game!" << endl;
}

void options(Game& game) {
    vector<string> menu = { "11x11","13x13","15x15","Back" };
    int sizes[3] = { 11,13,15 };
    int key{}, selected = 0;

    while (key != ENTER) {
        renderMenu(menu, "Options", selected);
        key = _getch();
        changeSelected(key, selected, 4);
    }

    if (menu[selected] != "Back") {
        game.size = sizes[selected];
    }
}

// Wyczyszczenie wszystkich pol w ktorych jest gracz
void clearPlayer(Game& game, Player& player) {
    isEnd = true;
    for (int y = 1; y < game.size + 1; y++) {
        for (int x = 1; x < game.size + 1; x++) {
            if (game.map[y][x] == EMPTY) {
                isEnd = false;
            }
            if (game.map[y][x] == PLAYER) {
                game.map[y][x] = EMPTY;
            }
        }
    }
}

void renderMap(Game& game, Player& player) {
    system("cls");

    string title = "Your score: " + to_string(player.points);
    renderLabel(title);

    clearPlayer(game, player);

    for (unsigned int y = 0; y < player.bodyCoords.size(); y++) {
        int playerY = player.bodyCoords[y][0];
        int playerX = player.bodyCoords[y][1];
        game.map[playerY][playerX] = PLAYER;
    }

    for (int y = 0; y < game.size + 2; y++) {
        for (int x = 0; x < game.size + 2; x++) {
            cout << game.map[y][x];
        }
        cout << endl;
    }
}

void createMap(Game& game, Player& player) {
    int playerStartingX = player.bodyCoords.back()[1];
    int playerStartingY = player.bodyCoords.back()[0];

    vector<vector<string>> map(game.size + 2);

    for (int i = 0; i < game.size + 2; i++) {
        map[i] = vector<string>(game.size + 2);
    }


    for (int y = 0; y < game.size + 2; y++) {
        for (int x = 0; x < game.size + 2; x++) {
            if (y == 0 || x == 0 || y == game.size + 1 || x == game.size + 1) {
                map[y][x] = WALL;
            }
            else if (y == playerStartingY && x == playerStartingX) {
                map[y][x] = PLAYER;
            }
            else {
                map[y][x] = EMPTY;
            }
        }
    }

    game.map = map;
}

// Tworzenie w losowych miejscach punktow
void renderPoints(Game& game, Player& player) {
    int pointX{}, pointY{};

    srand(time(NULL));

    while (!isEnd) {
        Sleep(5000);

        pointX = rand() % game.size + 1;
        pointY = rand() % game.size + 1;
        if (game.map[pointY][pointX] == EMPTY) {
            game.map[pointY][pointX] = SCORE;
            if (!isEnd) {
                renderMap(game, player);
            }
        }
    }
}

void end(Game& game, Player& player) {
    system("cls");
    renderLabel("Press any key to continue");
}

int moveKey{};

void movePlayer(Game& game, Player& player) {
    const int SPEED = 300; //ms
    const int KEY_UP = 72;
    const int KEY_DOWN = 80;
    const int KEY_LEFT = 75;
    const int KEY_RIGHT = 77;
    int moveX = 0;
    int moveY = 1;

    while (!isEnd) {
        Sleep(SPEED);

        if (moveKey == KEY_UP && moveY == 0) {
            moveY = -1;
            moveX = 0;
        }
        else if (moveKey == KEY_DOWN && moveY == 0) {
            moveY = 1;
            moveX = 0;
        }
        else if (moveKey == KEY_LEFT && moveX == 0) {
            moveX = -1;
            moveY = 0;
        }
        else if (moveKey == KEY_RIGHT && moveX == 0) {
            moveX = 1;
            moveY = 0;
        }

        // Zmiana pozycji gracza na podstawie kliknietych wyzej klawiszy
        int beginY = player.bodyCoords.back()[0];
        int beginX = player.bodyCoords.back()[1];
        if (game.map[beginY + moveY][beginX + moveX] == WALL || game.map[beginY + moveY][beginX + moveX] == PLAYER) {
            player.isDead = true;
            isEnd = true;
            end(game, player);
            break;
        }
        else if (game.map[beginY + moveY][beginX + moveX] == SCORE) {
            player.bodyCoords.push_back({ beginY + moveY, beginX + moveX });
            player.points += 1;
            if (player.points > game.bestScore) {
                game.bestScore = player.points;
            }
        }
        else {
            player.bodyCoords.push_back({ beginY + moveY, beginX + moveX });
            player.bodyCoords.erase(player.bodyCoords.begin());
        }

        renderMap(game, player);
    }
}

// Pokazanie ilosci punktow po przegranej grze oraz najlepszy wynik
bool endgame(Game& game, Player& player) {
    system("cls");

    vector<string> menu = { "Retry", "Back" };
    int key{}, selected = 0;

    string title{};
    if (player.isDead) {
        title = "You lose! Your score: " + to_string(player.points) + " Your highscore: " + to_string(game.bestScore);
    }
    else {
        title = "You won! Your score: " + to_string(player.points) + " Your highscore: " + to_string(game.bestScore);
    }

    while (key != ENTER) {
        renderMenu(menu, title, selected);
        key = _getch();
        changeSelected(key, selected, 2);
    }

    if (menu[selected] == "Retry") return true;
}

bool start(Game& game) {
    vector<int> startingPoint = { game.size / 2 + 1, game.size / 2 + 1 };
    Player newPlayer = { false, 0, {startingPoint} };

    createMap(game, newPlayer);
    renderMap(game, newPlayer);

    // Uzycie thread aby obie funkcjie dzialaly w tym samym momencie
    thread t_movePlayer(movePlayer, ref(game), ref(newPlayer));
    thread t_points(renderPoints, ref(game), ref(newPlayer));

    while (!isEnd) {
        moveKey = _getch();
    }

    t_movePlayer.join();
    t_points.join();

    bool isPlayAgain = endgame(game, newPlayer);
    return isPlayAgain;
}


int main()
{
    int defaultGameSize = 11;
    Game newGame = { defaultGameSize, 0, {{}} };

start:
    vector<string> menu = { "Start","Options","Exit" };
    int key{}, selected = 0;


    while (key != ENTER) {
        renderMenu(menu, "Snake", selected);
        key = _getch();
        changeSelected(key, selected, 3);
    }

    if (menu[selected] == "Start") {
    again:
        bool isPlayAgain = start(newGame);
        if (isPlayAgain) {
            goto again;
        }
        else {
            goto start;
        }
    }
    if (menu[selected] == "Options") {
        options(newGame);
        goto start;
    }
    if (menu[selected] == "Exit") {
        exit();
    }

    return -1;
}
