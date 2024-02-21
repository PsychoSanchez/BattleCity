module;

#include <windows.h>
#include <tchar.h>

export module player;

import std.core;
import std.memory;

import constants;
import transform;
import spritemanager;

export class Player
{
private:
    bool isAlive = false;
    int x = 0;
    int y = 0;
    int spawnX = 0;
    int spawnY = 0;
    int health = 0;
    int armor = 0;
    int initialHealth;
    int initialArmor;
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
        x = spawnX;
        y = spawnY;
        direction = spawnDirection;
        health = initialHealth;
        armor = initialArmor;
        isAlive = true;
    }

    bool getIsAlive() { return isAlive; }

    bool applyDamage() {
        if (armor > 0) {
            armor--;
        }
        else {
            health--;
        }

        if (health <= 0) {
            // Kill the tank
            isAlive = false;
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
};

