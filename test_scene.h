#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"

class Chunk{ //egy chunk 6x6 darab 200 x 200 pixel-es részbõl van

public:
	int x = 0;
	int y = 0;

    Chunk() {}
	Chunk(int x, int y){
        this->x = x;
        this->y = y;
	}
};

class TestScene : public Scene{
    SFW_OBJECT(TestScene, Scene);

public:
	virtual void input_event(const Ref<InputEvent> &event);
	virtual void update(float delta);
	virtual void render();
	void GenerateChunk(int startX, int startY);

    TestScene();

    List<Chunk> chunks;
    int camX;
    int camY;
    int playerSpeed;
    int chunkBlockSize;
    bool start = false; //lefutott-e az update elején egyszer egy start, ez csak most kell talán késöbb nem

    bool pressW = false;
    bool pressS = false;
    bool pressA = false;
    bool pressD = false;

    //debug
    bool showChunkBorder = false;

protected:
    Ref<Font> _font;
    Ref<Image> _grassImage;
    Ref<Texture> _grassTexture;
};
#endif
