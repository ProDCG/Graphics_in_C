#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>

typedef struct {
    double x, y, z;
} Point3D;

Point3D rotateX(Point3D point, double angle) {
    Point3D res;
    res.x = point.x;
    res.y = point.y * cos(angle) - point.z * sin(angle);
    res.z = point.y * sin(angle) + point.z * cos(angle);
    return res;
}

Point3D rotateY(Point3D point, double angle) {
    Point3D res;
    res.x = point.x * cos(angle) + point.z * sin(angle);
    res.y = point.y;
    res.z = point.x * sin(angle) - point.z * cos(angle);
    return res;
}

Point3D rotateZ(Point3D point, double angle) {
    Point3D res;
    res.x = point.x * cos(angle) - point.y * sin(angle);
    res.y = point.x * sin(angle) + point.y * cos(angle);
    res.z = point.z;
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
    UINT16 FOV = 90;
    UINT16 WIDTH = 500;
    UINT16 HEIGHT = 500;

    HDC screen = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP memBitmap = CreateCompatibleBitmap(screen, WIDTH, HEIGHT);
    SelectObject(memDC, memBitmap);

    Point3D plane[3] = { {-50, 0, 200}, {0, 100, 200}, {50, 0, 200} };
    POINT points[3];

    UINT8 vertices = sizeof(plane) / sizeof(plane[0]);

    for(;;) {
        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(memDC, &(RECT){0, 0, WIDTH, HEIGHT}, bgBrush);
        DeleteObject(bgBrush);

        double currentTime = (double)clock() / 50;
        // COLORREF color = GetRainbowColor(currentTime);
        // HBRUSH brush = CreateSolidBrush(color);
        // HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
        HPEN black = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        // HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));

        // SelectObject(memDC, black);

        // rotation
        // double angle = 0.01;
        // for (int i = 0; i < 3; i++) {
        //     plane[i] = rotateX(plane[i], 0);
        // }
        // angle += 0.01;

        Point3D verts[3];
        for (int i = 0; i < vertices; i++) {
            verts[i] = rotateX(plane[i], 2 * currentTime / 180.0 * 3.14159);
            verts[i] = rotateY(verts[i], 2 * currentTime / 180.0 * 3.14159);
            verts[i] = rotateZ(verts[i], 2 * currentTime / 180.8 * 3.14159);
        }
        printf("Value within rotation: %f", 2 * currentTime / 180.0 * 3.14159);


        // projection
        for (int i = 0; i < vertices; i++) {
            points[i].x = WIDTH / 2 + (FOV * verts[i].x) / (verts[i].z + FOV + 300);
            points[i].y = HEIGHT / 2 + (FOV * verts[i].y) / (verts[i].z + FOV + 300);
        }

        // Rectangle(screen, 0, 0, WIDTH, HEIGHT);
        // FillRgn(memDC, CreatePolygonRgn(points, 3, WINDING), brush);
        SelectObject(memDC, black);
        // Polygon(memDC, points, 3);

        // Draw a black polygon, except leave the empty unfilled
        Polygon(memDC, points, vertices);
        // Polyline(memDC, points, 3);


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