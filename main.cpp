#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <random>
#include <stdint.h>
#include <unistd.h>
#include <chrono>
#include <vector>

#define WIDTH 90
#define HEIGHT 40
#define charSetLength 4
#define FrameRate 2000000
#define frameDelay 1000000 / FrameRate

std::string charSet[] = {
    "░", // Light shade (U+2591)
    "▒", // Medium shade (U+2592)
    "▓", // Dark shade (U+2593)
    "█"  // Full block (U+2588)
};

uint8_t inverseCharSet(std::string symbol)
{

    if(symbol == "░")
    {
        return 0;
    }
    else if(symbol == "▒")
    {
        return 0;
    }
    else if(symbol == "▓")
    {
        return 0;
    }
    else if(symbol == "█")
    {
        return 0;
    }
    else 
    {
        return 255;
    }
}

struct charColor
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    std::string symbol;
} typedef charColor;

charColor uint8_tToCharColor(uint8_t value)
{

    uint8_t rMask = 0b00000011;
    uint8_t gMask = 0b00001100;
    uint8_t bMask = 0b00110000;
    uint8_t symbolMask = 0b11000000;

    // find the red value
    uint8_t r = rMask & value;

    // find the red value
    uint8_t g = gMask & value;
    g >>= 2;

    // find the red value
    uint8_t b = bMask & value;
    b >>= 4;

    // find the red value
    uint8_t symbol = symbolMask & value;
    symbol >>= 6;

    return charColor{.r = r, .g = g, .b = b, .symbol = charSet[symbol]};
}

uint8_t charColorToUint8_t(charColor charColor)
{
    uint8_t value = (inverseCharSet(charColor.symbol) << 6) + (charColor.b << 4) + (charColor.g << 2) + charColor.r;
    return value;
}

struct Vec2
{
    float X;
    float Y;

    static float Distance(Vec2 a, Vec2 b)
    {
        float X = a.X - b.X;
        float Y = a.Y - b.Y;
        return sqrt(X * X + Y * Y);
    }

    static float DistanceSquared(Vec2 a, Vec2 b)
    {
        float X = a.X - b.X;
        float Y = a.Y - b.Y;
        return X * X + Y * Y;
    }

    Vec2 operator+(const Vec2 other)
    {
        return {this->X + other.X, this->Y + other.Y};
    }

    Vec2 operator-(const Vec2 other)
    {
        return {this->X - other.X, this->Y - other.Y};
    }

    Vec2 operator/(const float scalar)
    {
        return {this->X / scalar, this->Y / scalar};
    }

    Vec2 operator*(const float scalar)
    {
        return {this->X * scalar, this->Y * scalar};
    }

    Vec2 operator-()
    {
        return {-this->X, -this->Y};
    }

    Vec2 ceilDifference()
    {
        Vec2 ceil = {ceilf(this->X), ceilf(this->Y)};
        Vec2 copy = {this->X, this->Y};
        return copy - ceil;
    }

    void operator+=(Vec2 other)
    {
        this->X += other.X;
        this->Y += other.Y;
    }

    void operator-=(Vec2 other)
    {
        this->X -= other.X;
        this->Y -= other.Y;
    }

    void operator*=(float scalar)
    {
        this->X *= scalar;
        this->Y *= scalar;
    }

    void operator/=(float scalar)
    {
        this->X /= scalar;
        this->Y /= scalar;
    }

    Vec2 Normalize()
    {
        float length = this->Length();

        return {this->X / length, this->Y / length};
    }

    float Length()
    {
        return sqrt(this->X * this->X + this->Y * this->Y);
    }

    float LengthSquared()
    {
        return this->X * this->X + this->Y * this->Y;
    }
};

