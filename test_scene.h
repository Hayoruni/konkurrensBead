#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"

enum class CameraState{
    Player,
    Camera,
    Both
};
struct Projectile{
    float x = 0;
    float y = 0;
    float targetX = 0;
    float targetY = 0;
    int eSize = 0;
    int offsetX = 0;
    int offsetY = 0;
    float lifeTime = 0.0f;
    int speed = 0;

    Projectile(){};
    Projectile(float x, float y, int eSize, int offsetX, int offsetY, float targetX, float targetY, float lifeTime, int speed){
        this->x = x;
        this->y = y;
        this->eSize = eSize;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        this->targetX = targetX;
        this->targetY = targetY;
        this->lifeTime = lifeTime;
        this->speed = speed;
	}
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
    float x = 0;
    float y = 0;
    int eSize = 0;
    int offsetX = 0;
    int offsetY = 0;
    float hp = 0;
    float attackElapsed = 0;
    float attackSpeed = 0;
    int speed = 0;
    int damage = 0;

    Player(){};
    Player(float x, float y, int eSize, int offsetX, int offsetY, float hp, int speed,float attackSpeed, int damage){
        this->x = x;
        this->y = y;
        this->eSize = eSize;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        this->hp = hp;
        this->speed = speed;
        this->attackSpeed = attackSpeed;
        this->attackElapsed = attackSpeed;
        this->damage = damage;
	}
};

struct Enemy{
    float x = 0;
    float y = 0;
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
    Enemy(float x, float y, int eSize, int offsetX, int offsetY, int hp, int speed, int damage){
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
	virtual void CheckCollisionProjectile();
	virtual void PlayerAttack();

    TestScene();

    Mutex mtx;
    List<Chunk> chunks;
    Player player;
    List<Enemy> enemies;
    List<Projectile> projectiles;
    float camX;
    float camY;
    int chunkBlockSize;
    int charOffsetX;
    int charOffsetY;
    int chunkOffsetX;
    int chunkOffsetY;
    float score;
    bool start = false; //lefutott-e az update elején egyszer egy start, ez csak most kell talán késöbb nem
    CameraState camState;
    float enemySpawnTimer;
    float prePlayerX;
    float prePlayerY;
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

    int index;
    int indey;

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
    Ref<Image> _projectileImage;
    Ref<Texture> _projectileTexture;

};


struct ThreadData {
    int startX;
    int startY;
    TestScene* scene;
};
#endif
