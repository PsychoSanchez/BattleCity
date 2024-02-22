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
import pickup;

using namespace Gdiplus;
using namespace std;

export class GameManager {
private:
    bool isGameOver = false;
    int columnCount, rowCount, cellSize, scoreboardHeight = 16, maxProjectileCount = 50, maxPickupCount = 5;
    int width, height;
    
    Bitmap* buffer;
    Graphics* graphics;
    Bitmap* gameOverScreen;

    unique_ptr<SpriteManager> spriteManager;
    vector<vector<unique_ptr<Wall>>> walls;
    vector<shared_ptr<Player>> tanks;
    vector<unique_ptr<Projectile>> shots;
    vector<unique_ptr<Animation>> animations;
    vector<unique_ptr<Pickup>> pickups;
    
    DWORD gameTickInterval = GAME_TICK_INTERVAL;
    DWORD lastGameTickUpdate = 0;
    DWORD lastDrawUpdate = 0;

    bool isInBounds(Vector2* position) {
        return position->x >= 0 && position->x < columnCount && position->y >= 0 &&
            position->y < rowCount;
    }

    bool isWall(Vector2* position) {
        auto wallType = walls[position->x][position->y]->getType();
        return wallType != WallType::Empty && wallType != WallType::Net;
    }

    shared_ptr<Player> getAliveTankFromPosition(Vector2 position) {
        for (auto& tank : tanks) {
            if (tank->getIsAlive() && tank->getX() == position.x &&
                tank->getY() == position.y) {
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

    void drawScorePanel(DWORD tick) {
        SolidBrush grayBrush(Color::Gray);
        int expectedTextLength = 125;
		int scoreRenderCoords[4][2] = { 
            {0,0}, 
            {columnCount * cellSize - expectedTextLength, rowCount * cellSize + scoreboardHeight},
            {0, rowCount * cellSize + scoreboardHeight}, 
            {columnCount * cellSize - expectedTextLength, 0}
        };
        
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
        
        wss.str(L"");
        wss << L"FPS: ";
        wss << 1000 / double(tick - this->lastDrawUpdate);
        graphics->DrawString(wss.str().c_str(), wss.str().length(), &font, PointF(expectedTextLength * 2, 0), &grayBrush); 

        // TODO: Render game time (top-center/bottom-center)
    }

public:
    void init() {
        columnCount = COLUMN_COUNT;
        rowCount = ROW_COUNT;
        cellSize = CELLS_SIZE;

        height = (rowCount + 1) * cellSize + scoreboardHeight;
        width = columnCount * cellSize;

        buffer = new Bitmap(width, height);
        graphics = new Graphics(buffer);
        gameOverScreen = Bitmap::FromFile(L"textures\\gameover.png");
        graphics->SetSmoothingMode(SmoothingModeNone);

        spriteManager =
            unique_ptr<SpriteManager>(new SpriteManager(Bitmap::FromFile(L"textures\\tanks.png"), cellSize));
        walls = GenerateWalls(columnCount, rowCount);
        tanks = vector<shared_ptr<Player>>();
        shots = vector<unique_ptr<Projectile>>();
        animations = vector<unique_ptr<Animation>>();
        pickups = vector<unique_ptr<Pickup>>();

        for (int i = 0; i < maxProjectileCount; i++) {
            shots.push_back(unique_ptr<Projectile>(
                new Projectile({ -1, -1 }, LookDirection::Down)));
        }

        for (int i = 0; i < maxProjectileCount; i++) {
            animations.push_back(unique_ptr<Animation>(new Animation()));
        }

        for (int i = 0; i < maxPickupCount; i++) {
            pickups.push_back(unique_ptr<Pickup>(new Pickup()));
        }

        // TODO: Randomize player spawn positions 
        auto tank1 = shared_ptr<Player>(new Player(&PLAYER_1_CONFIG));
        tank1->setSpawn(0, 0, LookDirection::Down);
        tank1->spawn();
        tanks.push_back(tank1);

        auto tank2 = shared_ptr<Player>(new Player(&PLAYER_2_CONFIG));
        tank2->setSpawn(columnCount - 1, rowCount - 1, LookDirection::Top);
        tank2->spawn();
        tanks.push_back(tank2);

        auto tank3 = shared_ptr<Player>(new Player(&PLAYER_3_CONFIG));
        tank3->setSpawn(0, rowCount - 1, LookDirection::Top);
        tank3->spawn();
        tanks.push_back(tank3);

        auto tank4 = shared_ptr<Player>(new Player(&PLAYER_4_CONFIG));
        tank4->setSpawn(columnCount - 1, 0, LookDirection::Top);
        tank4->spawn();
        tanks.push_back(tank4);
    }

    int getFirstAliveTankId() {
        for (auto &tank : tanks) {
            if (tank->getLiveCount() > 0) {
                return tank->getId();
            }
        }

        return -1;
    }

    void reset() {
        walls = GenerateWalls(columnCount, rowCount);

        for (auto& tank : tanks) {
            tank->reset();
        }

        isGameOver = false;
    }

    void spawnPickup(PickupType type) {
        for (auto& pickup : pickups) {
            if (!pickup->getIsSpawned()) {
                while (true) {
                    int x = rand() % (columnCount - 1);
                    int y = rand() % (rowCount - 1);
                    auto& wall = walls[x][y];
                    
                    if (wall->getType() == WallType::Empty) {
                        pickup->setPosition(wall->getPosition());
                        pickup->setType(type);
                        pickup->setIsSpawned(true);
                        break;
                    }
                }

                break;
            }
        }
    }

    void spawnHealthPickup() {
        this->spawnPickup(PickupType::Health);
    }

    void spawnArmorPickup() {
        this->spawnPickup(PickupType::Armor);
    }

    void update() {
        if (isGameOver) {
            return;
        }

        for (auto& tank : tanks) {
            tank->processControls();
        }

        auto tick = GetTickCount();
        if (tick - this->lastGameTickUpdate < this->gameTickInterval) {
            return;
        }

        this->lastGameTickUpdate = tick;

        for (auto& tank : tanks) {
            if (!tank->getIsAlive()) {
                if (tank->canSpawn() && tank->getIsFireButtonPressed()) {
                    tank->spawn();
                    this->spawnAnimation(&SPAWN_ANIMATION, tank->getPosition());
                }

                continue;
            }

            LookDirection direction = tank->getPressedMovementKey();

            if (direction != LookDirection::None) {
                Vector2 newPosition =
                    GetPositionInDirection(tank->getPosition(), direction);
                
                if (isInBounds(&newPosition) && !isWall(&newPosition) &&
                    getAliveTankFromPosition(newPosition) == nullptr) {
                    tank->setPosition(newPosition);
                }  else {
                    tank->setPosition(tank->getPosition());
                }

                tank->setDirection(direction);
            } else {
                tank->setPosition(tank->getPosition());
            }

            if (tank->getIsFireButtonPressed() && tank->isShotAvailable(tick)) {
                auto position = tank->getPosition();
                auto direction = tank->getDirection();
                auto playerId = tank->getId();

                // Find first available shot from the pool
                for (auto& shot : shots) {
                    if (shot->getIsDestroyed()) {
                        shot->setPlayerId(playerId);
                        shot->setPosition(position);
                        shot->setDirection(direction);
                        shot->setIsDestroyed(false);

                        tank->setLastShotTick(tick);

                        break;
                    }
                }
            }

            tank->tick();
        }

        for (int i = 0; i < shots.size(); i++) {
            auto& shot = shots[i];
            if (shot->getIsDestroyed()) {
                continue;
            }

            auto position = shot->getPosition();
            auto direction = shot->getDirection();
            auto newPosition = GetPositionInDirection(position, direction);
            auto tank = this->getAliveTankFromPosition(newPosition);

            if (!isInBounds(&newPosition)) {
                shot->setIsDestroyed(true);
            } else if (isWall(&newPosition)) {
                auto wall = walls[newPosition.x][newPosition.y].get();
                wall->applyDamage();

                shot->setIsDestroyed(true);
                this->spawnAnimation(&EXPLOSION_ANIMATION, newPosition);
            } else if (tank != nullptr) {
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
            } else {
                // Check against other shots 
                for (int j = i + 1; j < shots.size(); j++) {
                    auto& secondShot = shots[j];
                    if (secondShot->getIsDestroyed()) {
                        continue;
                    }

                    auto secondShotPosition = secondShot->getPosition();
                    
                    if ((secondShotPosition.x == newPosition.x && secondShotPosition.y == newPosition.y) || (
                        secondShotPosition.x == position.x && secondShotPosition.y == position.y
                    )) {
                        secondShot->setIsDestroyed(true);
                        shot->setIsDestroyed(true);
                        this->spawnAnimation(&EXPLOSION_ANIMATION, newPosition);
                        break;                    
                    }
                }
            }

            if (shot->getIsDestroyed()) {
                continue;
            }

            shot->setPosition(newPosition);
        }
        
        for (auto& pickup : pickups) {
            if (!pickup->getIsSpawned()) {
                continue;
            }

            auto tank = getAliveTankFromPosition(pickup->getPosition());
            if (tank == nullptr) {
                continue;
            }
            
            switch (pickup->getType()) {
                case PickupType::Health:
                    tank->addHealth();
                    break;
                case PickupType::Armor:
                    tank->addArmor();
                    break;
            }

            pickup->setIsSpawned(false);
        }
        
        int aliveTanks = 0;
        for (auto &tank : tanks) {
            aliveTanks += tank->getLiveCount() > 0 ? 1 : 0;
        }

        this->isGameOver = aliveTanks < 2;
    }

    void draw(HDC hdc) {
        auto tick = GetTickCount();

        // Render back graphics into the window
        Graphics windowGraphics(hdc);
        
        if (isGameOver) {
            graphics->Clear(Color::Black);
            int padding = 16;
            graphics->DrawImage(gameOverScreen, REAL(padding), REAL(padding), REAL(width - padding * 2), REAL(height - padding * 2));

            int winnerId = this->getFirstAliveTankId();
            SolidBrush grayBrush(Color::Gray);
            Font font(L"Verdana", 32, FontStyleBold);
            wstringstream wss;
            wss.str(L"");
            wss << L"PLAYER ";
            wss << winnerId;
            wss << L" WINS";
            graphics->DrawString(wss.str().c_str(), wss.str().length(), &font, PointF(this->width / 2, this->height / 2), &grayBrush); 

            windowGraphics.DrawImage(buffer, 0, 0);
            
            return;
        }

        graphics->Clear(Color::Black);

        for (auto& tank : tanks) {
            if (!tank->getIsAlive()) {
                continue;
            }

            LookDirection direction = tank->getDirection();
            int shift = tank->getIsArmored() ? 4 : 0; 
            TextureCoordinates* sprite = &tank->getFrames()[shift + direction];
            auto tickPercent = double(tick - this->lastGameTickUpdate) / double(this->gameTickInterval);
            auto clampedTickPercent = tickPercent > 1 ? 1 : tickPercent < 0 ? 0 : tickPercent;
            
            auto tankCurrentPosition = tank->getPosition();
            auto tankPrevPosition = tank->getPrevPosition();
            
            auto tankX = REAL(tankPrevPosition.x) + REAL(tankCurrentPosition.x - tankPrevPosition.x) * clampedTickPercent;
            auto tankY = REAL(tankPrevPosition.y) + REAL(tankCurrentPosition.y - tankPrevPosition.y) * clampedTickPercent;

            spriteManager->draw(graphics, sprite, tankX, tankY);
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

        for (auto& pickup : pickups) {
            if (pickup->getIsSpawned()) {
                auto type = pickup->getType();
                auto position = pickup->getPosition();
                auto texture = type == PickupType::Health ? &HEALTH_PICKUP_META : &ARMOR_PICKUP_META;
                
                spriteManager->draw(graphics, texture, position.x, position.y);
            }
        }

        this->drawScorePanel(tick);

        windowGraphics.DrawImage(buffer, 0, 0);

        this->lastDrawUpdate = tick;
    }
};
