module;

#include <windows.h>
#include <tchar.h>

export module wall;

import transform;
import constants;
import spritemanager;

import std.core;
import std.memory;

export enum WallType { Brick, Concrete, Steel, Net, Empty };

export class Wall {
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
    Vector2 getPosition() { return { x, y }; }
};

export bool isTankSpawnPosition(int x, int y) {
    for (auto point : SPAWN_POINTS) {
        if (x == point[0] && y == point[1]) {
            return true;
        }
    }

    return false;
}

export std::vector<std::vector<std::unique_ptr<Wall>>> GenerateWalls(int columnCount, int rowCount) {
    srand((unsigned)time(NULL));

    auto walls = std::vector<std::vector<std::unique_ptr<Wall>>>();

    for (int x = 0; x < columnCount; x++) {
        auto row = std::vector<std::unique_ptr<Wall>>();

        for (int y = 0; y < rowCount; y++) {
            int type =
                isTankSpawnPosition(x, y) ? 4 : rand() % 4;

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

            row.push_back(std::unique_ptr<Wall>(new Wall(x, y, wallType, CELLS_SIZE)));
        }

        walls.push_back(std::move(row));
    }

    return walls;
}


export TextureCoordinates* GetWallTexture(WallType type) {
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
