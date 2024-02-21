#include "StdAfx.h"
#include "gdiplus.h"
#include <iostream>
#include <vector>

using namespace Gdiplus;
using namespace std;

const REAL TILE_SIZE = 16;

struct TextureCoordinates {
  REAL x;
  REAL y;
  REAL width;
  REAL height;
  Unit unit;
};

TextureCoordinates BRICK_META = {
    0 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates CONCRETE_META = {
    1 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates ARMOR_PICKUP_META = {
    2 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};
TextureCoordinates HEALTH_PICKUP_META = {
    7 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates NET_META = {
    7 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates TANK_A_FRAMES[8] = {
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

TextureCoordinates TANK_B_FRAMES[8] = {
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

TextureCoordinates SHELL_FRAMES[] = {
    {3 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {4 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, UnitPixel},
    {5 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel}};

TextureCoordinates EXPLOSION_FRAMES[] = {
    {0 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {1 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {2 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel}};

TextureCoordinates SPAWN_FRAMES[] = {
    {4 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {5 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel},
    {6 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, UnitPixel}};

class SpriteManager {
private:
  Bitmap *texture;
  int cellSize;

public:
  SpriteManager(Bitmap *texture, int cellSize) {
    this->texture = texture;
    this->cellSize = cellSize;
  }

  // x, y - cell coordinates (not pixels)
  void draw(Graphics *g, TextureCoordinates *meta, int x, int y) {
    RectF dest = RectF(x * cellSize, y * cellSize, cellSize, cellSize);
    g->DrawImage(texture, dest, meta->x, meta->y, meta->width, meta->height,
                 meta->unit);
  }
};
