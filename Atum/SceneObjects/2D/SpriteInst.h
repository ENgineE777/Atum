
#pragma once

#include "Services/Scene/SceneObject.h"
#include "SpriteAsset.h"
#include "Services/Script/Libs/scriptarray.h"
#include "SpriteGraphAsset.h"

/**
\page scene_assets_2D_SpriteInst Instance

Instance holds array of sprite instances. This array can be injected into a script array.

To add new sprite instance firstly scene object needed to be selected. After that press hot key 'O'
to add new sprite instance into center of screen. If another sprite instace is selected then pressing
hot key 'P' will add another instance of a sprite near previosly selected one. Press hot key 'I' to
delete selected instance of a sprite.

Scene object supporting muliselection. That means holding Left Alt and dragging mouse after left mouse click
will select multiple scene instances. Pressing hot key 'I' will fill selection by instance. Pressing
hot key 'P' will delete evry instances inside rectangular area.

![Caption text](../images/Sprites.jpg "Image")

This class ::SpriteInst is a representation on C++ side.

This class Script::Sprite is a representation in a script.

Instance property parameters
----------------------------

Name              | Description
------------------| -------------
x                 | X coordinate of a position
y                 | Y coordinate of a position
visible           | If instance is visible. 1 means visible. 0 means invicible
color             | Color of a instance
alpha             | Transparency of a instance

Parameters
----------

Name              | Description
------------------| -------------
axis_scale        | Scale of a axis
Depth             | Z depth
draw_level        | Draw priority
inst              | Array of instances of a sprite assets

*/

/**
\ingroup gr_code_scene_objects_2D
*/

/**
\brief Representation of instance of sprite asset

Instance holds array of sprite instances. This array can be injected into a script array.

This class Script::Sprite is a representation in a script.

*/

class SpriteInst : public SceneObjectInst, public CScriptArray::Listiner
{
public:
	META_DATA_DECL(SpriteInst)

	/**
	\brief Scale of a axis
	*/

	float axis_scale = 1.0f;

	/**
	\brief Draw priority
	*/

	int draw_level = 0;

#ifndef DOXYGEN_SKIP

	Transform2D trans;
	Transform2D multi_trans;

	struct Instance
	{
		META_DATA_DECL_BASE(Instance)

		int index = 0;
		Sprite::FrameState frame_state;

		void SetObject(asIScriptObject* object, vector<int>* mapping);
		void SetPos(Vector2 pos);
		Vector2 GetPos();
		void SetFlipped(int horz_flipped);
		int  GetFlipped();
		void SetVisible(int visible);
		bool IsVisible();

		float GetAlpha();
		void  SetAlpha(float set_alpha);

		float GetSizeX();
		void  SetSizeX(float set_size);

		float GetSizeY();
		void  SetSizeY(float set_size);

		float GetAngle();
		void  SetAngle(float set_angle);

		bool ActivateLink(string& link);
		void GotoNode(string& node);

		Color color;

		bool auto_delete = false;
		bool hack_marker = false;

		SpriteGraphAsset::Instance graph_instance;
		Vector2 dir = 0.0f;

	private:
		vector<int>* mapping = nullptr;
		asIScriptObject* object = nullptr;
		Vector2 pos;
		float   size_x = -1.0f;
		float   size_y = -1.0f;
		int     visible = 1;
		float   alpha = 1.0f;
		float   angle = 0.0f;
	};

	struct InstanceHolder
	{
		int index = -1;
		Instance* inst = nullptr;
	};

	bool mapped = false;
	vector<int> mapping;
	CScriptArray* array = nullptr;

	int sel_inst = -1;
	vector<Instance> instances;

	bool rect_select = false;
	Vector2 rect_p1;
	Vector2 rect_p2;

	vector<int> sel_instances;

	string scr_prefix;

	virtual ~SpriteInst() = default;

	void BindClassToScript() override;
	bool InjectIntoScript(const char* type, void* property, const char* prefix) override;
	void MakeMapping(asIScriptObject* object, const char* prefix);

	void Init() override;
	void ApplyProperties() override;
	virtual void Draw(float dt);

	bool Play() override;
	void Release() override;

	void OnResize(int at, int delta) override;
	void OnRemove(int start, asUINT count) override;

	int AddInstance(float x, float y, bool auto_delete);
	void RemoveInstance(int index);
	void ClearInstances();

#ifdef EDITOR
	void ClearRect();
	void FillRect();
	void OnRectSelect(Vector2 p1, Vector2 p2) override;
	bool CheckSelection(Vector2 ms) override;
	void SetEditMode(bool ed) override;
	void SetGizmo();
	int  GetInstCount();
#endif

#endif
};
