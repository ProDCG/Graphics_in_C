#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>

typedef struct {
    UINT16 x, y, z;
} Point3D;

Point3D rotateX(Point3D point, double angle) {
    Point3D res;
    res.x = point.x;
    res.y = point.y * cos(angle) - point.z * sin(angle);
    res.z = point.y * sin(angle) + point.z * cos(angle);
    return res;
}

COLORREF GetRainbowColor(double time) {
    const double frequency = 0.3;

    BYTE red = (BYTE)(sin(frequency * time + 0) * 127 + 128);
    BYTE green = (BYTE)(sin(frequency * time + 2) * 127 + 128);
    BYTE blue = (BYTE)(sin(frequency * time + 4) * 127 + 128);

    return RGB(red, green, blue);
}

int main() {
    UINT16 FOV = 60;
    UINT16 WIDTH = 500;
    UINT16 HEIGHT = 500;
    UINT8 vertices = 3;

    HDC screen = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP memBitmap = CreateCompatibleBitmap(screen, WIDTH, HEIGHT);
    SelectObject(memDC, memBitmap);

    Point3D plane[3] = { {0, 0, 0}, {100, 100, 10}, {100, 0, 0}};
    POINT points[3];

    for(;;) {
        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(memDC, &(RECT){0, 0, WIDTH, HEIGHT}, bgBrush);
        DeleteObject(bgBrush);

        double currentTime = (double)clock() / 50;
        COLORREF color = GetRainbowColor(currentTime);
        HBRUSH brush = CreateSolidBrush(color);
        // HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
        // HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));

        // SelectObject(memDC, black);

        // rotation
        // double angle = 0.01;
        // for (int i = 0; i < 3; i++) {
        //     plane[i] = rotateX(plane[i], 0);
        // }
        // angle += 0.01;

        // projection
        for (int i = 0; i < 3; i++) {
            points[i].x = WIDTH / 2 + (FOV * plane[i].x) / (plane[i].z + FOV);
            points[i].y = HEIGHT / 2 + (FOV * plane[i].y) / (plane[i].z + FOV);
        }

        // Rectangle(screen, 0, 0, WIDTH, HEIGHT);
        FillRgn(memDC, CreatePolygonRgn(points, 3, WINDING), brush);

        BitBlt(screen, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);

        // DeleteObject(brush);
        // DeleteObject(black);

        Sleep(20);
    }

    DeleteObject(memBitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, screen);

    return 0;
}

// Developer Command Prompt Visual Studio
// cl Gdi32.lib User32.lib main.c
// main.exe