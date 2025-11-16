#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"
struct TerrainItem{
    int x = 0;
    int y = 0;
    Ref<Texture>* texture;

    TerrainItem(){};
    TerrainItem(int x, int y, Ref<Texture>* texture){
        this->x = x;
        this->y = y;
        this->texture = texture;
	}
};
struct Chunk{ //egy chunk 6x6 darab 200 x 200 pixel-es részbõl van
    int x = 0;
	int y = 0;
	List<TerrainItem> terrainItems = {};

    Chunk() {}
	Chunk(int x, int y, List<TerrainItem> terrainItems){
        this->x = x;
        this->y = y;
        this->terrainItems = terrainItems;
	}
};

class TestScene : public Scene{
    SFW_OBJECT(TestScene, Scene);

public:
	virtual void input_event(const Ref<InputEvent> &event);
	virtual void update(float delta);
	virtual void render();
	virtual void GenerateChunk(int startX, int startY);
	static void thread_func(void *p_user_data);

    TestScene();

    Mutex mtx;
    List<Chunk> chunks;
    int camX;
    int camY;
    int playerSpeed;
    int chunkBlockSize;
    bool start = false; //lefutott-e az update elején egyszer egy start, ez csak most kell talán késöbb nem
    RandomPCG rndStart;

    bool pressW = false;
    bool pressS = false;
    bool pressA = false;
    bool pressD = false;

    //debug
    bool showChunkBorder = false;
    bool showTerrainItemBorder = false;

protected:
    Ref<Font> _font;
    Ref<Image> _grassImage;
    Ref<Texture> _grassTexture;
    Ref<Image> _rockImage1;
    Ref<Texture> _rockTexture1;
    Ref<Image> _rockImage2;
    Ref<Texture> _rockTexture2;
    Ref<Image> _rockImage3;
    Ref<Texture> _rockTexture3;
    Ref<Image> _rockImage4;
    Ref<Texture> _rockTexture4;

    List<Ref<Texture>> _rockTextures;

};


struct ThreadData {
    int startX;
    int startY;
    TestScene* scene;
};
#endif
