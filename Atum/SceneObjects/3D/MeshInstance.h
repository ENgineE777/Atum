
#pragma once

#include "MeshAsset.h"
#include "Services/Script/Libs/scriptarray.h"

class MeshInstance : public SceneObjectInst, public CScriptArray::Listiner
{
public:
	META_DATA_DECL(MeshInstance)

	GET_ASSET_METHOD(MeshAsset)

	struct Instance
	{
		enum MapIndices
		{
			IndexMesh,
			IndexVisible
		};

		META_DATA_DECL_BASE(Instance)

		Color color;

		vector<int>* mapping = nullptr;
		asIScriptObject* object = nullptr;
		Mesh::Instance* mesh = nullptr;

		void SetObject(asIScriptObject* object, vector<int>* mapping);
		void SetTransform(const Matrix& transform);
		Matrix GetTransform();
		Matrix GetLocatorTransform(const string& name);
		void SetColor(Vector3& color);
		void SetVisible(int set_visible);
		bool IsVisible();
		Vector3 dir = 0.0f;
		uint32_t collide_group = 1;
		uint32_t ignore_group = 0;
	};

	string scr_prefix;
	bool mapped = false;
	vector<int> mapping;
	vector<Instance> instances;
	CScriptArray* array = nullptr;

#ifndef DOXYGEN_SKIP

	MeshInstance();
	virtual ~MeshInstance() = default;

	void Init() override;
	void ApplyProperties() override;

	void BindClassToScript() override;
	bool InjectIntoScript(const char* type_name, int type, void* property, const char* prefix) override;
	void MakeMapping(asIScriptObject* object, const char* prefix);

	void Load(JSONReader& reader) override;
	void Save(JSONWriter& writer) override;

	void Draw(float dt);

	bool Is3DObject();

	void OnResize(int at, int delta) override;
	void OnRemove(int start, asUINT count) override;

	int AddInstance();
	void RemoveInstance(int index);
	void ClearInstances();

	void Release() override;

#ifdef EDITOR
	int sel_inst = -1;
	bool CheckSelection(Vector2 ms, Vector3 start, Vector3 dir) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
#endif

#endif
};
