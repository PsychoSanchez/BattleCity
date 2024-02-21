#include "StdAfx.h"
#include "gdiplus.h"
#include "sstream"
#include "time.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

using namespace Gdiplus;
using namespace std;

// struct Vector2 {
//   int x, y;
// };

// enum Direction { Left, Right, Bottom, Top };

// enum WallType { Brick, Concrete, Steel, Net, None };

// class Transform {
// private:
//   int x, y;
//   Direction direction;

// public:
//   Transform(int x, int y, Direction direction);
//   Vector2 getPosition();
//   Direction getDirection();
// };

// class GameDraw {
// class Sprite {
// private:
//   Texture texture;

// public:
//   Sprite(str path);
//   ~Sprite();
//   void draw();
// };

// class Player {
// class Tank {
// private:
//   int id;
//   int health;
//   int armor;

// public:
//   Tank();
//   ~Tank();
//   int getId();
//   void kill();
//   void move(Direction direction);
//   Direction getDirection();
//   Vector2 getPosition();
//   void applyDamage();
// };

// class Projectile
// class Shot {
// private:
//   shared_ptr<Tank> owner;
//   Transform transform;

// public:
//   Shot(shared_ptr<Tank>, unique_ptr<Transform> transform);
//   ~Shot();
//   void move();
// };

// class Wall {
enum WallType { Brick, Concrete, Steel, Net, None };

class Wall {
private:
  int x, y;
  int health;
  int armor;
  WallType type;

public:
  Wall(int x, int y, WallType type, int size) {
    this->x = x;
    this->y = y;
    this->type = type;
    this->health = 1;
    this->armor = 0;
  }
  void applyDamage() {
    if (armor > 0) {
      armor--;
    } else {
      health--;
    }

    if (health <= 0) {
      type = WallType::None;
    }
  }
  WallType getType() { return type; }
  int getHealth() { return health; }
  int getArmor() { return armor; }
  int getX() { return x; }
  int getY() { return y; }
};

// class GameState {
// private:
//   vector<shared_ptr<Tank>> tanks;
//   vector<unique_ptr<Shot>> shots;
//   vector<unique_ptr<Pole>> poles;

// public:
//   void addTank(shared_ptr<Tank> tank);
//   void addPole(unique_ptr<Pole> pole);
//   void addShot(unique_ptr<Shot> shot);
//   void loop();
//   void draw();
// };

// int[] KEYS_1 = {VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT};
// char[] KEYS_2 = {'W', 'S', 'A', 'D'};
// Direction[] DIRECTIONS = {Top, Bottom, Left, Right};

// Direction getTankDirection(void *keys) {
//   for (int i = 0; i < 4; i++) {
//     if (GetKeyState(keys[i]) < 0) {
//       return DIRECTIONS[i];
//     }
//   }

//   return None;
// }

// bool isFirePressed(int buttonKeyCode) { return GetKeyState(buttonKeyCode) <
// 0; }

// void GameState::loop() {
//   for (int i = 0; i < shots.size(); i++) {
//     for (int j = 0; j < poles.size(); j++) {
//       if (shots[i]->getPosition() == poles[j]->getPosition()) {
//         poles[j]->applyDamage();
//         poles[j]->kill();
//         shots[i]->kill();
//       }
//     }

//     for (int j = 0; j < tanks.size(); j++) {
//       if (shots[i]->getPosition() == tanks[j]->getPosition()) {
//         tanks[j]->applyDamage();
//         shots[i]->kill();
//       }
//     }
//   }

//   for (int i = 0; i < tanks.size(); i++) {
//     Direction direction = getTankDirection(KEYS_1);
//     tanks[i]->move(direction);

//     if (isFirePressed(VK_RCONTROL)) {
//       Vector2 position = tanks[i]->getPosition();

//       shots.push_back(unique_ptr<Shot>(new Shot(
//           tanks[i], unique_ptr<Transform>(new Transform(
//                         position.x, position.y,
//                         tanks[i]->getDirection())))));
//     }
//   }

//   draw();
// }

// void GameState::draw() {
//   for (int i = 0; i < tanks.size(); i++) {
//     tanks[i]->move();
//   }
//   for (int i = 0; i < poles.size(); i++) {
//     poles[i]->move();
//   }
//   for (int i = 0; i < shots.size(); i++) {
//     shots[i]->move();
//   }
// }

int COLUMN_COUNT = 10;
int ROW_COUNT = 10;
int CELLS_SIZE = 50;
REAL TILE_SIZE = 16;

vector<vector<unique_ptr<Wall>>> generateWalls(int columnCount, int rowCount) {
  srand((unsigned)time(NULL));

  auto poles = vector<vector<unique_ptr<Wall>>>();

  for (int x = 0; x < columnCount; x++) {
    auto row = vector<unique_ptr<Pole>>();

    for (int y = 0; y < rowCount; y++) {
      int type = rand() % 4;
      WallType wallType = WallType::None;
      if (type == 0) {
        wallType = WallType::Brick;
      } else if (type == 1) {
        wallType = WallType::Concrete;
      } else if (type == 2) {
        wallType = WallType::Net;
      } else {
        wallType = WallType::None;
      }

      row.push_back(unique_ptr<Pole>(
          new Wall(x * CELLS_SIZE, y * CELLS_SIZE, wallType, CELLS_SIZE)));
    }

    poles.push_back(row);
  }

  return poles;
}

