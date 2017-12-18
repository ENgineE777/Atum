
#include "TestGUI.h"

CLASSDECLDECL(SceneObject, TestGUI)

META_DATA_DESC(TestGUI)

FLOAT_PROP(TestGUI, pos.x, 10.0f, "Geometry", "x")
FLOAT_PROP(TestGUI, pos.y, 10.0f, "Geometry", "y")
FLOAT_PROP(TestGUI, size.x, 100.0f, "Geometry", "width")
FLOAT_PROP(TestGUI, size.y, 100.0f, "Geometry", "height")

META_DATA_DESC_END()

TestGUI::TestGUI() : SceneObject()
{
}

TestGUI::~TestGUI()
{
}

void TestGUI::Init()
{
	GUIWidget::quad.Init();

	GUIWidget::tex = render.LoadTexture("Media/sprite_level16_2-hd.png");

	//Tasks()->AddTask(100, this, (Object::Delegate)&TestGUI::Work);
	RenderTasks()->AddTask(0, this, (Object::Delegate)&TestGUI::Work);
}

void TestGUI::ApplyProperties()
{
	widget.pos.Pos() = Vector(pos.x, pos.y, 0.0f);
	widget.size = size;
}

void TestGUI::Play()
{
	//client.listiner = &listiner;
	//client.Connect("127.0.0.1", 6881);
}

void TestGUI::Work(float dt)
{
	widget.Draw();
}