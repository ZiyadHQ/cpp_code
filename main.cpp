#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <random>
#include <unistd.h>

#define WIDTH 60
#define HEIGHT 40
#define charSetLength 10
#define FrameRate 3
#define frameDelay 1000000 / FrameRate

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
        this->X += other.X;
        this->Y += other.Y;
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

char charSet[] =
    {
        ' ',
        '.',
        ':',
        '-',
        '=',
        '+',
        '*',
        '#',
        '%',
        '@'};

// Function to refresh the screen and print it
void refreshScreen(__uint8_t buffer[])
{
    printf("\033[H\033[2J\033[H");
    std::string displayOutput = "";

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++) // Corrected the condition from 'i < WIDTH' to 'j < WIDTH'
        {
            int index = i * WIDTH + j;
            __uint8_t value = buffer[index];
            char ascii_xel = charSet[value % charSetLength];
            displayOutput += ascii_xel; // ascii pixel
            displayOutput += ' ';       // Add space for readability
        }
        displayOutput += "\n"; // Newline after each row of characters
    }

    std::cout.flush() << displayOutput;
}

void clearBuffer(__uint8_t buffer[], __uint8_t value)
{

    for (int i = 0; i < HEIGHT * WIDTH; i++)
    {
        buffer[i] = value;
    }
}

void randomizeBuffer(__uint8_t buffer[])
{

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++) // Corrected the condition from 'i < WIDTH' to 'j < WIDTH'
        {
            int index = i * WIDTH + j;
            buffer[index] = rand() % 10;
        }
    }
}

bool drawPixel(__uint8_t buffer[], int X, int Y, __uint8_t value)
{

    if (X >= WIDTH || X < 0 || Y >= HEIGHT || Y < 0)
    {
        return false;
    }

    buffer[Y * WIDTH + X] = value;
    return true;
}

bool drawPixelV(__uint8_t buffer[], Vec2 position, __uint8_t value)
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

void drawLine(__uint8_t buffer[], Vec2 startPos, Vec2 endPos, __uint8_t value)
{
    Vec2 direction = endPos - startPos;
    direction = direction.Normalize();

    Vec2 position = startPos;

    float distance = startPos.Distance(startPos, endPos);

    for(int i=0; i<distance; i++)
    {
        drawPixelV(buffer, position, value);
        position += direction;
    }
}

void drawRectangle(__uint8_t buffer[], int X, int Y, int colWidth, int colHeight, __uint8_t value)
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

int main()
{
    int direction = 0;

    // Initialize the buffer with some values (for testing, we fill it with random values)
    __uint8_t buffer[WIDTH * HEIGHT];

    Vec2 pos1 = {10, 10};

    Vec2 pos2 = {20, 20};

    Vec2 velocity = {1, 0};

    while (true)
    {
        clearBuffer(buffer, 0);
        std::string msg;
        drawLine(buffer, pos1, pos2, 4);
        drawPixelV(buffer, pos1, 9);
        drawPixelV(buffer, pos2, 9);
        refreshScreen(buffer);
        pos1 += velocity;
        usleep(frameDelay);
    }

    printf("Hello World!\n");
    return 0;
}
