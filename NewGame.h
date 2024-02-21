
#pragma once
#include "SpriteManager.h"
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

enum LookDirection { Top, Right, Down, Left, None };

int MOVEMENT_CONTROLS_1[] = {VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT};
int MOVEMENT_CONTROLS_2[] = {'W', 'S', 'A', 'D'};
int FIRE_CONTROL_1 = VK_RSHIFT;
int FIRE_CONTROL_2 = VK_LSHIFT;
LookDirection CONTROL_TO_LOOK_DIRECTION_MAP[] = {Top, Down, Left, Right};

int COLUMN_COUNT = 10;
int ROW_COUNT = 10;
int CELLS_SIZE = 30;

enum WallType { Brick, Concrete, Steel, Net, Empty };

struct Vector2 {
  int x, y;
};

bool isButtonPressed(int buttonKeyCode) {
  return GetKeyState(buttonKeyCode) < 0;
}

LookDirection getTankDirection(int *keys) {
  for (int i = 0; i < 4; i++) {
    if (isButtonPressed(keys[i])) {
      return CONTROL_TO_LOOK_DIRECTION_MAP[i];
    }
  }

  return None;
}

class Wall {
private:
  int x, y;
  int health;
  WallType type;

public:
  Wall(int x, int y, WallType type, int size) {
    this->x = x;
    this->y = y;
    this->type = type;
    this->health = 1;
  }
  void applyDamage() {
    if (type != WallType::Brick) {
      return;
    }

    health--;

    if (health <= 0) {
      type = WallType::Empty;
    }
  }
  WallType getType() { return type; }
  int getHealth() { return health; }
  int getX() { return x; }
  int getY() { return y; }
  Vector2 getPosition() { return {x, y}; }
};

class Tank {
private:
  bool isAlive = false;
  int x, y, spawnX = 0, spawnY = 0;
  int health, armor, initialHealth, initialArmor;
  LookDirection direction, spawnDirection = LookDirection::Down;
  int *controls;
  int fireControl;
  TextureCoordinates *frames;

public:
  Tank(int health, int armor) {
    this->initialHealth = health;
    this->initialArmor = armor;
  }

  void setSpawn(int x, int y, LookDirection direction) {
    spawnX = x;
    spawnY = y;
    spawnDirection = direction;
  }

  void setControls(int *controls) { this->controls = controls; }
  int *getControls() { return controls; }
  void setFrames(TextureCoordinates *frames) { this->frames = frames; }
  TextureCoordinates *getFrames() { return frames; }
  void setFireControl(int control) { this->fireControl = control; }
  int getFireControl() { return fireControl; }

  void spawn() {
    x = spawnX;
    y = spawnY;
    direction = spawnDirection;
    health = initialHealth;
    armor = initialArmor;
    isAlive = true;
  }

  bool getIsAlive() { return isAlive; }

  void applyDamage() {
    if (armor > 0) {
      armor--;
    } else {
      health--;
    }

    if (health <= 0) {
      // Kill the tank
      isAlive = false;
    }
  }

  int getX() { return x; }
  int getY() { return y; }

  void setPosition(Vector2 position) {
    this->x = position.x;
    this->y = position.y;
  }
  Vector2 getPosition() { return {x, y}; }

  void setDirection(LookDirection direction) { this->direction = direction; }
  LookDirection getDirection() { return direction; }
};

class Projectile {
private:
  bool isDestroyed = true;
  Vector2 position;
  LookDirection direction;

public:
  Projectile(Vector2 position, LookDirection direction) {
    this->isDestroyed = true;
    this->position = position;
    this->direction = direction;
  }

  void setIsDestroyed(bool isDestroyed) { this->isDestroyed = isDestroyed; }
  bool getIsDestroyed() { return isDestroyed; }
  Vector2 getPosition() { return position; }
  void setPosition(Vector2 position) { this->position = position; }
  LookDirection getDirection() { return direction; }
  void setDirection(LookDirection dir) { this->direction = dir; }
};

class Animation {
private:
  int currentFrame = -1;
  AnimationMeta *meta;
  Vector2 position;

public:
  bool isPlaying() { return currentFrame != -1; }
  void play(AnimationMeta *meta, Vector2 position) {
    this->currentFrame = 0;
    this->meta = meta;
    this->position = position;
  }
  Vector2 getPosition() { return this->position; }
  TextureCoordinates *getFrame() { return &this->meta->frames[currentFrame]; }
  void tick() {
    currentFrame++;

    if (currentFrame >= this->meta->length) {
      currentFrame = -1;
    }
  }
};

