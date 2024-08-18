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

void scale(Point3D* cube_verts, int count, double scalar) {
    for (int i = 0; i < count; i++) {
        cube_verts[i].x *= scalar;
        cube_verts[i].y *= scalar;
        cube_verts[i].z *= scalar;
    }
}

void translateZ(Point3D* cube_verts, int count, double scalar) {
    for (int i = 0; i < count; i++) {
        cube_verts[i].z += scalar;
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
    UINT16 WIDTH = 1000;
    UINT16 HEIGHT = 1000;
    double cameraDistance = 500.0;

    HDC screen = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP memBitmap = CreateCompatibleBitmap(screen, WIDTH, HEIGHT);
    SelectObject(memDC, memBitmap);

    const double phi = (1.0 + sqrt(5.0)) * 0.5;
    const double a = 1.0;
    const double b = 1.0 / phi;

    Point3D ico_verts[12] = {
        {0, b, -a},   {b, a, 0},   {-b, a, 0},  // 0, 1, 2
        {0, b, a},    {0, -b, a},  {-a, 0, b},  // 3, 4, 5
        {0, -b, -a},  {a, 0, -b},  {a, 0, b},   // 6, 7, 8
        {-a, 0, -b},  {b, -a, 0},  {-b, -a, 0}  // 9, 10, 11
    };

    int ico_faces[20][3] = {
        {0, 1, 2}, {3, 2, 1}, {3, 4, 5}, {3, 8, 4},
        {0, 6, 7}, {0, 9, 6}, {4, 10, 11}, {6, 11, 10},
        {2, 5, 9}, {11, 9, 5}, {1, 7, 8}, {10, 8, 7},
        {3, 5, 2}, {3, 1, 8}, {0, 2, 9}, {0, 7, 1},
        {6, 10, 7}, {6, 9, 11}, {4, 8, 10}, {4, 11, 5}
    };

    int ico_vert_count = sizeof(ico_verts) / sizeof(ico_verts[0]);
    int ico_face_count = sizeof(ico_faces) / sizeof(ico_verts[0]);

    Point3D cube_verts[8] = {
        {-1, -1, 1},     // A, 0
        {1, -1, 1},      // B, 1
        {1, -1, -1},     // C, 2
        {-1, -1, -1},    // D, 3
        {-1, 1, 1},      // E, 4
        {1, 1, 1},       // F, 5
        {1, 1, -1},      // G, 6
        {-1, 1, -1}      // H, 7
    };

    int cube_faces[6][4] = {
        {0, 1, 2, 3}, // ABCD -y
        {4, 5, 6, 7}, // EFGH +y
        {0, 3, 7, 4}, // ADEH -x
        {1, 2, 6, 5}, // BCFG +x
        {2, 3, 7, 6}, // CDGH -z
        {0, 4, 5, 1}  // AEBF
    };

    int cube_vert_count = sizeof(cube_verts) / sizeof(cube_verts[0]);
    int cube_face_count = sizeof(cube_faces) / sizeof(cube_faces[0]);

    for(;;) {
        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(memDC, &(RECT){0, 0, WIDTH, HEIGHT}, bgBrush);
        DeleteObject(bgBrush);

        double currentTime = (double)clock() / 50;
        // currentTime = 0;
        double angle = 2 * 2 * currentTime / 180.0 * M_PI;

        HPEN rainbowPen = CreatePen(PS_SOLID, 2, GetRainbowColor(currentTime));
        HPEN blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

        SelectObject(memDC, rainbowPen);

        Point3D rotatedVerts[12];
        memcpy(rotatedVerts, ico_verts, sizeof(ico_verts));
        for (int i = 0; i < ico_vert_count; i++) {
            rotatedVerts[i] = rotateX(ico_verts[i], angle);
            rotatedVerts[i] = rotateY(rotatedVerts[i], angle);
            rotatedVerts[i] = rotateZ(rotatedVerts[i], angle);
        }

        scale(rotatedVerts, 12, 250);

        POINT points[3];
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 3; j++) {
                Point3D v = rotatedVerts[ico_faces[i][j]];
                points[j].x = WIDTH / 2 + (FOV * v.x) / (v.z + cameraDistance);
                points[j].y = WIDTH / 2 + (FOV * v.y)  / (v.z + cameraDistance);
            }
            Polyline(memDC, points, 3);
        }

        // Point3D rotatedVerts[8];
        // memcpy(rotatedVerts, cube_verts, sizeof(cube_verts));
        // for (int i = 0; i < cube_vert_count; i++) {
        //     rotatedVerts[i] = rotateX(cube_verts[i], angle);
        //     rotatedVerts[i] = rotateY(rotatedVerts[i], angle);
        //     rotatedVerts[i] = rotateZ(rotatedVerts[i], angle);
        // }

        // scale(rotatedVerts, cube_vert_count, 150 + 25 * cos(0.25 * currentTime));


        // POINT points[4];
        // for (int i = 0; i < cube_face_count; i++) {
        //     for (int j = 0; j < 4; j++) {
        //         Point3D v = rotatedVerts[cube_faces[i][j]];
        //         points[j].x = WIDTH / 2 + (FOV * v.x) / (v.z + cameraDistance);
        //         points[j].y = HEIGHT / 2 + (FOV * v.y) / (v.z + cameraDistance);
        //     }
        //     Polyline(memDC, points, 4);
        // }

        // for (int i = 0; i < cube_vert_count; i++) {
        //     rotatedVerts[i] = rotateX(cube_verts[i], angle);
        //     rotatedVerts[i] = rotateY(rotatedVerts[i], angle);
        //     rotatedVerts[i] = rotateZ(rotatedVerts[i], angle);
        // }

        // scale(rotatedVerts, cube_vert_count, 150 - 50 * cos(0.25 * currentTime));

        // for (int i = 0; i < cube_face_count; i++) {
        //     for (int j = 0; j < 4; j++) {
        //         Point3D v = rotatedVerts[cube_faces[i][j]];
        //         points[j].x = WIDTH / 2 + (FOV * v.x) / (v.z + cameraDistance);
        //         points[j].y = HEIGHT / 2 + (FOV * v.y) / (v.z + cameraDistance);
        //     }
        //     Polyline(memDC, points, 4);
        // }

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
