
#include "ScriptCore.h"
#include "Services/Core/Core.h"
#include "Services/Scene/Scene.h"
#include "Services/Scene/SceneManager.h"
#include "Services/Scene/SceneObject.h"
#include "SceneObjects/2D/Sprite.h"
#include "SceneObjects/2D/Sprite.h"

#undef GetObject

void ScriptCore::Render::DebugText(float x, float y, string& text)
{
	core.render.DebugPrintText({ x, y}, COLOR_WHITE, text.c_str());
}

void ScriptCore::Render::DebugLine2D(Vector2 p1, Vector2 p2)
{
	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;
	p1 = p1 * scale - Sprite::cam_pos * scale + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
	p2 = p2 * scale - Sprite::cam_pos * scale + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

	core.render.DebugLine2D(p1, COLOR_WHITE, p2, COLOR_WHITE);
}

void ScriptCore::Render::DebugLine2DScr(float x1, float y1, float x2, float y2)
{
	DebugLine2D(Vector2(x1, y1), Vector2(x2, y2));
}

int ScriptCore::Render::GetWidth()
{
	return core.render.GetDevice()->GetWidth();
}

int ScriptCore::Render::GetHeight()
{
	return core.render.GetDevice()->GetHeight();
}

SoundInstance * ScriptCore::Sound::CreateSound(string& file_name)
{
	return core.sounds.CreateSound(core.scripts.script_caller, file_name.c_str());
}

SoundStream* ScriptCore::Sound::CreateStream(string& file_name)
{
	return core.sounds.CreateStream(core.scripts.script_caller, file_name.c_str());
}

int ScriptCore::Controls::GetAliasIndex(string& alias)
{
	return core.controls.GetAlias(alias.c_str());
}

int ScriptCore::Controls::GetState(int alias, int action)
{
	return core.controls.GetAliasState(alias, (::Controls::AliasAction)action);
}

float ScriptCore::Controls::GetValue(int alias, int delta)
{
	return core.controls.GetAliasValue(alias, delta);
}

int ScriptCore::Controls::GetDebugState(string& alias, int action)
{
	return core.controls.DebugKeyPressed(alias.c_str(), (::Controls::AliasAction)action);
}

int ScriptCore::Controls::IsGamepadConnected()
{
	return core.controls.IsGamepadConnected() ? 1 : 0;
}

void ScriptCore::Scene::SetStateToGroup(string& group_name, int state)
{
	core.scene_manager.SetScenesGroupsState(group_name.c_str(), state);
}

void ScriptCore::Scene::Load(string& scene_name)
{
	core.scene_manager.LoadScene(scene_name.c_str());
}

void ScriptCore::Scene::Unload(string& scene_name)
{
	core.scene_manager.UnloadScene(scene_name.c_str());
}

int ScriptCore::Scene::Raycast2D(float origin_x, float origin_y, float dir_x, float dir_y, float dist, int group, float& hit_x, float& hit_y, float& normal_x, float& normal_y, string& object, int& index)
{
	PhysScene::RaycastDesc rcdesc;

	float scale = 1.0f / 50.0f;

	rcdesc.origin = Vector(origin_x * scale, -origin_y * scale, 0.0f);
	rcdesc.dir = Vector(dir_x, -dir_y, 0.0f);
	rcdesc.length = dist * scale;
	rcdesc.group = group;

	if (core.scene_manager.PScene()->RayCast(rcdesc))
	{
		hit_x = rcdesc.hitPos.x * 50.0f;
		hit_y = -rcdesc.hitPos.y * 50.0f;

		normal_x = rcdesc.hitNormal.x;
		normal_y = -rcdesc.hitNormal.y;
		object = rcdesc.userdata->object->GetName();
		index = rcdesc.userdata->index;

		return 1;
	}

	return 0;
}

void ScriptCore::Scene::CallClassInstancesMethod(string& scene_name, string& class_name, string& method_name)
{
	core.scripts.CallClassInstancesMethod(scene_name.c_str(), class_name.c_str(), method_name.c_str());
}

