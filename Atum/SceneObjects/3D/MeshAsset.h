
#pragma once

#include "Services/Scene/SceneAsset.h"
#include "Services/Mesh/Meshes.h"

class MeshAsset : public SceneAsset
{
public:
	META_DATA_DECL(MeshAsset)

	std::string mesh_name;
	Mesh::Instance* instance = nullptr;

	/**
	\brief Transform data of a sprite. This data will be used in instances of a sprite.
	*/

	Matrix trans;

#ifndef DOXYGEN_SKIP

	MeshAsset();
	virtual ~MeshAsset() = default;

	void Init() override;
	void ApplyProperties() override;
	void Draw(float dt);

	Matrix* Trans();
	bool Is3DObject();

#ifdef EDITOR
	//void SetEditMode(bool ed) override;
	void ResetView();
#endif

#endif
};
