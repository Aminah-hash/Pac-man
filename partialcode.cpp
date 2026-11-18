#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <optional>
using namespace std;

const int rows = 21;
const int cols = 21;
const int tile_size = 28;
const int hud_height = 80;
const int window_width = cols * tile_size;
const int window_height = rows * tile_size + hud_height;

const char* originalMaze[rows] = {
    "#####################",
    "#.........#.........#",
    "#.###.###.#.###.###.#",
    "#o#...#...#...#...#o#",
    "#.#.#.#.#####.#.#.#.#",
    "#...#.........#...#.#",
    "###.#.###.###.#.###.#",
    "#...#...#.#...#.....#",
    "#.#####.#.#.#####.#.#",
    "#.......   .......#.#",
    "#.#####.###.#####.#.#",
    "#.....#.....#.....#.#",
    "###.#.#.###.#.###.#.#",
    "#...#...#...#...#...#",
    "#.###.###.#.###.###.#",
    "#o....#...#...#....o#",
    "#.##..#.#####.#..##.#",
    "#...... ..... ......#",
    "#.###.###.#.###.###.#",
    "#.........#.........#",
    "#####################"
};

char maze[rows][cols + 1];

struct Character {
    int row, col;
    int dr, dc;
    sf::Color color;
};

Character pacman;

int score = 0;
int foodleft = 0;

const char* getLetter(char ch) {
    switch (ch) {
    case 'A': return "01110""10001""10001""11111""10001""10001""10001";
    case 'C': return "01111""10000""10000""10000""10000""10000""01111";
    case 'E': return "11111""10000""10000""11110""10000""10000""11111";
    case 'H': return "10001""10001""10001""11111""10001""10001""10001";
    case 'I': return "11111""00100""00100""00100""00100""00100""11111";
    case 'O': return "01110""10001""10001""10001""10001""10001""01110";
    case 'R': return "11110""10001""10001""11110""10100""10010""10001";
    case 'S': return "01111""10000""10000""01110""00001""00001""11110";
    case ':': return "00000""00100""00100""00000""00100""00100""00000";
    case '0': return "01110""10001""10011""10101""11001""10001""01110";
    case '1': return "00100""01100""00100""00100""00100""00100""01110";
    case '2': return "01110""10001""00001""00010""00100""01000""11111";
    case '3': return "11110""00001""00001""01110""00001""00001""11110";
    case '4': return "00010""00110""01010""10010""11111""00010""00010";
    case '5': return "11111""10000""10000""11110""00001""00001""11110";
    case '6': return "01110""10000""10000""11110""10001""10001""01110";
    case '7': return "11111""00001""00010""00100""01000""01000""01000";
    case '8': return "01110""10001""10001""01110""10001""10001""01110";
    case '9': return "01110""10001""10001""01111""00001""00001""01110";
    default: return "00000""00000""00000""00000""00000""00000""00000";
    }
}

void drawText(sf::RenderWindow& window, string text,
    float x, float y, int size, sf::Color color) {

    for (int i = 0; i < text.length(); i++) {
        if (text[i] == ' ') {
            x += size * 4;
            continue;
        }

        const char* pattern = getLetter(text[i]);

        for (int r = 0; r < 7; r++) {
            for (int c = 0; c < 5; c++) {
                if (pattern[r * 5 + c] == '1') {
                    sf::RectangleShape pixel({
                        float(size), float(size)
                        });

                    pixel.setPosition({
                        x + c * size,
                        y + r * size
                        });

                    pixel.setFillColor(color);
                    window.draw(pixel);
                }
            }
        }

        x += size * 6;
    }
}

void resetMaze() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++)
            maze[r][c] = originalMaze[r][c];

        maze[r][cols] = '\0';
    }
}

void countFood() {
    foodleft = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (maze[r][c] == '.' || maze[r][c] == 'o')
                foodleft++;
        }
    }
}

bool isWall(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols)
        return true;

    return maze[row][col] == '#';
}

void resetGame() {
    resetMaze();

    score = 0;

    pacman = { 1, 1, 0, 1, sf::Color::Yellow };

    countFood();
}

