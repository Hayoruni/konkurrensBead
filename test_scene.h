#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "sfw.h"

class Chunk{

public:
	int x = 0;
	int y = 0;

	Chunk(int x, int y){
        this->x = x;
        this->y = y;
	}
};

class TestScene : public Scene{
    SFW_OBJECT(TestScene, Scene);

public:
	virtual void input_event(const Ref<InputEvent> &event);
	virtual void update(float delta);
	virtual void render();

    TestScene();

    List<Chunk> chunks = {};

protected:
    int _int_slider_value;
    bool _test_label_shown;

    Ref<Font> _font;
    Ref<Image> _grassImage;
    Ref<Texture> _grassTexture;

    HashMap<String, int> _test_keys;
};
#endif
