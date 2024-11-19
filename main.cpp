#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <random>
#include <stdint.h>
#include <unistd.h>

#define WIDTH 90
#define HEIGHT 40
#define charSetLength 4
#define FrameRate 10
#define frameDelay 1000000 / FrameRate

char charSet[] =
    {
        '.',
        '-',
        '+',
        '#',
};

uint8_t inverseCharSet(char symbol)
{

    switch (symbol)
    {
    case '.':
    return 0;
    break;

    case '-':
    return 1;
    break;

    case '+':
    return 2;
    break;

    case '#':
    return 3;
    break;
    
    default:
    return 255;
    break;
    }
}

struct charColor
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    char symbol;
}typedef charColor;

charColor uint8_tToCharColor(uint8_t value)
{

    uint8_t rMask = 0b00000011;
    uint8_t gMask = 0b00001100;
    uint8_t bMask = 0b00110000;
    uint8_t symbolMask = 0b11000000;

    //find the red value
    uint8_t r = rMask & value;

    //find the red value
    uint8_t g = gMask & value;
    g >>= 2;

    //find the red value
    uint8_t b = bMask & value;
    b >>= 4;

    //find the red value
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

std::string formattedAsciixel(const charColor color) {
    // Map 2-bit red, green, and blue to a 6x6x6 grid index (216 colors)
    // Scaling 0-3 (2-bit) to 0-5 (for the 6x6x6 grid)
    int red = color.r * 2;    // Map 0-3 -> 0-5
    int green = color.g * 2;  // Map 0-3 -> 0-5
    int blue = color.b * 2;   // Map 0-3 -> 0-5

    // Calculate the 256-color index based on the 6x6x6 color cube
    int colorIndex = 16 + (red * 36) + (green * 6) + blue;  // 216 colors + 16 base colors

    // Start ANSI escape sequence for 256-color mode
    std::string colorCode = "\033[38;5;";
    colorCode += std::to_string(colorIndex) + "m";  // Add the color index

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
            displayOutput += formattedAsciixel(asciixel) + " ";
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

int main()
{
    int direction = 0;

    // Initialize the buffer with some values (for testing, we fill it with random values)
    uint8_t buffer[WIDTH * HEIGHT];

    Vec2 pos1 = {0, -10};
    Vec2 pos2 = {WIDTH, HEIGHT};
    Vec2 velocity = {2.0f, 1.0f};

    while (true)
    {
        clearBuffer(buffer, 0);
        std::string msg;
        // randomizeBuffer(buffer);
        drawLine(buffer, pos1, pos2, 4);
        drawPixelV(buffer, pos1, rand());
        drawPixelV(buffer, pos2, rand());
        // drawRectangle(buffer, pos1.X, pos1.Y, 4, 4, charColorToUint8_t(charColor{.r = 3, .g = 3, .b = 3, .symbol = '#'}));
        refreshScreen(buffer);
        pos1 += velocity;
        pos2 -= velocity;
        usleep(frameDelay);
    }

    printf("Hello World!\n");
    return 0;
}
