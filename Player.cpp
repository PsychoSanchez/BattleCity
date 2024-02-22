module;

#include <windows.h>
#include <tchar.h>

export module player;

import std.core;
import std.memory;

import constants;
import transform;
import spritemanager;

export struct PlayerConfig {
    int id;
    int health;
    int armor;
    int liveCount;
    TextureCoordinates *frames;
    int* movementControls;
    int fireControl;
};

export PlayerConfig PLAYER_1_CONFIG = {
    1,
    3,
    0,
    15,
    TANK_A_FRAMES,
    MOVEMENT_CONTROLS_1,
    FIRE_CONTROL_1
};

export PlayerConfig PLAYER_2_CONFIG = {
    2,
    3,
    0,
    15,
    TANK_B_FRAMES,
    MOVEMENT_CONTROLS_2,
    FIRE_CONTROL_2
};

export class Player
{
private:
    bool isAlive = false;
    int id = 0;
    int x = 0;
    int y = 0;
    int spawnX = 0;
    int spawnY = 0;
    int health = 0;
    int armor = 0;
    int liveCount = 0;
    int killCount = 0;
    int initialHealth;
    int initialArmor;
    int initialLiveCount;
    LookDirection direction;
    LookDirection spawnDirection = LookDirection::Down;
    int* controls;
    int fireControl;
    TextureCoordinates* frames;

public:
    Player(int health, int armor) {
        this->initialHealth = health;
        this->initialArmor = armor;
    }

    Player(PlayerConfig* config) {
        this->id = config->id;
        this->initialHealth = config->health;
        this->initialArmor = config->armor;
        this->frames = config->frames;
        this->controls = config->movementControls;
        this->fireControl = config->fireControl;
        this->initialLiveCount = config->liveCount;
        this->liveCount = config->liveCount;
    }

    void setSpawn(int x, int y, LookDirection direction) {
        spawnX = x;
        spawnY = y;
        spawnDirection = direction;
    }

    void setControls(int* controls) { this->controls = controls; }
    int* getControls() { return controls; }
    void setFrames(TextureCoordinates* frames) { this->frames = frames; }
    TextureCoordinates* getFrames() { return frames; }
    void setFireControl(int control) { this->fireControl = control; }
    int getFireControl() { return fireControl; }

    void spawn() {
        if (this->liveCount <= 0) {
            return;
        }

        x = spawnX;
        y = spawnY;
        direction = spawnDirection;
        health = initialHealth;
        armor = initialArmor;
        isAlive = true;
    }

    void reset() {
        this->liveCount = this->initialLiveCount;
        this->killCount = 0;
        this->spawn();
    }

    bool getIsAlive() { return isAlive; }

    // Returns isAlive
    bool applyDamage() {
        if (armor > 0) {
            armor--;
        }
        else {
            health--;
        }

        if (health <= 0) {
            isAlive = false;
            liveCount--;
        }

        return isAlive;
    }

    int getX() { return x; }
    int getY() { return y; }

    void setPosition(Vector2 position) {
        this->x = position.x;
        this->y = position.y;
    }
    Vector2 getPosition() { return { x, y }; }

    void setDirection(LookDirection direction) { this->direction = direction; }
    LookDirection getDirection() { return direction; }

    int getId() {return this->id;}
    
    void increaseKillCount() {
        this->killCount++;
    }
    
    int getKillCount() {
        return this->killCount;
    }

    int getLiveCount() {
        return this->liveCount;
    }
};