void ScriptCore_Scene_Raycast2D(asIScriptGeneric *gen)
{
	ScriptCore::Scene* scene = (ScriptCore::Scene*)gen->GetObject();

	float hit_x, hit_y, normal_x, normal_y;
	string object;
	int index;

	float scale = 1.0f / 50.0f;

	if (scene->Raycast2D(gen->GetArgFloat(0), gen->GetArgFloat(1), gen->GetArgFloat(2), gen->GetArgFloat(3), gen->GetArgFloat(4), gen->GetArgDWord(5), hit_x, hit_y, normal_x, normal_y, object, index))
	{
		*((float*)(gen->GetArgAddress(6))) = hit_x;
		*((float*)(gen->GetArgAddress(7))) = hit_y;

		*((float*)(gen->GetArgAddress(8))) = normal_x;
		*((float*)(gen->GetArgAddress(9))) = normal_y;
		*((string*)(gen->GetArgAddress(10))) = object;
		*((int*)(gen->GetArgAddress(11))) = index;

		gen->SetReturnDWord(1);

		return;
	}

	gen->SetReturnDWord(0);
}

int ScriptCore::Utils::IsPointInTriangle(Vector2& pt, Vector2& p1, Vector2& p2, Vector2& p3)
{
	polygon.clear();
	polygon.push_back(p1);
	polygon.push_back(p2);
	polygon.push_back(p3);

	DrawPolygon(polygon);

	return MathUtils::IsPointInPolygon(pt, polygon);
}

int ScriptCore::Utils::IsPointInRectangle(Vector2& pt, Vector2& center, Vector2& offset, Vector2& size, float angle)
{
	polygon = { { 0.0f, size.y * 0.5f },{ size.x, size.y * 0.5f }, { size.x, -size.y * 0.5f }, { 0.0f, -size.y * 0.5f } };

	float cs = cosf(angle);
	float sn = sinf(angle);

	Vector2 local_offset = offset * size * 0.5f;

	for (int i = 0; i < 4; i++)
	{
		polygon[i] = polygon[i] - local_offset;
		polygon[i] = Vector2{ polygon[i].x * cs - polygon[i].y * sn, polygon[i].x * sn + polygon[i].y * cs} + center;
	}

	DrawPolygon(polygon);

	return MathUtils::IsPointInPolygon(pt, polygon);
}

int ScriptCore::Utils::IsPointInSector(Vector2& pt, Vector2& center, float orientation, float distance, float angle)
{
	float side_angle = orientation - angle * 0.5f;
	Vector2 p2(cosf(side_angle) * distance + center.x, sinf(side_angle) * distance + center.y);

	side_angle = orientation + angle * 0.5f;
	Vector2 p3(cosf(side_angle) * distance + center.x, sinf(side_angle) * distance + center.y);

	return IsPointInTriangle(pt, center, p2, p3);
}

void ScriptCore_Utils_IsPointInTriangle(asIScriptGeneric *gen)
{
	ScriptCore::Utils* utils = (ScriptCore::Utils*)gen->GetObject();

	Vector2* pt = (Vector2*)gen->GetArgObject(0);
	Vector2* p1 = (Vector2*)gen->GetArgObject(1);
	Vector2* p2 = (Vector2*)gen->GetArgObject(2);
	Vector2* p3 = (Vector2*)gen->GetArgObject(3);

	gen->SetReturnDWord(utils->IsPointInTriangle(*pt, *p1, *p2, *p3));
}

void ScriptCore_Utils_IsPointInRectangle(asIScriptGeneric *gen)
{
	ScriptCore::Utils* utils = (ScriptCore::Utils*)gen->GetObject();

	Vector2* pt = (Vector2*)gen->GetArgObject(0);
	Vector2* start = (Vector2*)gen->GetArgObject(1);
	Vector2* offset = (Vector2*)gen->GetArgObject(2);
	Vector2* size = (Vector2*)gen->GetArgObject(3);

	gen->SetReturnDWord(utils->IsPointInRectangle(*pt, *start, *offset, *size, gen->GetArgFloat(4)));
}

void ScriptCore_Utils_IsPointInSector(asIScriptGeneric *gen)
{
	ScriptCore::Utils* utils = (ScriptCore::Utils*)gen->GetObject();

	Vector2* pt = (Vector2*)gen->GetArgObject(0);
	Vector2* center = (Vector2*)gen->GetArgObject(1);

	gen->SetReturnDWord(utils->IsPointInSector(*pt, *center, gen->GetArgFloat(2), gen->GetArgFloat(3), gen->GetArgFloat(4)));
}

