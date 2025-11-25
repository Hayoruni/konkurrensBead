#include "test_scene.h"

void TestScene::input_event(const Ref<InputEvent> &event){

    Ref<InputEventKey> iek = event;

    if(iek.is_valid()){
        if (iek->is_echo()) {
            return;
        }

        if(iek->is_released()){
            if(iek->get_physical_scancode() == KEY_W){
                pressW = false;
            }
            if(iek->get_physical_scancode() == KEY_S){
                pressS = false;
            }
            if(iek->get_physical_scancode() == KEY_A){
                pressA = false;
            }
            if(iek->get_physical_scancode() == KEY_D){
                pressD = false;
            }
        }
        if(iek->is_pressed()){
            if(iek->get_physical_scancode() == KEY_W){
                pressW = true;
            }
            if(iek->get_physical_scancode() == KEY_S){
                pressS = true;
            }
            if(iek->get_physical_scancode() == KEY_A){
                pressA = true;
            }
            if(iek->get_physical_scancode() == KEY_D){
                pressD = true;
            }
        }
    }

}
void TestScene::GenGhost(){
    RandomPCG rnd;
    rnd.seed(SFWTime::time_ns());
    Enemy enemy;
    switch(rnd.random(0,3)){
        case 0:
            enemy = Enemy(player.x+rnd.random(-1100, 1100), player.y+rnd.random(650, 750), chunkBlockSize/3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 10, 300, 10);
            break;
        case 1:
            enemy = Enemy(player.x+rnd.random(-1100, 1100), player.y+rnd.random(-750, -650), chunkBlockSize/3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 10, 300, 10);
            break;
        case 2:
            enemy = Enemy(player.x+rnd.random(-1100, -1000), player.y+rnd.random(-650, 650), chunkBlockSize/3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 10, 300, 10);
            break;
        case 3:
            enemy = Enemy(player.x+rnd.random(1000, 1100), player.y+rnd.random(-650, 650), chunkBlockSize/3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 10, 300, 10);
            break;
    }
    enemies.push_back(enemy);
}

void TestScene::GenerateChunk(int x, int y){
    List<TerrainItem> terrainObjs = {};
    RandomPCG rnd;

    rnd.seed(SFWTime::time_ns()+x+y);
    int rockCount = rnd.random(0, 4);
    for(int i = 0; i < rockCount; i++){
        int rockX = rnd.random(-8, 9);
        int rockY = rnd.random(-8, 9);
        int rockT = rnd.random(0, 3);
        terrainObjs.push_back(TerrainItem((int)(rockX*(chunkBlockSize/3)-(chunkBlockSize/3/2)+x), (int)(-rockY*(chunkBlockSize/3)+(chunkBlockSize/3/2)+y), chunkBlockSize/3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, &_rockTextures[rockT]));
    }


    mtx.lock();
    chunks.push_back(Chunk(x, y, 960-(chunkBlockSize*3), 540-(chunkBlockSize*3), terrainObjs));
    mtx.unlock();
}

void TestScene::thread_func(void *p_user_data){
    ThreadData* td = reinterpret_cast<ThreadData*>(p_user_data);
    td->scene->GenerateChunk(td->startX, td->startY);
    delete td;
}

void TestScene::CheckCollisionPlayer(){
    Rect2 playerRect = Rect2(player.x + player.offsetX + prePlayerX, -player.y + player.offsetY + prePlayerY, player.eSize, player.eSize);

    //enemies
    for(int i = 0; i < enemies.size(); i++){
        Rect2 enemyRect = Rect2(enemies[i].x + enemies[i].offsetX, -enemies[i].y + enemies[i].offsetY, enemies[i].eSize, enemies[i].eSize);
        if(enemyRect.intersects(playerRect)){
            player.hp -= enemies[i].damage * scaledDelta;
            if(player.hp <= 0.0f){
                //TODO player itt hal meg
                //timeScale = 0.0f;
            }
        }
    }

    //rocks
    canMoveX = true;
    canMoveY = true;

    //x preMove
    playerRect = Rect2(player.x + player.offsetX + prePlayerX, -player.y + player.offsetY, player.eSize, player.eSize);
    for(int i = 0; i < chunks.size(); i++){
        for(int j = 0; j < chunks[i].terrainItems.size(); j++){
            Rect2 itemRect = Rect2(chunks[i].terrainItems[j].x + chunks[i].terrainItems[j].offsetX, -chunks[i].terrainItems[j].y + chunks[i].terrainItems[j].offsetY, chunks[i].terrainItems[j].eSize, chunks[i].terrainItems[j].eSize);
            if(itemRect.intersects(playerRect)){
                canMoveX = false;
                break;
            }
        }
        if(!canMoveX) break;
    }

    //y preMove //TODO ez nem jo és nem ertem miert
    playerRect = Rect2(player.x + player.offsetX, -player.y + player.offsetY + prePlayerY, player.eSize, player.eSize);
    for(int i = 0; i < chunks.size(); i++){
        for(int j = 0; j < chunks[i].terrainItems.size(); j++){
            Rect2 itemRect = Rect2(chunks[i].terrainItems[j].x + chunks[i].terrainItems[j].offsetX, -chunks[i].terrainItems[j].y + chunks[i].terrainItems[j].offsetY, chunks[i].terrainItems[j].eSize, chunks[i].terrainItems[j].eSize);
            if(itemRect.intersects(playerRect)){
                canMoveY = false;
                break;
            }
        }
        if(!canMoveY) break;
    }
}

