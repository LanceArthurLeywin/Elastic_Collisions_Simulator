#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std;

// Screen size
const int WIDTH = 80;
const int HEIGHT = 25;

// Time per frame in milliseconds
const int FRAME_DELAY = 50;

// Particle class
struct Particle {
    double x, y;       // Position
    double vx, vy;     // Velocity
    double radius;     // Radius
    double mass;       // Mass

    Particle(double _x, double _y, double _vx, double _vy, double _r, double _m)
        : x(_x), y(_y), vx(_vx), vy(_vy), radius(_r), mass(_m) {
    }

    void move() {
        x += vx;
        y += vy;

        // Wall collisions
        if (x - radius < 0) {
            x = radius;
            vx = -vx;
        }
        if (x + radius >= WIDTH) {
            x = WIDTH - radius - 1;
            vx = -vx;
        }
        if (y - radius < 0) {
            y = radius;
            vy = -vy;
        }
        if (y + radius >= HEIGHT) {
            y = HEIGHT - radius - 1;
            vy = -vy;
        }
    }
};

// Utility: distance squared
double dist2(const Particle& a, const Particle& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

// Elastic collision between two particles
void collide(Particle& a, Particle& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance == 0) return; // Avoid division by zero

    // Check if they are overlapping
    if (distance < a.radius + b.radius) {
        // Normal vector
        double nx = dx / distance;
        double ny = dy / distance;

        // Relative velocity
        double vx_rel = a.vx - b.vx;
        double vy_rel = a.vy - b.vy;

        // Velocity along the normal
        double vn = vx_rel * nx + vy_rel * ny;

        if (vn > 0) return; // Moving away

        // Impulse scalar
        double i = -(1 + 1.0) * vn / (1 / a.mass + 1 / b.mass);

        // Apply impulse
        a.vx += (i / a.mass) * nx;
        a.vy += (i / a.mass) * ny;
        b.vx -= (i / b.mass) * nx;
        b.vy -= (i / b.mass) * ny;

        // Separate particles to avoid sticking
        double overlap = 0.5 * (a.radius + b.radius - distance + 0.01);
        a.x -= overlap * nx;
        a.y -= overlap * ny;
        b.x += overlap * nx;
        b.y += overlap * ny;
    }
}

// Draw the particles on console
void draw(const vector<Particle>& particles) {
    vector<string> screen(HEIGHT, string(WIDTH, ' '));

    for (const auto& p : particles) {
        int px = static_cast<int>(p.x);
        int py = static_cast<int>(p.y);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            screen[py][px] = 'O';
        }
    }

    // Clear console
    cout << "\x1B[2J\x1B[H";
    for (const auto& line : screen) {
        cout << line << "\n";
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    vector<Particle> particles;

    // Create random particles
    for (int i = 0; i < 10; i++) {
        double r = 1;

        // Generate random integer positions within bounds
        int x_int = r + rand() % (WIDTH - static_cast<int>(2 * r));
        int y_int = r + rand() % (HEIGHT - static_cast<int>(2 * r));

        double x = static_cast<double>(x_int);
        double y = static_cast<double>(y_int);

        double vx = static_cast<double>((rand() % 3 + 1) * (rand() % 2 ? 1 : -1));
        double vy = static_cast<double>((rand() % 3 + 1) * (rand() % 2 ? 1 : -1));
        double m = 1;

        particles.emplace_back(x, y, vx, vy, r, m);
    }

    // Main loop
    while (true) {
        // Move particles
        for (auto& p : particles) p.move();

        // Check collisions
        for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                collide(particles[i], particles[j]);
            }
        }

        // Draw
        draw(particles);

        // Delay
        this_thread::sleep_for(chrono::milliseconds(FRAME_DELAY));
    }

    return 0;
}
