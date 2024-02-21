module;

#include <windows.h>
#include <tchar.h>

export module projectile;

import transform;

export class Projectile {
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