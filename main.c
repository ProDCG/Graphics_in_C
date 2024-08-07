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
    HDC screen = GetDC(NULL);

    Point3D plane[4] = { {0, 0, 0}, {100, 0, 0}, {100, 100, 0}, {0, 100, 0} };
    POINT points[4];

    UINT16 FOV = 60;
    UINT16 WIDTH = 500;
    UINT16 HEIGHT = 500;

    for(;;) {
        double currentTime = (double)clock() / 50;

        COLORREF color = GetRainbowColor(currentTime);

        HBRUSH brush = CreateSolidBrush(color);
        HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));

        SelectObject(screen, RGB(255, 255, 255));

        for (int i = 0; i < 4; i++) {
            points[i].x = WIDTH / 2 + (FOV * plane[i].x) / (plane[i].z + FOV);
            points[i].y = HEIGHT / 2 + (FOV * plane[i].y) / (plane[i].z + FOV);
        }

        Rectangle(screen, 0, 0, WIDTH, HEIGHT);

        // SelectObject(screen, RGB(0, 0, 0));
        // Polygon(screen, (POINT[]){ {50, 50}, {500, 50}, {500, 500}, {50, 500} }, 4);
        // Polygon(screen, points, 4);

        // Write a polygn using the points array, and then fill it with black
        // CreatePolygonRgn(points, 4, WINDING);
        FillRgn(screen, CreatePolygonRgn(points, 4, WINDING), black);

        



        // Polygon(screen, points, 4);

        // double degrees = 0.01;
        // for (int i = 0; i < 4; i++) {
        //     plane[i] = rotateX(plane[i], degrees);
        // }
        // degrees += 0.01;

        DeleteObject(brush);

        Sleep(20);
    }

    ReleaseDC(NULL, screen);    

    return 0;
}

// Developer Command Prompt Visual Studio
// cl Gdi32.lib User32.lib main.c
// main.exe