void TestScene::update(float delta){
    scaledDelta = delta * timeScale;
    if(!start){
        Thread t1, t2, t3, t4, t5, t6, t7, t8, t9;
        ThreadData* td1 = new ThreadData{0, 0, this};
        ThreadData* td2 = new ThreadData{chunkBlockSize*6, 0, this};
        ThreadData* td3 = new ThreadData{-chunkBlockSize*6, 0, this};
        ThreadData* td4 = new ThreadData{0, chunkBlockSize*6, this};
        ThreadData* td5 = new ThreadData{0, -chunkBlockSize*6, this};
        ThreadData* td6 = new ThreadData{chunkBlockSize*6, chunkBlockSize*6, this};
        ThreadData* td7 = new ThreadData{chunkBlockSize*6, -chunkBlockSize*6, this};
        ThreadData* td8 = new ThreadData{-chunkBlockSize*6, chunkBlockSize*6, this};
        ThreadData* td9 = new ThreadData{-chunkBlockSize*6, -chunkBlockSize*6, this};
        t1.start(&TestScene::thread_func, td1);
        t2.start(&TestScene::thread_func, td2);
        t3.start(&TestScene::thread_func, td3);
        t4.start(&TestScene::thread_func, td4);
        t5.start(&TestScene::thread_func, td5);
        t6.start(&TestScene::thread_func, td6);
        t7.start(&TestScene::thread_func, td7);
        t8.start(&TestScene::thread_func, td8);
        t9.start(&TestScene::thread_func, td9);

        start = true;
    }

    if(canSpawnEnemy) enemySpawnTimer += scaledDelta;
    if(enemySpawnTimer > 1.0f){
        enemySpawnTimer = 0.0f;
        GenGhost();
    }
    score+=scaledDelta;

    List<Enemy>::Element *e = enemies.front();
    for(int i = 0; i<enemies.size(); i++){
        int targetX = player.x - enemies[i].x;
        int targetY = player.y - enemies[i].y;
        double value = targetX*targetX+targetY*targetY;
        int length = Math::sqrt(value);
        if(length>enemies[i].speed*scaledDelta){
            enemies[i].x = enemies[i].x + (enemies[i].speed*scaledDelta) * targetX / length;
            enemies[i].y = enemies[i].y + (enemies[i].speed*scaledDelta) * targetY / length;
        }

        enemies[i].aliveTime+=scaledDelta;
        if(enemies[i].aliveTime>5.0f){
            e->erase();
            i--;
        }
        e = e->next();
    }

    //camera/player control
    prePlayerX = 0;
    prePlayerY = 0;
    if(pressW) prePlayerY += player.speed * scaledDelta;
    if(pressS) prePlayerY += -player.speed *  scaledDelta;
    if(pressA) prePlayerX += -player.speed * scaledDelta;
    if(pressD) prePlayerX += player.speed * scaledDelta;

    //CheckCollisionPlayer();

    if(camState == CameraState::Both){
        if(canMoveX) player.x += prePlayerX;
        if(canMoveY) player.y += prePlayerY;

        if(canMoveX) camX += prePlayerX;
        if(canMoveY) camY += prePlayerY;
    }
    else if(camState == CameraState::Camera){
        camX += prePlayerX;
        camY += prePlayerY;
    }
    else if(camState == CameraState::Player){
        if(canMoveX) player.x += prePlayerX;
        if(canMoveY) player.y += prePlayerY;
    }
}
void TestScene::render(){
    Renderer *r = Renderer::get_singleton();

    r->clear_screen(Color(1, 1 ,1));
    r->camera_2d_projection_set_to_window();

    //chunk render
    for(int i = 0; i < chunks.size(); i++){
        int x = chunks[i].x;
        int y = chunks[i].y;
        if(Math::abs(x - player.x) > 1560 || Math::abs(y - player.y) > 1140){
            continue;
        }
        for(int j = 0; j < 6; j++){
            for(int k = 0; k < 6; k++){
                r->draw_texture(_grassTexture, Rect2(x - camX + chunks[i].offsetX, -(y - camY) + chunks[i].offsetY, chunkBlockSize, chunkBlockSize));
                x+= chunkBlockSize;
            }
        y -= chunkBlockSize;
        x = chunks[i].x;
        }
        for(int j = 0;j<chunks[i].terrainItems.size();j++){
            r->draw_texture(*chunks[i].terrainItems[j].texture, Rect2(chunks[i].terrainItems[j].x - camX + chunks[i].terrainItems[j].offsetX, -(chunks[i].terrainItems[j].y - camY) + chunks[i].terrainItems[j].offsetY, chunks[i].terrainItems[j].eSize, chunks[i].terrainItems[j].eSize));
        }
    }

    //player
    r->draw_texture(_playerTexture, Rect2(player.x - camX + player.offsetX, -(player.y - camY) + player.offsetY, player.eSize, player.eSize));

    //enemy
    for(int i = 0; i < enemies.size(); i++){
        r->draw_texture(_enemyTexture, Rect2(enemies[i].x - camX + enemies[i].offsetX, -(enemies[i].y - camY) + enemies[i].offsetY, enemies[i].eSize, enemies[i].eSize));
    }

    //ui
    r->draw_text_2d("Score: "+String::num(score), _font, Vector2(10,10), Color(190, 0, 180));
    r->draw_text_2d("HP: "+String::num(player.hp, 0), _font, Vector2(10,50), Color(190, 0, 180));

    //debug
    if(showChunkBorder){
        for(int i = 0; i < chunks.size(); i++){
            r->draw_line_rect(Rect2(chunks[i].x-camX + chunks[i].offsetX, -(chunks[i].y-camY) + chunks[i].offsetY, chunkBlockSize*6, chunkBlockSize*6),Color(180, 0, 0), 2);
            //chunk pozicioja
            r->draw_text_2d("X: "+String::num(chunks[i].x)+"\nY: "+String::num(chunks[i].y), _font, Vector2(chunks[i].x-camX+chunkBlockSize*3 + chunks[i].offsetX, -(chunks[i].y-camY)+chunkBlockSize*3 + chunks[i].offsetY), Color(190, 0, 180));
            //chunk es player tavolsaga
            //r->draw_text_2d("X: "+String::num(Math::abs(chunks[i].x-player.x))+"\nY: "+String::num(Math::abs(chunks[i].y-player.y)), _font, Vector2(chunks[i].x-camX+chunkBlockSize*3 + chunks[i].offsetX, -(chunks[i].y-camY)+chunkBlockSize*3 + chunks[i].offsetY), Color(190, 0, 180));
        }
    }
    if(showBlockBorder){
        for(int i = 0; i < chunks.size(); i++){
            for(int j = 0; j < 6; j++){
                for(int k = 0; k < 6; k++){
                    r->draw_line_rect(Rect2(chunks[i].x-camX+j*chunkBlockSize + chunks[i].offsetX, -(chunks[i].y-camY)+k*chunkBlockSize + chunks[i].offsetY, chunkBlockSize, chunkBlockSize),Color(180, 0, 0), 2);
                }
            }
        }
    }
    if(showItemBorder){
        r->draw_line_rect(Rect2(player.x - camX + player.offsetX, -(player.y - camY) + player.offsetY,  player.eSize, player.eSize),Color(0, 180, 180), 2);
        for(int i = 0; i < chunks.size(); i++){
            for(int j = 0;j<chunks[i].terrainItems.size();j++){
                r->draw_line_rect(Rect2(chunks[i].terrainItems[j].x - camX + chunks[i].terrainItems[j].offsetX, -(chunks[i].terrainItems[j].y - camY) + chunks[i].terrainItems[j].offsetY, chunks[i].terrainItems[j].eSize, chunks[i].terrainItems[j].eSize),Color(0, 0, 180), 2);
                r->draw_text_2d("Y: "+String::num(chunks[i].terrainItems[j].y), _font, Vector2(chunks[i].terrainItems[j].x - camX + chunks[i].terrainItems[j].offsetX, -(chunks[i].terrainItems[j].y - camY) + chunks[i].terrainItems[j].offsetY), Color(190, 0, 180));
                r->draw_text_2d("X: "+String::num(chunks[i].terrainItems[j].x), _font, Vector2(chunks[i].terrainItems[j].x - camX + chunks[i].terrainItems[j].offsetX, -(chunks[i].terrainItems[j].y - camY-30) + chunks[i].terrainItems[j].offsetY), Color(190, 0, 180));
            }
        }
        for(int i = 0; i <enemies.size(); i++){
            r->draw_line_rect(Rect2(enemies[i].x - camX + enemies[i].offsetX, -(enemies[i].y - camY) + enemies[i].offsetY,  enemies[i].eSize, enemies[i].eSize),Color(0, 180, 180), 2);
        }
    }
    if(showCenterLine){
        r->draw_line(Vector2(0, r->get_window_size().y/2),Vector2(r->get_window_size().x, r->get_window_size().y/2), Color(180, 0, 0), 2);
        r->draw_line(Vector2(r->get_window_size().x/2, 0),Vector2(r->get_window_size().x/2, r->get_window_size().y), Color(180, 0, 0), 2);
    }


    GUI::new_frame();

    ImGui::Begin("Debug");
    if(ImGui::Button("Chunk Border on/off")){
        showChunkBorder = !showChunkBorder;
    };
    if(ImGui::Button("Block Border on/off")){
        showBlockBorder = !showBlockBorder;
    };
    if(ImGui::Button("Item Border on/off")){
        showItemBorder = !showItemBorder;
    };
    if(ImGui::Button("Center Line on/off")){
        showCenterLine = !showCenterLine;
    };
    if(ImGui::Button("Enemy Spawning on/off")){
        canSpawnEnemy = !canSpawnEnemy;
    };

    ImGui::Text("\nCamera Modes");
    if(ImGui::Button("Both")){
        camState = CameraState::Both;
    };
    if(ImGui::Button("Only Camera")){
        camState = CameraState::Camera;
    };
    if(ImGui::Button("Only Player")){
        camState = CameraState::Player;
    };
    ImGui::Text("");
    if(ImGui::Button("Snap Camera to Player")){
        camX=player.x;
        camY=player.y;
    };

    ImGui::Text("\ncamX: %d | playerX: %d", camX, player.x);
    ImGui::Text("camY: %d | playerY: %d", camY, player.y);

    ImGui::End();

    GUI::render();
}

