
#pragma once

#include "MeshAsset.h"

class MeshInstance : public SceneObjectInst
{
public:
	META_DATA_DECL(MeshInstance)

	GET_ASSET_METHOD(MeshAsset)

	struct Instance
	{
		META_DATA_DECL_BASE(Instance)

		Color color;

		Mesh::Instance* mesh = nullptr;

		/*~Instance()
		{
			RELEASE(mesh)
		}*/
	};

	vector<Instance> instances;

#ifndef DOXYGEN_SKIP

	MeshInstance();
	virtual ~MeshInstance() = default;

	void Init() override;
	void ApplyProperties() override;

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;

	void Draw(float dt);

	bool Is3DObject();

#ifdef EDITOR
	int sel_inst = -1;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif

#endif
};