struct TextureCoordinates {
  REAL x;
  REAL y;
  REAL width;
  REAL height;
  Unit unit;
};

TextureCoordinates BRICK_META = {
    0, 0, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates CONCRETE_META = {
    16, 0, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates ARMOR_PICKUP_META = {
    32, 0, TILE_SIZE, TILE_SIZE, UnitPixel,
};
TextureCoordinates HEALTH_PICKUP_META = {
    112, 0, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates NET = {
    112, 64, TILE_SIZE, TILE_SIZE, UnitPixel,
};

TextureCoordinates TANK_A_FRAMES[8] = {
    {0, 16, TILE_SIZE, TILE_SIZE, UnitPixel},
    {32, 16, TILE_SIZE, TILE_SIZE, UnitPixel},
    {64, 16, TILE_SIZE, TILE_SIZE, UnitPixel},
    {96, 16, TILE_SIZE, TILE_SIZE, UnitPixel},
    // Upgraded tank
    {0, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
    {32, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
    {64, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
    {96, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
};

TextureCoordinates TANK_B_FRAMES[8] = {
    {0, 32, TILE_SIZE, TILE_SIZE, UnitPixel},
    {32, 32, TILE_SIZE, TILE_SIZE, UnitPixel},
    {64, 32, TILE_SIZE, TILE_SIZE, UnitPixel},
    {96, 32, TILE_SIZE, TILE_SIZE, UnitPixel},
    // Upgraded tank
    {0, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
    {32, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
    {64, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
    {96, 64, TILE_SIZE, TILE_SIZE, UnitPixel},
};

TextureCoordinates SHELL_FRAMES[] = {{48, 0, TILE_SIZE, TILE_SIZE, UnitPixel},
                                     {64, 0, TILE_SIZE, TILE_SIZE, UnitPixel},
                                     {80, 0, TILE_SIZE, TILE_SIZE, UnitPixel},
                                     {96, 0, TILE_SIZE, TILE_SIZE, UnitPixel}};

TextureCoordinates EXPLOSION_FRAMES[] = {
    {0, 48, TILE_SIZE, TILE_SIZE, UnitPixel},
    {16, 48, TILE_SIZE, TILE_SIZE, UnitPixel},
    {32, 48, TILE_SIZE, TILE_SIZE, UnitPixel}};

TextureCoordinates SPAWN_FRAMES[] = {{64, 48, TILE_SIZE, TILE_SIZE, UnitPixel},
                                     {80, 48, TILE_SIZE, TILE_SIZE, UnitPixel},
                                     {96, 48, TILE_SIZE, TILE_SIZE, UnitPixel}};
class SpriteManager {
private:
  Bitmap *texture;
  Graphics *g;
  int cellSize;

public:
  SpriteManager(Graphics *g, Bitmap *texture, int cellSize) {
    this->g = g;
    this->texture = texture;
    this->cellSize = cellSize;
  }

  void draw(TextureCoordinates *meta, int x, int y) {
    RectF rect = RectF(x, y, TILE_SIZE, TILE_SIZE);
    g->DrawImage(texture, rect, meta->x * cellSize, meta->y * cellSize,
                 meta->width, meta->height, meta->unit);
  }
};

TextureCoordinates *getWallTexture(WallType type) {
  switch (type) {
  case WallType::Brick:
    return &BRICK_META;
  case WallType::Concrete:
    return &CONCRETE_META;
  case WallType::Net:
    return &NET;
  default:
    return nullptr;
  }
}

int draw() {
  // SolidBrush blackBrush(Color::Black);
  // SolidBrush whiteBrush(Color::White);

  int bbx = COLUMN_COUNT * CELLS_SIZE;
  int bby = ROW_COUNT * CELLS_SIZE + 16;

  Bitmap backBuffer(bbx, bby);
  Graphics graphics(&backBuffer);

  graphics.SetSmoothingMode(SmoothingModeAntiAlias);
  graphics.Clear(Color::Black);

  SpriteManager spriteManager(
      &graphics, Bitmap::FromFile(L"textures\\tanks.png"), TILE_SIZE);

  auto walls = generateWalls(COLUMN_COUNT, ROW_COUNT);

  for (auto wallRow : walls) {
    for (auto wall : wallRow) {
      auto wallType = wall->getType();
      if (wallType == WallType::None) {
        continue;
      }

      spriteManager.draw(getWallTexture(wallType), wall->getX(), wall->getY());
    }
  }

  // GameState game;
  // game.loop();
  // return 0;
}
