
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SceneAssets/Phys2DComp.h"

class Triger2D : public SceneObject
{
public:
	META_DATA_DECL(Triger2D)

	Phys2DCompInst::BodyUserData body;
	Transform2D trans;

	virtual ~Triger2D() = default;

	void Init() override;
	void EditorDraw(float dt);

	bool Play() override;
	void Stop() override;

#ifdef EDITOR
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
#endif
};
