#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"

class TestScene : public Scene{
    SFW_OBJECT(TestScene, Scene);

public:
	virtual void input_event(const Ref<InputEvent> &event);
	virtual void update(float delta);
	virtual void render();

    TestScene();

protected:
    int _int_slider_value;
    bool _test_label_shown;

    Ref<Font> _font;
    Ref<Image> _image;
    Ref<Texture> _texture;

    HashMap<String, int> _test_keys;
};
#endif
