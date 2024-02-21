module;

#include <windows.h>
#include <tchar.h>
#include "gdiplus.h"

export module game;

import std.core;
import std.memory;

import projectile;
import wall;
import player;
import animation;
import transform;
import constants;
import spritemanager;

using namespace Gdiplus;
using namespace std;

export bool isButtonPressed(int buttonKeyCode) {
    return GetKeyState(buttonKeyCode) < 0;
}

export LookDirection getTankDirection(int* keys) {
    for (int i = 0; i < 4; i++) {
        if (isButtonPressed(keys[i])) {
            return CONTROL_TO_LOOK_DIRECTION_MAP[i];
        }
    }

    return None;
}

export class GameManager {
private:
    int columnCount, rowCount, cellSize;
    Bitmap* buffer;
    Graphics* graphics;
    SpriteManager* spriteManager;
    vector<vector<unique_ptr<Wall>>> walls;
    vector<shared_ptr<Player>> tanks;
    vector<unique_ptr<Projectile>> shots;
    vector<unique_ptr<Animation>> animations;

    bool isInBounds(Vector2* position) {
        return position->x >= 0 && position->x < columnCount && position->y >= 0 &&
            position->y < rowCount;
    }

    bool isWall(Vector2* position) {
        auto wallType = walls[position->x][position->y]->getType();
        return wallType != WallType::Empty && wallType != WallType::Net;
    }

    bool isTank(Vector2* position) {
        for (auto& tank : tanks) {
            if (tank->getIsAlive() && tank->getX() == position->x &&
                tank->getY() == position->y) {
                return true;
            }
        }

        return false;
    }

    bool spawnAnimation(AnimationMeta* animationMeta, Vector2 postion) {
        for (auto& animation : animations) {
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
        walls = GenerateWalls(columnCount, rowCount);
        tanks = vector<shared_ptr<Player>>();
        shots = vector<unique_ptr<Projectile>>();
        animations = vector<unique_ptr<Animation>>();

        for (int i = 0; i < maxProjectileCount; i++) {
            shots.push_back(unique_ptr<Projectile>(
                new Projectile({ -1, -1 }, LookDirection::Down)));
        }
        for (int i = 0; i < maxProjectileCount; i++) {
            animations.push_back(unique_ptr<Animation>(new Animation()));
        }

        auto tank1 = shared_ptr<Player>(new Player(3, 0));
        tank1->setSpawn(0, 0, LookDirection::Down);
        tank1->setControls(MOVEMENT_CONTROLS_1);
        tank1->setFrames(TANK_A_FRAMES);
        tank1->setFireControl(FIRE_CONTROL_1);
        tank1->spawn();
        tanks.push_back(tank1);

        auto tank2 = shared_ptr<Player>(new Player(3, 0));
        tank2->setSpawn(columnCount - 1, rowCount - 1, LookDirection::Top);
        tank2->setControls(MOVEMENT_CONTROLS_2);
        tank2->setFrames(TANK_B_FRAMES);
        tank2->setFireControl(FIRE_CONTROL_2);
        tank2->spawn();
        tanks.push_back(tank2);
    }

    void reset() {
        walls = GenerateWalls(columnCount, rowCount);

        for (auto& tank : tanks) {
            tank->spawn();
        }
    }

    void update() {
        for (auto& tank : tanks) {
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
                    GetPositionInDirection(tank->getPosition(), direction);

                if (isInBounds(&newPosition) && !isWall(&newPosition) &&
                    !isTank(&newPosition)) {
                    tank->setPosition(newPosition);
                }

                tank->setDirection(direction);
            }

            if (isButtonPressed(tank->getFireControl())) {
                auto position = tank->getPosition();
                auto direction = tank->getDirection();

                for (auto& shot : shots) {
                    if (shot->getIsDestroyed()) {
                        shot->setPosition(position);
                        shot->setDirection(direction);
                        shot->setIsDestroyed(false);

                        break;
                    }
                }
            }
        }

        for (auto& shot : shots) {
            if (shot->getIsDestroyed()) {
                continue;
            }

            auto position = shot->getPosition();
            auto direction = shot->getDirection();
            auto newPosition = GetPositionInDirection(position, direction);

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
                for (auto& tank : tanks) {
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

        for (auto& tank : tanks) {
            if (!tank->getIsAlive()) {
                continue;
            }

            LookDirection direction = tank->getDirection();
            TextureCoordinates* sprite = &tank->getFrames()[direction];

            spriteManager->draw(graphics, sprite, tank->getX(), tank->getY());
        }

        for (auto& shot : shots) {
            if (shot->getIsDestroyed()) {
                continue;
            }

            LookDirection direction = shot->getDirection();
            TextureCoordinates* sprite = &SHELL_FRAMES[direction];

            spriteManager->draw(graphics, sprite, shot->getPosition().x,
                shot->getPosition().y);
        }

        for (auto& wallRow : walls) {
            for (auto& wall : wallRow) {
                auto wallType = wall->getType();
                if (wallType == WallType::Empty) {
                    continue;
                }

                spriteManager->draw(graphics, GetWallTexture(wallType), wall->getX(),
                    wall->getY());
            }
        }

        for (auto& animation : animations) {
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