TestScene::TestScene(){
    _font.instance();
    _font->load_default(31.5);

    _grassImage.instance();
    _grassImage->load_from_file("assetts/inUse/grass.png");
    _grassTexture.instance();
    _grassTexture->create_from_image(_grassImage);

    _rockImage1.instance();
    _rockImage1->load_from_file("assetts/inUse/Rock1.png");
    _rockTexture1.instance();
    _rockTexture1->create_from_image(_rockImage1);
    _rockImage2.instance();
    _rockImage2->load_from_file("assetts/inUse/Rock2.png");
    _rockTexture2.instance();
    _rockTexture2->create_from_image(_rockImage2);
    _rockImage3.instance();
    _rockImage3->load_from_file("assetts/inUse/Rock3.png");
    _rockTexture3.instance();
    _rockTexture3->create_from_image(_rockImage3);
    _rockImage4.instance();
    _rockImage4->load_from_file("assetts/inUse/Rock4.png");
    _rockTexture4.instance();
    _rockTexture4->create_from_image(_rockImage4);

    _rockTextures.push_back(_rockTexture1);
    _rockTextures.push_back(_rockTexture2);
    _rockTextures.push_back(_rockTexture3);
    _rockTextures.push_back(_rockTexture4);

    _playerImage.instance();
    _playerImage->load_from_file("assetts/inUse/Player.png");
    _playerTexture.instance();
    _playerTexture->create_from_image(_playerImage);
    _enemyImage.instance();
    _enemyImage->load_from_file("assetts/inUse/Enemy.png");
    _enemyTexture.instance();
    _enemyTexture->create_from_image(_enemyImage);

    chunks = {};
    enemies = {};
    camX = 0;
    camY = 0;
    chunkBlockSize = 200;
    player = Player(0, 0, chunkBlockSize / 3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 100, 950, 10);
    camState = CameraState::Both;
    enemySpawnTimer = 0.0f;
    score = 0;
    prePlayerX = 0;
    prePlayerY = 0;
    canMoveX = true;
    canMoveY = true;
}
