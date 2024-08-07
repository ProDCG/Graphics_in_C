#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>

COLORREF GetRainbowColor(double time) {
    const double frequency = 0.3;

    BYTE red = (BYTE)(sin(frequency * time + 0) * 127 + 128);
    BYTE green = (BYTE)(sin(frequency * time + 2) * 127 + 128);
    BYTE blue = (BYTE)(sin(frequency * time + 4) * 127 + 128);

    return RGB(red, green, blue);
}

int main() {
    HDC screen = GetDC(NULL);
    

    for(;;) {
        double currentTime = (double)clock() / 50;

        COLORREF color = GetRainbowColor(currentTime);

        HBRUSH brush = CreateSolidBrush(color);

        SelectObject(screen, brush);

        // Rectangle(screen, 50, 50, 500, 500);
        // Polygon(screen, (POINT[]){ {50, 50}, {500, 50}, {500, 500}, {50, 500} }, 4);

        DeleteObject(brush);

        Sleep(20);
    }

    ReleaseDC(NULL, screen);    

    return 0;
}

// cl Gdi32.lib User32.lib draw.c