std::string formattedAsciixel(const charColor color)
{
    // Map 2-bit red, green, and blue to a 6x6x6 grid index (216 colors)
    // Scaling 0-3 (2-bit) to 0-5 (for the 6x6x6 grid)
    int red = color.r * 2;   // Map 0-3 -> 0-5
    int green = color.g * 2; // Map 0-3 -> 0-5
    int blue = color.b * 2;  // Map 0-3 -> 0-5

    // Calculate the 256-color index based on the 6x6x6 color cube
    int colorIndex = 16 + (red * 36) + (green * 6) + blue; // 216 colors + 16 base colors

    // Start ANSI escape sequence for 256-color mode
    std::string colorCode = "\033[38;5;";
    colorCode += std::to_string(colorIndex) + "m"; // Add the color index

    // Add the symbol character and reset the color
    std::string result = colorCode + color.symbol + "\033[0m";

    return result;
}

// Function to refresh the screen and print it
void refreshScreen(uint8_t buffer[])
{
    printf("\033[H\033[2J\033[H");
    std::string displayOutput = "";

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++) // Corrected the condition from 'i < WIDTH' to 'j < WIDTH'
        {
            int index = i * WIDTH + j;
            uint8_t value = buffer[index];
            charColor asciixel = uint8_tToCharColor(value);
            // printf("original value: %d\n", value);
            // printf("r: %d, g: %d, b: %d, symbol: %c\n", asciixel.r, asciixel.g, asciixel.b, asciixel.symbol);
            // printf("re-encoded value: %d\n", charColorToUint8_t(asciixel));
            displayOutput += formattedAsciixel(asciixel) + formattedAsciixel(asciixel);
            // printf("%s\n", formattedAsciixel(asciixel));
        }
        displayOutput += "\n"; // Newline after each row of characters
    }

    std::cout.flush() << displayOutput;
}

void clearBuffer(uint8_t buffer[], uint8_t value)
{

    for (int i = 0; i < HEIGHT * WIDTH; i++)
    {
        buffer[i] = value;
    }
}

void randomizeBuffer(uint8_t buffer[])
{

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++) // Corrected the condition from 'i < WIDTH' to 'j < WIDTH'
        {
            int index = i * WIDTH + j;
            buffer[index] = rand();
        }
    }
}

bool drawPixel(uint8_t buffer[], int X, int Y, uint8_t value)
{

    if (X >= WIDTH || X < 0 || Y >= HEIGHT || Y < 0)
    {
        return false;
    }

    buffer[Y * WIDTH + X] = value;
    return true;
}

bool drawAsciixel(uint8_t buffer[], int X, int Y, charColor value)
{

    if (X >= WIDTH || X < 0 || Y >= HEIGHT || Y < 0)
    {
        return false;
    }

    buffer[Y * WIDTH + X] = charColorToUint8_t(value);
    return true;
}

bool drawPixelV(uint8_t buffer[], Vec2 position, uint8_t value)
{
    int X = (int)position.X;
    int Y = (int)position.Y;
    if (X >= WIDTH || X < 0 || Y >= HEIGHT || Y < 0)
    {
        return false;
    }

    buffer[Y * WIDTH + X] = value;
    return true;
}

void drawLine(uint8_t buffer[], Vec2 startPos, Vec2 endPos, uint8_t value)
{
    Vec2 direction = endPos - startPos;
    direction = direction.Normalize();

    Vec2 position = startPos;

    float distance = startPos.Distance(startPos, endPos);

    for (int i = 0; i < distance; i++)
    {
        drawPixelV(buffer, position, value);
        position += direction;
    }
}

void drawCircleV(uint8_t buffer[], Vec2 center, uint8_t value, uint8_t radius)
{
    for (int i = 0; i < radius; i++)
    {
        drawPixelV(buffer, center, value);
        drawPixelV(buffer, center + Vec2{(float)i, 0}, value);
        drawPixelV(buffer, center + Vec2{0, (float)i}, value);
        drawPixelV(buffer, center + Vec2{(float)-i, 0}, value);
        drawPixelV(buffer, center + Vec2{0, (float)-i}, value);
    }
}

