#include "pch.h"
#include "cheatMath.h"
#include <iostream>
#include "vector3.h"



Vector3 CalcAngle(Vector3 localPlayerHead, Vector3 targetHead) {
    Vector3 angle = { 0, 0, 0 };
    float dx = targetHead.x - localPlayerHead.x;
    float dy = targetHead.y - localPlayerHead.y;
    float dz = targetHead.z - localPlayerHead.z;
    float distance = std::sqrt(dx * dx + dy * dy);
    angle.x = (float)(atan2(dy, dx) * 180.0f / M_PI) + 90.0f;
    angle.y = (float)(atan2(dz, distance) * 180.0f / M_PI);
    return angle;
}

float GetDistance(Vector3 p1, Vector3 p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    float dz = p1.z - p2.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight) {
    float clipCoordsX = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
    float clipCoordsY = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
    float clipCoordsW = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];
   
    //if W < 0.1, then the object is behind our camera (we don’t draw it)
    if (clipCoordsW < 0.1f) return false;
    float NDCx = clipCoordsX / clipCoordsW;
    float NDCy = clipCoordsY / clipCoordsW;

    //screen pixel translation (Y inverts, as in OpenGL 0.0 is the left bottom or top corner depending on port)
    screen.x = (windowWidth / 2 * NDCx) + (NDCx + windowWidth / 2);
    screen.y = -(windowHeight / 2 * NDCy) + (NDCy + windowHeight / 2);
    return true;
}