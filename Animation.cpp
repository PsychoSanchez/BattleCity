export module animation;

import transform;
import spritemanager;

export struct AnimationMeta {
    TextureCoordinates* frames;
    int length;
};

export AnimationMeta EXPLOSION_ANIMATION = {
    EXPLOSION_FRAMES,
    3
};

export AnimationMeta SPAWN_ANIMATION = {
    SPAWN_FRAMES,
    3
};

export class Animation {
private:
    int currentFrame = -1;
    AnimationMeta* meta;
    Vector2 position;

public:
    bool isPlaying() { return currentFrame != -1; }
    void play(AnimationMeta* meta, Vector2 position) {
        this->currentFrame = 0;
        this->meta = meta;
        this->position = position;
    }
    Vector2 getPosition() { return this->position; }
    TextureCoordinates* getFrame() { return &this->meta->frames[currentFrame]; }
    void tick() {
        currentFrame++;

        if (currentFrame >= this->meta->length) {
            currentFrame = -1;
        }
    }
};