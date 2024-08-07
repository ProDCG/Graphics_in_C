#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>

#define M_PI 3.14159265358979323846

typedef struct {
    double x, y, z;
} Point3D;

Point3D rotateX(Point3D point, double angle) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    Point3D res;
    res.x = point.x;
    res.y = point.y * cosA - point.z * sinA;
    res.z = point.y * sinA + point.z * cosA;
    return res;
}

Point3D rotateY(Point3D point, double angle) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    Point3D res;
    res.x = point.x * cosA + point.z * sinA;
    res.y = point.y;
    res.z = -point.x * sinA + point.z * cosA;
    return res;
}

Point3D rotateZ(Point3D point, double angle) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    Point3D res;
    res.x = point.x * cosA - point.y * sinA;
    res.y = point.x * sinA + point.y * cosA;
    res.z = point.z;
    return res;
}

void scale(Point3D* vertices, int count, double scalar) {
    for (int i = 0; i < count; i++) {
        vertices[i].x *= scalar;
        vertices[i].y *= scalar;
        vertices[i].z *= scalar;
    }
}

void translateZ(Point3D* vertices, int count, double scalar) {
    for (int i = 0; i < count; i++) {
        vertices[i].z += scalar;
    }
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
    double cameraDistance = 200.0;

    HDC screen = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP memBitmap = CreateCompatibleBitmap(screen, WIDTH, HEIGHT);
    SelectObject(memDC, memBitmap);

    Point3D vertices[8] = {
        {-1, -1, 1},     // A, 0
        {1, -1, 1},      // B, 1
        {1, -1, -1},     // C, 2
        {-1, -1, -1},    // D, 3
        {-1, 1, 1},      // E, 4
        {1, 1, 1},       // F, 5
        {1, 1, -1},      // G, 6
        {-1, 1, -1}      // H, 7
    };

    int polygons[6][4] = {
        {0, 1, 2, 3}, // ABCD -y
        {4, 5, 6, 7}, // EFGH +y
        {0, 3, 7, 4}, // ADEH -x
        {1, 2, 6, 5}, // BCFG +x
        {2, 3, 7, 6}, // CDGH -z
        {0, 4, 5, 1}  // AEBF
    };

    int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
    int indices = sizeof(polygons) / sizeof(polygons[0]);

    for(;;) {
        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(memDC, &(RECT){0, 0, WIDTH, HEIGHT}, bgBrush);
        DeleteObject(bgBrush);

        double currentTime = (double)clock() / 50;
        double angle = 2 * currentTime / 180.0 * M_PI;

        HPEN rainbowPen = CreatePen(PS_SOLID, 1, GetRainbowColor(currentTime));
        HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

        Point3D rotatedVerts[8];
        memcpy(rotatedVerts, vertices, sizeof(vertices));
        for (int i = 0; i < vertexCount; i++) {
            rotatedVerts[i] = rotateX(vertices[i], angle);
            rotatedVerts[i] = rotateY(rotatedVerts[i], angle);
            rotatedVerts[i] = rotateZ(rotatedVerts[i], angle);
        }

        scale(rotatedVerts, vertexCount, 50 + 25 * cos(0.5 * currentTime));


        SelectObject(memDC, rainbowPen);

        POINT points[4];
        for (int i = 0; i < indices; i++) {
            for (int j = 0; j < 4; j++) {
                Point3D v = rotatedVerts[polygons[i][j]];
                points[j].x = WIDTH / 2 + (FOV * v.x) / (v.z + cameraDistance);
                points[j].y = HEIGHT / 2 + (FOV * v.y) / (v.z + cameraDistance);
            }
            Polyline(memDC, points, 4);
        }

        BitBlt(screen, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);

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
