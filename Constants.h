#pragma once

const float FIXED_DELTATIME = 1.0f / 50.0f;
const unsigned short WINDOW_WIDTH = 1024; // 455
const unsigned short WINDOW_HEIGHT = 768; // 256
const unsigned short FOV = 800; // the verticle length of window in game global axis is 800;
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
const float PI = 3.14159265358979323846f;
const float TWO_PI = 2.0f * PI;
const float INV255 = 1.0f / 255.0f;
const float INV_WINDOW_WIDTH = 1.f / WINDOW_WIDTH;
const float INV_WINDOW_HEIGHT = 1.f / WINDOW_HEIGHT;
const float STEP_X = (float)FOV * ASPECT_RATIO / WINDOW_WIDTH;
const float STEP_Y = (float)FOV / WINDOW_HEIGHT;