#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"

enum class CameraState{
    Player,
    Camera,
    Both
};

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
struct Player{
    int x = 0;
    int y = 0;
    int eSize = 0;
    int hp = 0;
    int speed = 0;
    int damage = 0;

    Player(){};
    Player(int x, int y, int eSize, int hp, int speed, int damage){
        this->eSize = eSize;
        this->x = x;
        this->y = y;
        this->hp = hp;
        this->speed = speed;
        this->damage = damage;
	}
};

struct Enemy{
    int x = 0;
    int y = 0;
    int eSize = 0;
    int hp = 0;
    int speed = 0;
    int damage = 0;

    Enemy(){};
    Enemy(int x, int y, int eSize, int hp, int speed, int damage){
        this->eSize = eSize;
        this->x = x;
        this->y = y;
        this->hp = hp;
        this->speed = speed;
        this->damage = damage;
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
    Player player;
    int camX;
    int camY;
    int chunkBlockSize;
    int playerOffsetX;
    int playerOffsetY;
    int chunkOffsetX;
    int chunkOffsetY;
    bool start = false; //lefutott-e az update elején egyszer egy start, ez csak most kell talán késöbb nem
    RandomPCG rndStart;
    CameraState camState;

    bool pressW = false;
    bool pressS = false;
    bool pressA = false;
    bool pressD = false;

    //debug
    bool showChunkBorder = false;
    bool showBlockBorder = false;
    bool showTerrainItemBorder = false;
    bool showCenterLine = false;

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

    Ref<Image> _playerImage;
    Ref<Texture> _playerTexture;
    Ref<Image> _enemyImage;
    Ref<Texture> _enemyTexture;

};


struct ThreadData {
    int startX;
    int startY;
    TestScene* scene;
};
#endif
