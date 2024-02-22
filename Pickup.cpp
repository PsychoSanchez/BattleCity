module;

export module pickup;

import transform;

export enum PickupType {
    Health,
    Armor
};

export class Pickup {
private:
    PickupType type = PickupType::Armor;
    Vector2 position = {-1, -1};
    bool isSpawned = false;

public:
    void setType(PickupType type) {
        this->type = type;
    }

    PickupType getType() {
        return this->type;
    }

    void setPosition(Vector2 position) {
        this->position = position;
    }

    Vector2 getPosition() {
        return this->position;
    }

    bool getIsSpawned() {
        return this->isSpawned;
    }

    void setIsSpawned(bool isSpawned) {
        this->isSpawned = isSpawned;
    }
};