void ScriptCore::Log(string& text)
{
	core.Log("Script", text.c_str());
}

void ScriptCore::Register(asIScriptEngine* engine)
{
	const char* script_class_name = nullptr;

	script_class_name = "ScriptRender";
	core.scripts.RegisterObjectType(script_class_name, sizeof(ScriptCore::Render), "gr_script_core", "Script render sub system");
	core.scripts.RegisterObjectMethod(script_class_name, "void DebugText(float x, float y, string&in text)", WRAP_MFN(ScriptCore::Render, DebugText), "Print debug text");
	core.scripts.RegisterObjectMethod(script_class_name, "void DebugLine2D(float x, float y, float to_x, float to_y)", WRAP_MFN(ScriptCore::Render, DebugLine2DScr), "Print debug line in 2D space");
	core.scripts.RegisterObjectMethod(script_class_name, "int GetWidth()", WRAP_MFN(ScriptCore::Render, GetWidth), "Get width of a screen");
	core.scripts.RegisterObjectMethod(script_class_name, "int GetHeight()", WRAP_MFN(ScriptCore::Render, GetHeight), "Get height of a screen");

	script_class_name = "ScriptControls";
	core.scripts.RegisterObjectType(script_class_name, sizeof(ScriptCore::Controls), "gr_script_core", "Script controls sub system");
	core.scripts.RegisterObjectMethod(script_class_name, "int GetAliasIndex(string&in alias)", WRAP_MFN(ScriptCore::Controls, GetAliasIndex), "Get index alias by name");
	core.scripts.RegisterObjectMethod(script_class_name, "int GetState(int alias_index, int action)", WRAP_MFN(ScriptCore::Controls, GetState), "Get state of an alias");
	core.scripts.RegisterObjectMethod(script_class_name, "float GetValue(int alias_index, int delta)", WRAP_MFN(ScriptCore::Controls, GetValue), "Get value of an alias");
	core.scripts.RegisterObjectMethod(script_class_name, "int GetDebugState(string&in alias, int action)", WRAP_MFN(ScriptCore::Controls, GetDebugState), "Get state of hardware alias");
	core.scripts.RegisterObjectMethod(script_class_name, "int IsGamepadConnected()", WRAP_MFN(ScriptCore::Controls, IsGamepadConnected), "Check if gamepad connected");

	script_class_name = "ScriptScene";
	core.scripts.RegisterObjectType(script_class_name, sizeof(ScriptCore::Scene), "gr_script_core", "Script scene sub system");
	core.scripts.RegisterObjectMethod(script_class_name, "int SetStateToGroup(string&in group_name, int state)", WRAP_MFN(ScriptCore::Scene, SetStateToGroup), "Set state to scene objects in a group by griup name");
	core.scripts.RegisterObjectMethod(script_class_name, "int Load(string&in scene_name)", WRAP_MFN(ScriptCore::Scene, Load), "Load scene");
	core.scripts.RegisterObjectMethod(script_class_name, "float Unload(string&in scene_name)", WRAP_MFN(ScriptCore::Scene, Unload), "Unload scene");
	core.scripts.RegisterObjectMethod(script_class_name, "int Raycast2D(float origin_x, float origin_y, float dir_x, float dir_y, float dist, int group, float&out hit_y, float&out hit_x, float&out normal_x, float&out normal_y, string&out object, int&out index)", asFUNCTION(ScriptCore_Scene_Raycast2D), "Make raycast in physical scene");
	core.scripts.RegisterObjectMethod(script_class_name, "void CallClassInstancesMethod(string&in scene_name, string&in class_name, string&in method)", WRAP_MFN(ScriptCore::Scene, CallClassInstancesMethod), "Call methos in instances of script classes");

	script_class_name = "SoundInstance";
	core.scripts.RegisterObjectType(script_class_name, sizeof(SoundInstance), "gr_script_core", "Script sound instance");
	core.scripts.RegisterObjectMethod(script_class_name, "bool Play(int play_type)", WRAP_MFN(SoundInstance, Play), "Start playing a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "bool IsPlaying()", WRAP_MFN(SoundInstance, IsPlaying), "Check if sound is playing");
	core.scripts.RegisterObjectMethod(script_class_name, "void SetVolume(float volume)", WRAP_MFN(SoundInstance, SetVolume), "Set volume of a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "float GetVolume()", WRAP_MFN(SoundInstance, GetVolume), "Get volume of a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "void Stop()", WRAP_MFN(SoundInstance, Stop), "Stop playing of a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "void Release()", WRAP_MFN(SoundInstance, Release), "Release sound");

	script_class_name = "SoundStream";
	core.scripts.RegisterObjectType(script_class_name, sizeof(SoundInstance), "gr_script_core", "Script sound stream");
	core.scripts.RegisterObjectMethod(script_class_name, "bool Play(int play_type)", WRAP_MFN(SoundStream, Play), "Start playing a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "void Pause(bool pause)", WRAP_MFN(SoundStream, Pause), "Pause/unpause playing a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "void Stop()", WRAP_MFN(SoundStream, Stop), "Stop playing of a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "void SetVolume(float volume)", WRAP_MFN(SoundStream, SetVolume), "Set volume of a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "float GetVolume()", WRAP_MFN(SoundStream, GetVolume), "Get volume of a sound");
	core.scripts.RegisterObjectMethod(script_class_name, "void Release()", WRAP_MFN(SoundStream, Release), "Release sound");

	script_class_name = "ScriptSound";
	core.scripts.RegisterObjectType(script_class_name, sizeof(ScriptCore::Sound), "gr_script_core", "Script sound sub system");
	core.scripts.RegisterObjectMethod(script_class_name, "SoundInstance@ CreateSound(string&in file_name)", WRAP_MFN(ScriptCore::Sound, CreateSound), "Create a sound instance");
	core.scripts.RegisterObjectMethod(script_class_name, "SoundStream@ CreateStream(string&in file_name)", WRAP_MFN(ScriptCore::Sound, CreateStream), "Create a sound stream");

	script_class_name = "ScriptUtils";
	core.scripts.RegisterObjectType(script_class_name, sizeof(ScriptCore::Utils), "gr_script_core", "Script utility class");
	core.scripts.RegisterObjectMethod(script_class_name, "int IsPointInTriangle(Vector2&in pt, Vector2&in p1, Vector2&in p2, Vector2&in p3)", asFUNCTION(ScriptCore_Utils_IsPointInTriangle), "Check if point inside of a triangle");
	core.scripts.RegisterObjectMethod(script_class_name, "int IsPointInRectangle(Vector2&in pt, Vector2&in center, Vector2&in offset, Vector2&in size, float angle)", asFUNCTION(ScriptCore_Utils_IsPointInRectangle), "Check if point inside of a triangle");
	core.scripts.RegisterObjectMethod(script_class_name, "int IsPointInSector(Vector2&in pt, Vector2&in center, float orientation, float distance, float angle)", asFUNCTION(ScriptCore_Utils_IsPointInSector), "Check if point inside of a sector");

	script_class_name = "ScriptCore";
	core.scripts.RegisterObjectType(script_class_name, sizeof(ScriptCore), "gr_script_core", "Script core class which have access to engine sub systems");
	core.scripts.RegisterObjectProperty(script_class_name, "ScriptControls controls", offsetof(ScriptCore, controls), "Access to controls sub system");
	core.scripts.RegisterObjectProperty(script_class_name, "ScriptRender render", offsetof(ScriptCore, render), "Access to render sub system");
	core.scripts.RegisterObjectProperty(script_class_name, "ScriptScene scene", offsetof(ScriptCore, scene), "Access to scene sub system");
	core.scripts.RegisterObjectProperty(script_class_name, "ScriptSound sound", offsetof(ScriptCore, sound), "Access to sound sub system");
	core.scripts.RegisterObjectProperty(script_class_name, "ScriptUtils utils", offsetof(ScriptCore, utils), "Access to utility class");

	core.scripts.RegisterObjectMethod(script_class_name, "void Log(string&in text)", WRAP_MFN(ScriptCore, Log), "Log a message into file");

	engine->RegisterGlobalProperty("ScriptCore core", this);
}