bool isTankSpawnPosition(int x, int y, int columnCount, int rowCount) {
  return (x == 0 && y == 0) || (x == columnCount - 1 && y == 0) ||
         (x == 0 && y == rowCount - 1) ||
         (x == columnCount - 1 && y == rowCount - 1);
}

vector<vector<unique_ptr<Wall>>> generateWalls(int columnCount, int rowCount) {
  srand((unsigned)time(NULL));

  auto walls = vector<vector<unique_ptr<Wall>>>();

  for (int x = 0; x < columnCount; x++) {
    auto row = vector<unique_ptr<Wall>>();

    for (int y = 0; y < rowCount; y++) {
      int type =
          isTankSpawnPosition(x, y, columnCount, rowCount) ? 4 : rand() % 4;

      WallType wallType = WallType::Empty;

      switch (type) {
      case 0:
        wallType = WallType::Brick;
        break;
      case 1:
        // Lowering the chances of Concrete wall spawn
        wallType = rand() % 2 == 0 ? WallType::Concrete : WallType::Brick;
        break;
      case 2:
        wallType = WallType::Net;
        break;
      default:
        wallType = WallType::Empty;
        break;
      }

      row.push_back(unique_ptr<Wall>(new Wall(x, y, wallType, CELLS_SIZE)));
    }

    walls.push_back(move(row));
  }

  return walls;
}

TextureCoordinates *getWallTexture(WallType type) {
  switch (type) {
  case WallType::Brick:
    return &BRICK_META;
  case WallType::Concrete:
    return &CONCRETE_META;
  case WallType::Net:
    return &NET_META;
  default:
    return nullptr;
  }
}

Vector2 getPositionInDirection(Vector2 position, LookDirection direction) {
  switch (direction) {
  case LookDirection::Top:
    position.y--;
    break;
  case LookDirection::Down:
    position.y++;
    break;
  case LookDirection::Left:
    position.x--;
    break;
  case LookDirection::Right:
    position.x++;
    break;
  }

  return position;
}

class GameManager {
private:
  int columnCount, rowCount, cellSize;
  Bitmap *buffer;
  Graphics *graphics;
  SpriteManager *spriteManager;
  vector<vector<unique_ptr<Wall>>> walls;
  vector<shared_ptr<Tank>> tanks;
  vector<unique_ptr<Projectile>> shots;
  vector<unique_ptr<Animation>> animations;

  bool isInBounds(Vector2 *position) {
    return position->x >= 0 && position->x < columnCount && position->y >= 0 &&
           position->y < rowCount;
  }

  bool isWall(Vector2 *position) {
    auto wallType = walls[position->x][position->y]->getType();
    return wallType != WallType::Empty && wallType != WallType::Net;
  }

  bool isTank(Vector2 *position) {
    for (auto &tank : tanks) {
      if (tank->getIsAlive() && tank->getX() == position->x &&
          tank->getY() == position->y) {
        return true;
      }
    }

    return false;
  }

  bool spawnAnimation(AnimationMeta *animationMeta, Vector2 postion) {
    for (auto &animation : animations) {
      if (!animation->isPlaying()) {
        animation->play(animationMeta, postion);

        return true;
      }
    }

    return false;
  }

public:
  void init() {
    columnCount = COLUMN_COUNT;
    rowCount = ROW_COUNT;
    cellSize = CELLS_SIZE;
    int maxProjectileCount = 25;

    int h = rowCount * cellSize;
    int w = columnCount * cellSize;

    buffer = new Bitmap(w, h);
    graphics = new Graphics(buffer);
    graphics->SetSmoothingMode(SmoothingModeNone);

    spriteManager =
        new SpriteManager(Bitmap::FromFile(L"textures\\tanks.png"), cellSize);
    walls = generateWalls(columnCount, rowCount);
    tanks = vector<shared_ptr<Tank>>();
    shots = vector<unique_ptr<Projectile>>();
    animations = vector<unique_ptr<Animation>>();

    for (int i = 0; i < maxProjectileCount; i++) {
      shots.push_back(unique_ptr<Projectile>(
          new Projectile({-1, -1}, LookDirection::Down)));
    }
    for (int i = 0; i < maxProjectileCount; i++) {
      animations.push_back(unique_ptr<Animation>(new Animation()));
    }

    auto tank1 = shared_ptr<Tank>(new Tank(3, 0));
    tank1->setSpawn(0, 0, LookDirection::Down);
    tank1->setControls(MOVEMENT_CONTROLS_1);
    tank1->setFrames(TANK_A_FRAMES);
    tank1->setFireControl(FIRE_CONTROL_1);
    tank1->spawn();
    tanks.push_back(tank1);

    auto tank2 = shared_ptr<Tank>(new Tank(3, 0));
    tank2->setSpawn(columnCount - 1, rowCount - 1, LookDirection::Top);
    tank2->setControls(MOVEMENT_CONTROLS_2);
    tank2->setFrames(TANK_B_FRAMES);
    tank2->setFireControl(FIRE_CONTROL_2);
    tank2->spawn();
    tanks.push_back(tank2);
  }

