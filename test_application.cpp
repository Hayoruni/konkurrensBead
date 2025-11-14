#include "test_application.h"

TestApplication::TestApplication(){
    Renderer::initialize();
    GUI::initialize();
    scene = Ref<Scene>(memnew(TestScene));
    AppWindow::get_singleton()->set_fullscreen(true);
}
TestApplication::~TestApplication(){
    Renderer::destroy();
    GUI::destroy();
}
