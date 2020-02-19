
#pragma once

#include "Services/Scene/SceneObject.h"
#include "BlueprintAsset.h"

class BlueprintInst : public SceneObjectInst
{
public:
	META_DATA_DECL(BlueprintrInst)
	Transform2D trans;
	std::vector<SceneObject*> childs;

	virtual ~BlueprintInst() = default;

	void Init() override;
	void Load(JSONReader& loader) override;
	void Save(JSONWriter& saver) override;

	void ApplyProperties() override;
	void SetState(State set_state) override;
	bool Play() override;

	SceneObject* GetChild(uint32_t uid) override;

#ifdef EDITOR
	void Copy(SceneObject* src) override;
	void SetEditMode(bool ed) override;
	void AddChildsToTree(EUITreeView* treeview) override;
#endif
};