void drawRectangle(uint8_t buffer[], int X, int Y, int colWidth, int colHeight, uint8_t value)
{

    for (int i = 0; i < colWidth; i++)
    {
        for (int j = 0; j < colHeight; j++)
        {
            // if any pixel breaks the boundries, then the proceeding one will also break(assuming starting point is on the buffer)
            if (!drawPixel(buffer, X + i, Y + j, value))
            {
                // break;
            }
        }
    }
}

struct Particle
{
    Vec2 position_old;
    Vec2 position_new;
    Vec2 acceleration;
    float radius;

    Particle(Vec2 initial_position, Vec2 acceleration, float radius)
    {
        this->position_old = initial_position;
        this->position_new = initial_position;
        this->acceleration = acceleration;
        this->radius = radius;
    }

    void step(float dt)
    {
        Vec2 new_position = position_new * 2 - position_old + acceleration * dt * dt;

        this->position_old = this->position_new;
        this->position_new = new_position;
    }

    void pullBack(Vec2 center, float radius)
    {
        float distance = center.Distance(this->position_new, center);

        if (distance + this->radius > radius)
        {
            Vec2 direction = this->position_new - center;

            direction = direction.Normalize();

            float overstep = distance - radius;

            this->position_new -= direction * overstep;
        }
    }

    bool CheckCollision(Particle &other)
    {
        // Calculate the distance between the particles
        float dx = this->position_new.X - other.position_new.X;
        float dy = this->position_new.Y - other.position_new.Y;
        float distance_sq = dx * dx + dy * dy;

        // Calculate the sum of radii squared
        float sum_of_radius = this->radius + other.radius;
        float sum_of_radius_sq = sum_of_radius * sum_of_radius;

        // Check if particles are colliding
        if (distance_sq < sum_of_radius_sq)
        {
            // Compute the distance (needed for penetration calculation)
            float distance = std::sqrt(distance_sq);

            // Calculate the penetration depth
            float penetration = sum_of_radius - distance;

            // Compute the collision direction
            Vec2 direction = this->position_new - other.position_new;
            direction.Normalize();

            // Resolve the collision by moving both particles equally
            penetration /= 2.0f;
            this->position_new += direction * penetration;
            other.position_new -= direction * penetration;

            return true;
        }

        // No collision detected
        return false;
    }

    void draw(uint8_t buffer[], uint8_t value)
    {
        drawCircleV(buffer, this->position_new, value, (int)this->radius);
    }
} typedef Particle;

int main()
{
    int direction = 0;

    // Initialize the buffer with some values (for testing, we fill it with random values)
    uint8_t buffer[WIDTH * HEIGHT];

    std::vector<Particle> listOfParticles;
    Vec2 center = Vec2{WIDTH / 2, HEIGHT / 2};

    for (size_t i = 0; i < 10; i++)
    {
        listOfParticles.push_back(Particle(center + Vec2{(float)i, 0}, Vec2{0, 1}, 2));
    }

    auto previous_time = std::chrono::high_resolution_clock::now();
    float dt = 0.0f;

    while (true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float>(current_time - previous_time).count();
        previous_time = current_time;
        clearBuffer(buffer, 0);
        drawCircleV(buffer, center, 100, HEIGHT / 2);
        for (size_t i = 0; i < listOfParticles.size() - 1; i++)
        {
            for (size_t j = i + 1; j < listOfParticles.size() - 1; j++)
            {
                listOfParticles[i].CheckCollision(listOfParticles[j]);
            }
        }
        for (Particle &p : listOfParticles)
        {
            p.step(dt);
            p.pullBack(center, HEIGHT / 2);
        }
        int color = 100;
        for (Particle &p : listOfParticles)
        {
            p.draw(buffer, color++);
        }
        refreshScreen(buffer);
        usleep(frameDelay);
        // printf("dt: %f", dt);
    }

    printf("Hello World!\n");
    return 0;
}
