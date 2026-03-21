#pragma once
class Vector3;
class Vector2;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
Vector3 CalcAngle(Vector3 localPlayerHead, Vector3 targetHead);
float GetDistance(Vector3 p1, Vector3 p2);
bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight);