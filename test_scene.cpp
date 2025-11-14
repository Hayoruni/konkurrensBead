#include "test_scene.h"

void TestScene::input_event(const Ref<InputEvent> &event){

    Ref<InputEventKey> iek = event;

    if(iek.is_valid()){
        if(!iek->is_pressed()){
            return;
        }
        if(iek->get_physical_scancode() == KEY_K){
            if(!_test_keys.has("K")){
                _test_keys["K"] = 1;
            }
            else{
                _test_keys["K"] += 1;
            }
        }
        if(iek->get_physical_scancode() == KEY_L){
            if(!_test_keys.has("L")){
                _test_keys["L"] = 1;
            }
            else{
                _test_keys["L"] += 1;
            }
        }
    }

}
void TestScene::update(float delta){
    Chunk chunk = Chunk(10,10);
    chunks.push_back(Chunk(10, 10));
}
void TestScene::render(){
    Renderer *r = Renderer::get_singleton();

    r->clear_screen(Color(1, 1 ,1));
    r->camera_2d_projection_set_to_window();
    int y = 10;
    int x = 10;
    for(int i = 0; i < 1; i++){
        r->draw_texture(_grassTexture, Rect2(chunks[i].x, chunks[i].y, 100, 100));
    }

    /*for(int i = 0; i<10; i++){
        for(int j = 0; j < 19; j++){
            //r->draw_rect(Rect2(j, y, 300, 300),Color(0, 255, 0));
            r->draw_texture(_grassTexture, Rect2(x, y, 100, 100));
            x += 100;
        }
        y += 105;
        x = 10;
    }*/


    //r->draw_text_2d("Text!", _font, Vector2(300, 300),Color(1, 0, 0));
    //r->draw_texture(_texture, Rect2(400, 400, 100, 100));
    //r->draw_texture_clipped(_texture, Rect2(32, 32, 16, 16), Rect2(400, 600, 100, 100));

    float currypos = 100;
    for(HashMap<String, int>::Element *E = _test_keys.front(); E; E = E->next){
        r->draw_text_2d(E->key() + " "+ String::num(E->value()), _font, Vector2(500, currypos),Color(0, 1, 0) );
        currypos += 100;
    }



    GUI::new_frame();

    /*ImGui::Begin("Ez egy ablak!");
    ImGui::Text("Ez egy textbox!");
    ImGui::SliderInt(" = int value", &_int_slider_value, -200, 200);

    if(ImGui::Button("Ez egy gomb")){
        LOG_MSG("Egy gomb meg lett nyomva");
    }

    if(_test_label_shown){
        ImGui::Text("Test label");
    }
    if(ImGui::Button("Toggle test label!")){
        _test_label_shown = !_test_label_shown;
    }

    String s = "Ez";
    s+= " egy";
    s+= " manupulált " + String::num(1.2);
    s += " String";

    ImGui::End();*/

    GUI::render();
}

TestScene::TestScene(){
    _int_slider_value = 10;
    _test_label_shown = true;

    _font.instance();
    _font->load_default(31.5);

    _grassImage.instance();
    _grassImage->load_from_file("grass.png");

    _grassTexture.instance();
    _grassTexture->create_from_image(_grassImage);
}
