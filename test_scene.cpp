#include "test_scene.h"

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
    chunks.push_back(Chunk(x, y));
    //TODO kiboviteni
}

void TestScene::update(float delta){
    if(!start){
        GenerateChunk(0, 0);
        GenerateChunk(chunkBlockSize*6, 0);
        GenerateChunk(-chunkBlockSize*6, 0);
        GenerateChunk(0, chunkBlockSize*6);
        GenerateChunk(0, -chunkBlockSize*6);

        start = true;
    }
    if(pressW) camY += playerSpeed; //ezeket meg kéne szorozni deltaTime-al de nem tudom hol van
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
    }

    //debug
    if(showChunkBorder){
        for(int i = 0; i < chunks.size(); i++){
            r->draw_line_rect(Rect2(chunks[i].x+camX, chunks[i].y+camY, chunkBlockSize*6, chunkBlockSize*6),Color(180, 0, 0), 2);
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

    chunks = {};
    camX = 0;
    camY = 0;
    playerSpeed = 20;
    chunkBlockSize = 200;
}
