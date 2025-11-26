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
    int eSize = 0;
    int offsetX = 0;
    int offsetY = 0;
    Ref<Texture>* texture;

    TerrainItem(){};
    TerrainItem(int x, int y, int eSize, int offsetX, int offsetY, Ref<Texture>* texture){
        this->x = x;
        this->y = y;
        this->eSize = eSize;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        this->texture = texture;
	}
};
struct Chunk{ //egy chunk 6x6 darab 200 x 200 pixel-es részbõl van
    int x = 0;
	int y = 0;
    int offsetX = 0;
    int offsetY = 0;
	List<TerrainItem> terrainItems = {};

    Chunk() {}
	Chunk(int x, int y, int offsetX, int offsetY, List<TerrainItem> terrainItems){
        this->x = x;
        this->y = y;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        this->terrainItems = terrainItems;
	}
};
struct Player{
    int x = 0;
    int y = 0;
    int eSize = 0;
    int offsetX = 0;
    int offsetY = 0;
    float hp = 0;
    int speed = 0;
    int damage = 0;

    Player(){};
    Player(int x, int y, int eSize, int offsetX, int offsetY, float hp, int speed, int damage){
        this->x = x;
        this->y = y;
        this->eSize = eSize;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        this->hp = hp;
        this->speed = speed;
        this->damage = damage;
	}
};

struct Enemy{
    int x = 0;
    int y = 0;
    int eSize = 0;
    int offsetX = 0;
    int offsetY = 0;
    int preMoveX = 0;
    int preMoveY = 0;
    int hp = 0;
    int speed = 0;
    int damage = 0;
    float aliveTime = 0; //TODO ezt torolni, csak debug

    Enemy(){};
    Enemy(int x, int y, int eSize, int offsetX, int offsetY, int hp, int speed, int damage){
        this->x = x;
        this->y = y;
        this->eSize = eSize;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
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
	virtual void GenGhost();
	virtual void CheckCollisionPlayer();
	virtual void CheckCollisionMoveEnemy();

    TestScene();

    Mutex mtx;
    List<Chunk> chunks;
    Player player;
    List<Enemy> enemies;
    int camX;
    int camY;
    int chunkBlockSize;
    int charOffsetX;
    int charOffsetY;
    int chunkOffsetX;
    int chunkOffsetY;
    float score;
    bool start = false; //lefutott-e az update elején egyszer egy start, ez csak most kell talán késöbb nem
    CameraState camState;
    float enemySpawnTimer;
    int prePlayerX;
    int prePlayerY;
    bool canMoveX;
    bool canMoveY;

    //engVar
    bool pressW = false;
    bool pressS = false;
    bool pressA = false;
    bool pressD = false;
    float scaledDelta = 0.0f;
    float timeScale = 1.0f;
    float fpsElapsed = 1.0f;
    int fps = 0;
    int fpsDisplay = 0;

    //debug
    bool showChunkBorder = false;
    bool showBlockBorder = false;
    bool showItemBorder = false;
    bool showCenterLine = false;
    bool canSpawnEnemy = false;

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
