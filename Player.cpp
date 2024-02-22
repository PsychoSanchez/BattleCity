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
    3,
    TANK_A_FRAMES,
    MOVEMENT_CONTROLS_1,
    FIRE_CONTROL_1
};

export PlayerConfig PLAYER_2_CONFIG = {
    2,
    3,
    0,
    3,
    TANK_B_FRAMES,
    MOVEMENT_CONTROLS_2,
    FIRE_CONTROL_2
};

export PlayerConfig PLAYER_3_CONFIG = {
    3,
    3,
    0,
    3,
    TANK_C_FRAMES,
    MOVEMENT_CONTROLS_3,
    FIRE_CONTROL_3
};

export PlayerConfig PLAYER_4_CONFIG = {
    4,
    3,
    0,
    3,
    TANK_D_FRAMES,
    MOVEMENT_CONTROLS_4,
    FIRE_CONTROL_4
};

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

export class Player
{
private:
    int id = 0;
    
    int x = 0;
    int y = 0;
    Vector2 prevPosition = {0, 0};
    LookDirection direction;
    
    bool isAlive = false;
    int health = 0;
    int armor = 0;
    
    int liveCount = 0;
    int killCount = 0;

    int spawnX = 0;
    int spawnY = 0;
    LookDirection spawnDirection = LookDirection::Down;

    int initialHealth;
    int initialArmor;
    int initialLiveCount;
    
    int* controls;
    int fireControl;
    TextureCoordinates* frames;

    DWORD lastShotTick = 0;
    int shotInterval = 500;

    bool isFireButtonPressed = false;
    LookDirection pressedDirection = LookDirection::None;

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
        this->prevPosition = {this->x, this->y};
        direction = spawnDirection;
        health = initialHealth;
        armor = initialArmor;
        isAlive = true;
    }

    bool canSpawn() {
        return this->getLiveCount() > 0;
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
        this->prevPosition = {this->x, this->y};
        this->x = position.x;
        this->y = position.y;
    }
    Vector2 getPosition() { return { x, y }; }
    Vector2 getPrevPosition() { return this->prevPosition; }

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

    bool isShotAvailable(DWORD tick) {
        return tick - this->shotInterval > this->lastShotTick;
    }

    void setLastShotTick(DWORD tick) {
        this->lastShotTick = tick;
    }

    void processControls() {
        this->isFireButtonPressed = this->isFireButtonPressed || isButtonPressed(this->getFireControl());
        auto newDirection = getTankDirection(this->getControls());
        
        if (newDirection != LookDirection::None) {
            this->pressedDirection = newDirection;
        }
    }

    LookDirection getPressedMovementKey() {
        return this->pressedDirection;
    }

    bool getIsFireButtonPressed() {
        return this->isFireButtonPressed;
    }

    void tick() {
        this->isFireButtonPressed = false;
        this->pressedDirection = LookDirection::None;
    }

    void addHealth() {
        if (this->health < 3) {
            this->health++;
        }
    }

    void addArmor() {
        this->armor++;
    }

    bool getIsArmored() {
        return this->armor > 0;
    }
};

