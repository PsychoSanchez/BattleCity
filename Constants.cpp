module;

#include <windows.h>
#include "gdiplus.h"

export module constants;

import transform;

export const int FRAME_TIMER = 1;
export const int FRAME_PERIOD = 4; // How often we run update function
export const int GAME_TICK_INTERVAL = 100; // 100 ms 1 game tick
export const int ARMOR_TIMER = 2;
export const int ARMOR_PERIOD = 25000;
export const int HEALTH_TIMER = 3;
export const int HEALTH_PERIOD = 10000;

export int MOVEMENT_CONTROLS_1[] = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT };
export int FIRE_CONTROL_1 = VK_SPACE;
export int MOVEMENT_CONTROLS_2[] = { 'W', 'S', 'A', 'D' };
export int FIRE_CONTROL_2 = VK_LSHIFT;
export int MOVEMENT_CONTROLS_3[] = { 'T', 'G', 'F', 'H' };
export int FIRE_CONTROL_3 = 'B';
export int MOVEMENT_CONTROLS_4[] = { 'I', 'K', 'J', 'L' };
export int FIRE_CONTROL_4 = 'M';

export LookDirection CONTROL_TO_LOOK_DIRECTION_MAP[] = { Top, Down, Left, Right };

export int COLUMN_COUNT = 30;
export int ROW_COUNT = 20;
export int CELLS_SIZE = 32;
export int SPAWN_POINTS[4][2] = { {0,0}, {COLUMN_COUNT - 1, 0}, {COLUMN_COUNT - 1, ROW_COUNT - 1}, {0, ROW_COUNT - 1} };
