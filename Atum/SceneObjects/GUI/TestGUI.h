
#pragma once

#include "Services/Scene/SceneObject.h"
#include "GUIWidget.h"
#include "../Programs.h"

class TestGUI : public SceneObject
{
public:

	Vector2 size;
	Vector2 pos;

	META_DATA_DECL(TestGUI)

	GUIWidget widget;

	CLASSDECLDIF(SceneObject, TestGUI)
	TestGUI();
	virtual ~TestGUI();

	virtual void Init();
	virtual void ApplyProperties();
	virtual void Play();
	virtual void Work(float dt);
};
