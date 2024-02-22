module;

#include <windows.h>
#include <tchar.h>
#include "gdiplus.h"

export module spritemanager;
import std.core;

using namespace Gdiplus;
using namespace std;

export const REAL TILE_SIZE = 16;

export struct TextureCoordinates {
    REAL x;
    REAL y;
    REAL width;
    REAL height;
    Unit unit;
};

export TextureCoordinates BRICK_META = {
    0 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

export TextureCoordinates CONCRETE_META = {
    1 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

export TextureCoordinates ARMOR_PICKUP_META = {
    2 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

export TextureCoordinates HEALTH_PICKUP_META = {
    7 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

export TextureCoordinates NET_META = {
    7 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

export TextureCoordinates TANK_A_FRAMES[8] = {
    {0 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {2 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {4 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 1 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    // Upgraded tank
    {0 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {2 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {4 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
};

export TextureCoordinates TANK_B_FRAMES[8] = {
    {0 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {2 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {4 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    // Upgraded tank
    {0 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {2 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {4 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
};

export TextureCoordinates SHELL_FRAMES[] = {
    {3 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {4 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {5 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel} };

export TextureCoordinates EXPLOSION_FRAMES[] = {
    {0 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {1 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {2 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel} };

export TextureCoordinates SPAWN_FRAMES[] = {
    {4 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {5 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel} };

export class SpriteManager {
private:
    Bitmap* texture;
    int cellSize;
    int offsetY = 16;

public:
    SpriteManager(Bitmap* texture, int cellSize) {
        this->texture = texture;
        this->cellSize = cellSize;
    }

    // x, y - cell coordinates (not pixels)
    void draw(Graphics* g, TextureCoordinates* meta, int x, int y) {
        RectF dest = RectF(x * cellSize, y * cellSize + offsetY, cellSize, cellSize);
        g->DrawImage(texture, dest, meta->x, meta->y, meta->width - 0.5, meta->height -0.5,
            meta->unit);
    }
};
