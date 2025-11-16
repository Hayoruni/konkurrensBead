#include "test_scene.h"
#include <windows.h>

void TestScene::input_event(const Ref<InputEvent> &event){

    Ref<InputEventKey> iek = event;

    if(iek.is_valid()){
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

void TestScene::GenerateChunk(int x, int y){
    List<TerrainItem> terrainObjs = {};
    RandomPCG rnd;
    rnd.seed(GetTickCount64()+rndStart.random(-9999999,9999999)); //TODO ide lehet kell mutex lock mert nem tudom hogy keri le a kozos randomot de jonak tunik szoval idk
    int rockCount = rnd.random(0, 4);
    for(int i = 0; i<rockCount; i++){
        int rockX = rnd.random(0, 17);
        int rockY = rnd.random(0, 17);
        int rockT = rnd.random(0, 3);
        terrainObjs.push_back(TerrainItem((int)(rockX*(chunkBlockSize/3)+x), (int)(rockY*(chunkBlockSize/3)+y), &_rockTextures[rockT]));
    }


    mtx.lock();
    chunks.push_back(Chunk(x, y, terrainObjs));
    mtx.unlock();
}

void TestScene::thread_func(void *p_user_data){
    ThreadData* td = reinterpret_cast<ThreadData*>(p_user_data);
    td->scene->GenerateChunk(td->startX, td->startY);
    delete td;
}

void TestScene::update(float delta){
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
    if(pressW) camY += playerSpeed; //TODO ezeket meg kéne szorozni deltaTime-al de nem tudom hol van
    if(pressS) camY -= playerSpeed; //és biztos hogy nem igy kell nézni az inputot de most debughoz jó
    if(pressA) camX += playerSpeed;
    if(pressD) camX -= playerSpeed;

}
void TestScene::render(){
    Renderer *r = Renderer::get_singleton();

    r->clear_screen(Color(1, 1 ,1));
    r->camera_2d_projection_set_to_window();

    //chunk render
    for(int i = 0; i < chunks.size(); i++){
        int x = chunks[i].x;
        int y = chunks[i].y;
        for(int j = 0; j < 6; j++){
            for(int k = 0; k < 6; k++){
                r->draw_texture(_grassTexture, Rect2(x+camX, y+camY, chunkBlockSize, chunkBlockSize));
                x+= chunkBlockSize;
            }
        y += chunkBlockSize;
        x = chunks[i].x;
        }
        for(int j = 0;j<chunks[i].terrainItems.size();j++){
            r->draw_texture(*chunks[i].terrainItems[j].texture, Rect2(chunks[i].terrainItems[j].x + camX, chunks[i].terrainItems[j].y + camY, chunkBlockSize/3, chunkBlockSize/3));
        }
    }

    //debug
    if(showChunkBorder){
        for(int i = 0; i < chunks.size(); i++){
            r->draw_line_rect(Rect2(chunks[i].x+camX, chunks[i].y+camY, chunkBlockSize*6, chunkBlockSize*6),Color(180, 0, 0), 2);
        }
    }
    if(showTerrainItemBorder){
        for(int i = 0; i < chunks.size(); i++){
            for(int j = 0;j<chunks[i].terrainItems.size();j++){
                r->draw_line_rect(Rect2(chunks[i].terrainItems[j].x+camX, chunks[i].terrainItems[j].y+camY, chunkBlockSize/3, chunkBlockSize/3),Color(0, 0, 180), 2);
            }
        }
    }

    //r->draw_text_2d("Text!", _font, Vector2(300, 300),Color(1, 0, 0));
    //r->draw_texture_clipped(_texture, Rect2(32, 32, 16, 16), Rect2(400, 600, 100, 100));

    /*float currypos = 100;
    for(HashMap<String, int>::Element *E = _test_keys.front(); E; E = E->next){
        r->draw_text_2d(E->key() + " "+ String::num(E->value()), _font, Vector2(500, currypos),Color(0, 1, 0) );
        currypos += 100;
    }*/



    GUI::new_frame();

    ImGui::Begin("Debug");
    if(ImGui::Button("Chunk Border on/off")){
        showChunkBorder = !showChunkBorder;
    };
    if(ImGui::Button("Terrain Item Border on/off")){
        showTerrainItemBorder = !showTerrainItemBorder;
    };
    if(pressW) ImGui::Text("W Pressed");
    else ImGui::Text("W Released");
    if(pressS) ImGui::Text("S Pressed");
    else ImGui::Text("S Released");
    if(pressA) ImGui::Text("A Pressed");
    else ImGui::Text("A Released");
    if(pressD) ImGui::Text("D Pressed");
    else ImGui::Text("D Released");
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

    chunks = {};
    camX = 0;
    camY = 0;
    playerSpeed = 20;
    chunkBlockSize = 200;
    rndStart.seed(GetTickCount64());
}