void collectFood() {
    if (maze[pacman.row][pacman.col] == '.') {
        score += 10;
        maze[pacman.row][pacman.col] = ' ';
        foodleft--;
    }
    else if (maze[pacman.row][pacman.col] == 'o') {
        score += 50;
        maze[pacman.row][pacman.col] = ' ';
        foodleft--;
    }
}

void movePacman(int dr, int dc) {
    int newRow = pacman.row + dr;
    int newCol = pacman.col + dc;

    if (!isWall(newRow, newCol)) {
        pacman.row = newRow;
        pacman.col = newCol;

        pacman.dr = dr;
        pacman.dc = dc;

        collectFood();
    }
}

void drawMaze(sf::RenderWindow& window) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            float x = c * tile_size;
            float y = hud_height + r * tile_size;

            if (maze[r][c] == '#') {
                sf::RectangleShape wall({
                    float(tile_size - 2),
                    float(tile_size - 2)
                    });

                wall.setPosition({ x + 1, y + 1 });
                wall.setFillColor(sf::Color(20, 30, 180));
                window.draw(wall);
            }
            else if (maze[r][c] == '.' || maze[r][c] == 'o') {
                float radius = maze[r][c] == '.' ? 3.f : 7.f;

                sf::CircleShape pellet(radius);

                pellet.setPosition({
                    x + tile_size / 2.f - radius,
                    y + tile_size / 2.f - radius
                    });

                pellet.setFillColor(sf::Color::Yellow);
                window.draw(pellet);
            }
        }
    }
}

void drawPacman(sf::RenderWindow& window) {
    float x = pacman.col * tile_size + 3.f;
    float y = hud_height + pacman.row * tile_size + 3.f;

    sf::CircleShape player(tile_size / 2.f - 3.f, 100);
    player.setPosition({ x, y });
    player.setFillColor(sf::Color::Yellow);

    sf::ConvexShape mouth;
    mouth.setPointCount(3);

    if (pacman.dc == 1) {
        mouth.setPoint(0, { 11.f, 11.f });
        mouth.setPoint(1, { 23.f, 5.f });
        mouth.setPoint(2, { 23.f, 17.f });
    }
    else if (pacman.dc == -1) {
        mouth.setPoint(0, { 11.f, 11.f });
        mouth.setPoint(1, { 0.f, 5.f });
        mouth.setPoint(2, { 0.f, 17.f });
    }
    else if (pacman.dr == -1) {
        mouth.setPoint(0, { 11.f, 11.f });
        mouth.setPoint(1, { 5.f, 0.f });
        mouth.setPoint(2, { 17.f, 0.f });
    }
    else {
        mouth.setPoint(0, { 11.f, 11.f });
        mouth.setPoint(1, { 5.f, 23.f });
        mouth.setPoint(2, { 17.f, 23.f });
    }

    mouth.setPosition({ x, y });
    mouth.setFillColor(sf::Color::Black);

    sf::CircleShape eye(2.f);
    eye.setPosition({ x + 13.f, y + 3.f });
    eye.setFillColor(sf::Color::Black);

    window.draw(player);
    window.draw(mouth);
    window.draw(eye);
}

void drawHUD(sf::RenderWindow& window) {
    drawText(window, "SCORE", 15.f, 12.f, 3, sf::Color::White);
    drawText(window, to_string(score), 15.f, 43.f, 3, sf::Color::Yellow);
}

int main() {
    resetGame();

    sf::RenderWindow window(
        sf::VideoMode({ window_width, window_height }),
        "Pac-Man - SFML 3.1"
    );

    window.setFramerateLimit(60);

    sf::Clock pacmanClock;

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (pacmanClock.getElapsedTime().asMilliseconds() >= 150) {

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                movePacman(-1, 0);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                movePacman(1, 0);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
                movePacman(0, -1);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                movePacman(0, 1);

            pacmanClock.restart();
        }

        window.clear(sf::Color::Black);

        drawHUD(window);
        drawMaze(window);
        drawPacman(window);

        window.display();
    }

    return 0;
}
