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
            if(iek->get_physical_scancode() == KEY_ESCAPE){
                escPress();
            }
        }
    }

}

void TestScene::newGameClick(){
    if(score > highScore)highScore = score;
    newGame();
}

void TestScene::quitClick(){
    Application::get_singleton()-> running = false;
}

void TestScene::escPress(){
    if(menuOpen && player.hp >= 0.0f){
        timeScale = 1.0f;
        menuOpen = false;
    }
    else if(!menuOpen){
        timeScale = 0.0f;
        menuOpen = true;
    }

}

void TestScene::newGame() {
    chunks.clear();
    chunksInGeneration.clear();
    enemies.clear();
    projectiles.clear();
    camX = 0.0f;
    camY = 0.0f;
    player = Player(0, 0, chunkBlockSize / 3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 100, 400, 3.0f, 10);
    camState = CameraState::Both;
    enemySpawnTimer = 0.0f;
    score = 0;
    prePlayerX = 0.0f;
    prePlayerY = 0.0f;
    canMoveX = true;
    canMoveY = true;
    menuOpen = false;

    timeScale = 1.0f;
    start = false;
    pressW = false;
    pressS = false;
    pressA = false;
    pressD = false;

    fpsElapsed = 1.0f;
    fps = 0;
    fpsDisplay = 0;
    fpsElapsed = 1.0f;
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
    int rockCount = rnd.random(1, 4);
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

void TestScene::CheckChunks(){
    indexX = Math::round(player.x / 1200);// if(Math::abs(player.x) % 1200 > 600){if(player.x / 1200 > 0){index += 1;}else if(player.x / 1200<0){index-=1;}}
    indexY = Math::round(player.y / 1200);// if(Math::abs(player.y) % 1200 > 600){if(player.y / 1200 > 0){indey += 1;}else if(player.y / 1200<0){indey-=1;}}

    int xPos = indexX * 1200;
    int yPos = indexY * 1200;

    auto CheckChunkAtPos = [&](int xAdd, int yAdd)
    {
        bool shouldGenerate = true;
        for(int i = 0; i < chunks.size(); i++){
            if(xPos+xAdd == chunks[i].x && yPos+yAdd == chunks[i].y){
                shouldGenerate = false;
                break;
            }
        }
        if(shouldGenerate){
            for(int i = 0; i < chunksInGeneration.size(); i++){
                if(chunksInGeneration[i].x == xPos + xAdd && chunksInGeneration[i].y == yPos + yAdd){
                    shouldGenerate = false;
                    break;
                }
            }
        }
        if(shouldGenerate){
            Thread t1;
            ThreadData* td1 = new ThreadData{xPos + xAdd, yPos + yAdd, this};
            t1.start(&TestScene::thread_func, td1);
            chunksInGeneration.push_back(Vector2(xPos + xAdd, yPos + yAdd));
        }
    };

    CheckChunkAtPos(chunkBlockSize*6, 0);
    CheckChunkAtPos(-chunkBlockSize*6, 0);
    CheckChunkAtPos(0, chunkBlockSize*6);
    CheckChunkAtPos(0, -chunkBlockSize*6);
    CheckChunkAtPos(chunkBlockSize*6, chunkBlockSize*6);
    CheckChunkAtPos(chunkBlockSize*6, -chunkBlockSize*6);
    CheckChunkAtPos(-chunkBlockSize*6, chunkBlockSize*6);
    CheckChunkAtPos(-chunkBlockSize*6, -chunkBlockSize*6);
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
                menuOpen = true;
                timeScale = 0.0f;
                if(score > highScore)highScore = score;
                return;
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

    //y preMove
    playerRect = Rect2(player.x + player.offsetX, -player.y + player.offsetY - prePlayerY, player.eSize, player.eSize);
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
void TestScene::CheckCollisionMoveEnemy(){
    for(int i = 0; i<enemies.size(); i++){
        int targetX = player.x - enemies[i].x;
        int targetY = player.y - enemies[i].y;
        double value = targetX*targetX+targetY*targetY;
        int length = Math::sqrt(value);
        if(length>enemies[i].speed*scaledDelta){
            enemies[i].preMoveX = enemies[i].x + (enemies[i].speed*scaledDelta) * targetX / length;
            enemies[i].preMoveY = enemies[i].y + (enemies[i].speed*scaledDelta) * targetY / length;
        }
    }

    Rect2 enemyRect;
    Rect2 enemyRect2;
    bool enemyCanMoveX = true;
    bool enemyCanMoveY = true;

    //x preMove
    for(int i = 0; i<enemies.size(); i++){
        enemyCanMoveX = true;
        enemyRect = Rect2(enemies[i].preMoveX + enemies[i].offsetX, -enemies[i].y + enemies[i].offsetY, enemies[i].eSize, enemies[i].eSize);
        for(int j = 0; j<enemies.size(); j++){
            if(enemies[i].x == enemies[j].x && enemies[i].y == enemies[j].y) continue;
            enemyRect2 = Rect2(enemies[j].x + enemies[j].offsetX, -enemies[j].y + enemies[j].offsetY, enemies[j].eSize, enemies[j].eSize);
            if(enemyRect.intersects(enemyRect2)){
                enemyCanMoveX = false;
            }
        }
        if(enemyCanMoveX) enemies[i].x = enemies[i].preMoveX;
    }

    //y preMove
    for(int i = 0; i<enemies.size(); i++){
        enemyCanMoveY = true;
        enemyRect = Rect2(enemies[i].x + enemies[i].offsetX, -enemies[i].preMoveY + enemies[i].offsetY, enemies[i].eSize, enemies[i].eSize);
        for(int j = 0; j<enemies.size(); j++){
            if(enemies[i].x == enemies[j].x && enemies[i].y == enemies[j].y) continue;
            enemyRect2 = Rect2(enemies[j].x + enemies[j].offsetX, -enemies[j].y + enemies[j].offsetY, enemies[j].eSize, enemies[j].eSize);
            if(enemyRect.intersects(enemyRect2)){
                enemyCanMoveY = false;
            }
        }
        if(enemyCanMoveY) enemies[i].y = enemies[i].preMoveY;
    }
}

void TestScene::CheckCollisionProjectile(){
    if(projectiles.size()<=0) return;
    Rect2 enemyRect;
    Rect2 projectileRect;
    List<Projectile>::Element *e = projectiles.front();

    for(int i = 0;i<projectiles.size();i++){
        projectileRect = Rect2(projectiles[i].x + projectiles[i].offsetX, -projectiles[i].y + projectiles[i].offsetY, projectiles[i].eSize, projectiles[i].eSize);
        for(int j = 0;j<enemies.size();j++){
            enemyRect = Rect2(enemies[j].x + enemies[j].offsetX, -enemies[j].y + enemies[j].offsetY, enemies[j].eSize, enemies[j].eSize);
            if(enemyRect.intersects(projectileRect)){
                e->erase();
                i--;
                enemies[j].hp = 0;
            }
        }
        e = e->next();
    }
}

void TestScene::PlayerAttack(){
    if(enemies.size()>0){
        int minIndex = 0;
        int minDistance = Math::sqrt((((enemies[0].x - player.x)*(enemies[0].x - player.x))+((enemies[0].y - player.y)*(enemies[0].y - player.y))));
        for(int i = 0;i <enemies.size();i++){
            int targetX =  enemies[i].x - player.x;
            int targetY = enemies[i].y - player.y;
            double value = targetX*targetX+targetY*targetY;
            int length = Math::sqrt(value);
            if(length < minDistance){
                minDistance = length;
                minIndex = i;
            }
        }
        float targetX =  enemies[minIndex].x - player.x;
        float targetY = enemies[minIndex].y - player.y;
        double value = targetX*targetX+targetY*targetY;
        float length = Math::sqrt(value);
        projectiles.push_back(Projectile(player.x,player.y,chunkBlockSize/6,960-chunkBlockSize/6/2, 540-chunkBlockSize/6/2,targetX/length,targetY/length,10.0f,player.speed*2));
    }
    else{
        projectiles.push_back(Projectile(player.x,player.y,chunkBlockSize/6,960-chunkBlockSize/6/2, 540-chunkBlockSize/6/2,1.0f,1.0f,10.0f,player.speed*2));
    }

}

void TestScene::update(float delta){
    scaledDelta = delta * timeScale;
    if(!start){
        Application::get_singleton()->target_fps = 500;
        chunks.push_back(Chunk(0, 0, 960-(chunkBlockSize*3), 540-(chunkBlockSize*3), {}));
        chunksInGeneration.push_back(Vector2(0, 0));

        start = true;
    }

    CheckChunks();

    fps++;
    fpsElapsed += delta;
    if(fpsElapsed > 1.0f){
        fpsDisplay = fps;
        fps = 0;
        fpsElapsed = 0.0f;
    }

    if(canSpawnEnemy) enemySpawnTimer += scaledDelta;
    if(enemySpawnTimer > 2.0f){
        enemySpawnTimer = 0.0f;
        GenGhost();
    }
    score+=scaledDelta;

    List<Enemy>::Element *e = enemies.front();
    for(int i = 0; i<enemies.size(); i++){
        if(enemies[i].hp<=0){
            e->erase();
            i--;
        }
        e = e->next();
    }
    CheckCollisionMoveEnemy();

    CheckCollisionProjectile();
    List<Projectile>::Element *e2 = projectiles.front();
    for(int i =0;i<projectiles.size();i++){
        projectiles[i].x += projectiles[i].targetX * projectiles[i].speed * scaledDelta;
        projectiles[i].y += projectiles[i].targetY * projectiles[i].speed * scaledDelta;
        projectiles[i].lifeTime-=scaledDelta;
        if(projectiles[i].lifeTime<=0.0f){
            e2->erase();
            i--;
        }
        e2 = e2->next();
    }

    player.attackElapsed -= scaledDelta;
    if(player.attackElapsed<=0.0f){
        player.attackElapsed = player.attackSpeed;
        PlayerAttack();
    }

    //menu button press
    if(menuOpen && input->is_mouse_button_pressed(1)){
        Point2 mPos = input->get_mouse_position();
        for(int i = 0; i< menuButtons.size(); i++){
            if(mPos.x > menuButtons[i].x-menuButtons[i].offsetX && mPos.x < menuButtons[i].x+menuButtons[i].offsetX &&
            mPos.y > menuButtons[i].y-menuButtons[i].offsetY && mPos.y < menuButtons[i].y+menuButtons[i].offsetY)
            {
                if(menuButtons[i].func){
                    (this->*menuButtons[i].func)();
                }
            }
        }
    }

    //camera/player control
    prePlayerX = 0.0f;
    prePlayerY = 0.0f;
    if(pressW) prePlayerY += player.speed * scaledDelta;
    if(pressS) prePlayerY += -player.speed * scaledDelta;
    if(pressA) prePlayerX += -player.speed * scaledDelta;
    if(pressD) prePlayerX += player.speed * scaledDelta;

    CheckCollisionPlayer();

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
    if(player.hp>0){
        r->draw_texture(_playerTexture, Rect2(player.x - camX + player.offsetX, -(player.y - camY) + player.offsetY, player.eSize, player.eSize));
    }
    //enemy
    for(int i = 0; i < enemies.size(); i++){
        r->draw_texture(_enemyTexture, Rect2(enemies[i].x - camX + enemies[i].offsetX, -(enemies[i].y - camY) + enemies[i].offsetY, enemies[i].eSize, enemies[i].eSize));
    }
    for(int i = 0; i < projectiles.size(); i++){
        r->draw_texture(_projectileTexture, Rect2(projectiles[i].x - camX + projectiles[i].offsetX, -(projectiles[i].y - camY) + projectiles[i].offsetY, projectiles[i].eSize, projectiles[i].eSize));
    }

    //ui
    r->draw_text_2d("Score: "+String::num(Math::floor(score)), _font, Vector2(5,10), Color(190, 0, 180));
    r->draw_text_2d("HP: "+String::num(player.hp, 0), _font, Vector2(5,45), Color(190, 0, 180));
    r->draw_text_2d("FPS: "+String::num(fpsDisplay), _font, Vector2(5,80), Color(190, 0, 180));

    if(menuOpen){
        if(player.hp<=0){
            r->draw_text_2d("You Died", _font, Vector2(890,230), Color(200, 0, 0));
        }
        for(int i = 0; i < menuButtons.size(); i++){
            r->draw_texture(*menuButtons[i].texture, Rect2(menuButtons[i].x - menuButtons[i].offsetX, menuButtons[i].y - menuButtons[i].offsetY, menuButtons[i].sizeX, menuButtons[i].sizeY));
            r->draw_text_2d(menuButtons[i].text, _font, Vector2(menuButtons[i].x - menuButtons[i].offsetX+15, menuButtons[i].y - menuButtons[i].offsetY+5), Color(255, 255, 255));
        }
        r->draw_text_2d("Highscore: "+String::num(Math::floor(highScore)), _font, Vector2(865,400), Color(255, 255, 255));
    }


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
    if(ImGui::Button("Pause/Resume")){
        if(timeScale == 1.0f) timeScale = 0.0f;
        else timeScale = 1.0f;
    };
    if(ImGui::Button("Damage Player 50hp")){
        player.hp-=50;
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

    ImGui::Text("\ncamX: %f | playerX: %f", camX, player.x);
    ImGui::Text("camY: %f | playerY: %f", camY, player.y);

    ImGui::Text("indexX: %d", indexX);
    ImGui::Text("indexy: %d", indexY);
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
    _projectileImage.instance();
    _projectileImage->load_from_file("assetts/inUse/projectile.png");
    _projectileTexture.instance();
    _projectileTexture->create_from_image(_projectileImage);
    _newGameImage.instance();
    _newGameImage->load_from_file("assetts/inUse/newGame.png");
    _newGameTexture.instance();
    _newGameTexture->create_from_image(_newGameImage);
    _quitImage.instance();
    _quitImage->load_from_file("assetts/inUse/quit.png");
    _quitTexture.instance();
    _quitTexture->create_from_image(_quitImage);

    input  = Input::get_singleton();
    menuButtons = {};
    chunks = {};
    chunksInGeneration = {};
    enemies = {};
    camX = 0.0f;
    camY = 0.0f;
    chunkBlockSize = 200;
    player = Player(0, 0, chunkBlockSize / 3, 960-chunkBlockSize/3/2, 540-chunkBlockSize/3/2, 100, 400, 3.0f, 10);
    camState = CameraState::Both;
    enemySpawnTimer = 0.0f;
    score = 0;
    highScore = 0;
    prePlayerX = 0.0f;
    prePlayerY = 0.0f;
    canMoveX = true;
    canMoveY = true;
    menuOpen = false;


    menuButtons.push_back(Button(960, 300, 190, 50, "New Game", &_newGameTexture, &TestScene::newGameClick));
    menuButtons.push_back(Button(960, 365, 90, 50, "Quit", &_quitTexture, &TestScene::quitClick));
}
