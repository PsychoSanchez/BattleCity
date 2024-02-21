export module transform;

export enum LookDirection { Top, Right, Down, Left, None };

export struct Vector2 {
    int x, y;
};

export Vector2 GetPositionInDirection(Vector2 position, LookDirection direction) {
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