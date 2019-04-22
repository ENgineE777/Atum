
#pragma once

#include "Services/Scene/SceneObject.h"
#include "Phys2DComp.h"

class Trigger2D : public SceneObject
{
public:
	META_DATA_DECL(Trigger2D)

	PhysScene::BodyUserData body;
	Transform2D trans;

	virtual ~Trigger2D() = default;

	void Init() override;

	bool Play() override;
	void Stop() override;

#ifdef EDITOR
	void Set2DPos(Vector2 pos) override;
	void EditorDraw(float dt);
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
#endif
};