  void reset() {
    walls = generateWalls(columnCount, rowCount);

    for (auto &tank : tanks) {
      tank->spawn();
    }
  }

  void update() {
    for (auto &tank : tanks) {
      if (!tank->getIsAlive()) {
        if (isButtonPressed(tank->getFireControl())) {
          tank->spawn();
          this->spawnAnimation(&SPAWN_ANIMATION, tank->getPosition());
        }

        continue;
      }

      auto controls = tank->getControls();
      LookDirection direction = getTankDirection(controls);

      if (direction != LookDirection::None) {
        Vector2 newPosition =
            getPositionInDirection(tank->getPosition(), direction);

        if (isInBounds(&newPosition) && !isWall(&newPosition) &&
            !isTank(&newPosition)) {
          tank->setPosition(newPosition);
        }

        tank->setDirection(direction);
      }

      if (isButtonPressed(tank->getFireControl())) {
        auto position = tank->getPosition();
        auto direction = tank->getDirection();

        for (auto &shot : shots) {
          if (shot->getIsDestroyed()) {
            shot->setPosition(position);
            shot->setDirection(direction);
            shot->setIsDestroyed(false);

            break;
          }
        }
      }
    }

    for (auto &shot : shots) {
      if (shot->getIsDestroyed()) {
        continue;
      }

      auto position = shot->getPosition();
      auto direction = shot->getDirection();
      auto newPosition = getPositionInDirection(position, direction);

      if (!isInBounds(&newPosition)) {
        shot->setIsDestroyed(true);
        continue;
      }

      if (isWall(&newPosition)) {
        auto wall = walls[newPosition.x][newPosition.y].get();
        wall->applyDamage();

        shot->setIsDestroyed(true);
        this->spawnAnimation(&EXPLOSION_ANIMATION, newPosition);
        continue;
      }

      if (isTank(&newPosition)) {
        for (auto &tank : tanks) {
          if (tank->getPosition().x == newPosition.x &&
              tank->getPosition().y == newPosition.y) {
            tank->applyDamage();
            this->spawnAnimation(&EXPLOSION_ANIMATION, newPosition);

            break;
          }
        }

        shot->setIsDestroyed(true);
        continue;
      }

      shot->setPosition(newPosition);
    }
  }

  void draw(HDC hdc) {
    graphics->Clear(Color::Black);

    for (auto &tank : tanks) {
      if (!tank->getIsAlive()) {
        continue;
      }

      LookDirection direction = tank->getDirection();
      TextureCoordinates *sprite = &tank->getFrames()[direction];

      spriteManager->draw(graphics, sprite, tank->getX(), tank->getY());
    }

    for (auto &shot : shots) {
      if (shot->getIsDestroyed()) {
        continue;
      }

      LookDirection direction = shot->getDirection();
      TextureCoordinates *sprite = &SHELL_FRAMES[direction];

      spriteManager->draw(graphics, sprite, shot->getPosition().x,
                          shot->getPosition().y);
    }

    for (auto &wallRow : walls) {
      for (auto &wall : wallRow) {
        auto wallType = wall->getType();
        if (wallType == WallType::Empty) {
          continue;
        }

        spriteManager->draw(graphics, getWallTexture(wallType), wall->getX(),
                            wall->getY());
      }
    }

    for (auto &animation : animations) {
      if (animation->isPlaying()) {
        auto position = animation->getPosition();
        spriteManager->draw(graphics, animation->getFrame(), position.x,
                            position.y);
        animation->tick();
      }
    }

    // Render back graphics into the window
    Graphics windowGraphics(hdc);
    windowGraphics.DrawImage(buffer, 0, 0);
  }
};
