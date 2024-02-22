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
    bool isGameOver = false;
    int columnCount, rowCount, cellSize, scoreboardHeight = 16;
    int width, height;
    Bitmap* buffer;
    Graphics* graphics;
    Bitmap* gameOverScreen;
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

    shared_ptr<Player> getAliveTankFromPosition(Vector2* position) {
        for (auto& tank : tanks) {
            if (tank->getIsAlive() && tank->getX() == position->x &&
                tank->getY() == position->y) {
                return tank;
            }
        }

        return nullptr;
    }

    shared_ptr<Player> getTankById(int id) {
        for (auto& tank : tanks) {
            if (tank->getId() == id) {
                return tank;
            }
        }

        return nullptr;
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

    void drawScorePanel() {
        SolidBrush grayBrush(Color::Gray);
        int expectedTextLength = 125;
        int scoreRenderCoords[4][2] = {{0,0}, {columnCount * cellSize - expectedTextLength, 0}, {0, rowCount * cellSize - scoreboardHeight}, {columnCount * cellSize - expectedTextLength, rowCount * cellSize - scoreboardHeight}};
        
        Font font(L"Verdana", 10, FontStyleBold);
        wstringstream wss;
        
        for (int i = 0; i < this->tanks.size(); i++) {
            wss.str(L"");
            wss << L"Kills: ";
            wss << tanks[i]->getKillCount();
            wss << L" Lives : ";
            wss << tanks[i]->getLiveCount();
            auto position = scoreRenderCoords[i];
            PointF point(position[0], position[1]);
            graphics->DrawString(wss.str().c_str(), wss.str().length(), &font, point, &grayBrush); 
        }

        // TODO: Render game time (top-center/bottom-center)
    }

public:
    void init() {
        columnCount = COLUMN_COUNT;
        rowCount = ROW_COUNT;
        cellSize = CELLS_SIZE;
        int maxProjectileCount = 25;

        height = (rowCount + 1) * cellSize + scoreboardHeight;
        width = columnCount * cellSize;

        buffer = new Bitmap(width, height);
        graphics = new Graphics(buffer);
        gameOverScreen = Bitmap::FromFile(L"textures\\gameover.png");
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

        auto tank1 = shared_ptr<Player>(new Player(&PLAYER_1_CONFIG));
        tank1->setSpawn(0, 0, LookDirection::Down);
        tank1->spawn();
        tanks.push_back(tank1);

        auto tank2 = shared_ptr<Player>(new Player(&PLAYER_2_CONFIG));
        tank2->setSpawn(columnCount - 1, rowCount - 1, LookDirection::Top);
        tank2->spawn();
        tanks.push_back(tank2);
    }

    void reset() {
        walls = GenerateWalls(columnCount, rowCount);

        for (auto& tank : tanks) {
            tank->reset();
        }

        isGameOver = false;
    }

    void update() {
        if (isGameOver) {
            return;
        }

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
                    getAliveTankFromPosition(&newPosition) == nullptr) {
                    tank->setPosition(newPosition);
                }

                tank->setDirection(direction);
            }

            if (isButtonPressed(tank->getFireControl())) {
                auto position = tank->getPosition();
                auto direction = tank->getDirection();
                auto playerId = tank->getId();

                for (auto& shot : shots) {
                    if (shot->getIsDestroyed()) {
                        shot->setPlayerId(playerId);
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

            auto tank = this->getAliveTankFromPosition(&newPosition);
            if (tank != nullptr) {
                this->spawnAnimation(&EXPLOSION_ANIMATION, newPosition);
                auto isAlive = tank->applyDamage();
                
                if (!isAlive) {
                    auto playerId = shot->getPlayerId();
                    auto creditedPlayer = this->getTankById(playerId);
                    
                    if (creditedPlayer != nullptr) {
                        creditedPlayer->increaseKillCount();
                    }
                }

                shot->setIsDestroyed(true);
                continue;
            }

            shot->setPosition(newPosition);
        }

        int aliveTanks = 0;
        for (auto &tank : tanks) {
            aliveTanks += tank->getLiveCount() > 0 ? 1 : 0;
        }

        this->isGameOver = aliveTanks < 2;
    }

    void draw(HDC hdc) {
        // Render back graphics into the window
        Graphics windowGraphics(hdc);
        
        if (isGameOver) {
            graphics->Clear(Color::Black);
            int padding = 16;
            graphics->DrawImage(gameOverScreen, REAL(padding), REAL(padding), REAL(width - padding * 2), REAL(height - padding * 2));
            windowGraphics.DrawImage(buffer, 0, 0);
            return;
        }

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

        this->drawScorePanel();

        windowGraphics.DrawImage(buffer, 0, 0);
    }
};
