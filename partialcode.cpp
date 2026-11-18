#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <optional>

using namespace std;

const int rows = 21;
const int cols = 21;
const int tile_size = 28;
const int hud_height = 80;
const int window_width = cols * tile_size;
const int window_height = rows * tile_size + hud_height;

char maze[rows][cols + 1] = {
    "#####################",
    "#.........#.........#",
    "#.###.###.#.###.###.#",
    "#o#...#...#...#...#o#",
    "#.#.#.#.#####.#.#.#.#",
    "#...#.........#...#.#",
    "###.#.###.###.#.###.#",
    "#...#...#.#...#.....#",
    "#.#####.#.#.#####.#.#",
    "#....... .......#.#",
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

struct Character {
    int row, col;
    int startRow, startCol;
    int dr, dc;
    sf::Color color;
};

Character pacman{ 1, 1, 1, 1, 0, 0, sf::Color::Yellow };
int score = 0;
int foodLeft = 0;
bool gameOver = false;
bool started = false;

void countFood() {
    foodLeft = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (maze[r][c] == '.' || maze[r][c] == 'o') foodLeft++;
        }
    }
}

bool isWall(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) return true;
    return maze[row][col] == '#';
}

void collectFood() {
    if (maze[pacman.row][pacman.col] == '.') {
        score += 10;
        maze[pacman.row][pacman.col] = ' ';
        foodLeft--;
    }
    else if (maze[pacman.row][pacman.col] == 'o') {
        score += 50;
        maze[pacman.row][pacman.col] = ' ';
        foodLeft--;
    }
}

void movePacman(int dr, int dc) {
    int newRow = pacman.row + dr;
    int newCol = pacman.col + dc;
    if (!isWall(newRow, newCol)) {
        pacman.row = newRow;
        pacman.col = newCol;
        collectFood();
    }
}

void drawMaze(sf::RenderWindow& window) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            float x = static_cast<float>(c * tile_size);
            float y = static_cast<float>(hud_height + r * tile_size);

            if (maze[r][c] == '#') {
                sf::RectangleShape wall({ float(tile_size - 2), float(tile_size - 2) });
                wall.setPosition({ x + 1.f, y + 1.f });
                wall.setFillColor(sf::Color(20, 30, 180));
                window.draw(wall);
            }
            else if (maze[r][c] == '.' || maze[r][c] == 'o') {
                float radius = (maze[r][c] == '.') ? 3.f : 7.f;
                sf::CircleShape pellet(radius);
                pellet.setPosition({ x + tile_size / 2.f - radius, y + tile_size / 2.f - radius });
                pellet.setFillColor(sf::Color::Yellow);
                window.draw(pellet);
            }
        }
    }
}

void drawCharacters(sf::RenderWindow& window) {
    sf::CircleShape player(tile_size / 2.f - 3.f);
    player.setPosition({ pacman.col * tile_size + 3.f, hud_height + pacman.row * tile_size + 3.f });
    player.setFillColor(pacman.color);
    window.draw(player);
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    countFood();

    sf::RenderWindow window(
        sf::VideoMode({ window_width, window_height }),
        "Pac-Man - SFML 3"
    );
    window.setFramerateLimit(60);

    sf::Clock moveClock;
    const float moveDelay = 0.14f; // Controls movement speed

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Handle input direction
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { pacman.dr = -1; pacman.dc = 0; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { pacman.dr = 1; pacman.dc = 0; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { pacman.dr = 0; pacman.dc = -1; }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { pacman.dr = 0; pacman.dc = 1; }

        // Move on a fixed timer so Pac-Man doesn't teleport
        if (moveClock.getElapsedTime().asSeconds() >= moveDelay) {
            if (pacman.dr != 0 || pacman.dc != 0) {
                movePacman(pacman.dr, pacman.dc);
            }
            moveClock.restart();
        }

        window.clear(sf::Color::Black);
        drawMaze(window);
        drawCharacters(window);
        window.display();
    }

    return 0;
}
