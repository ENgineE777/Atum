
#pragma once

#include "Scene.h"

class SceneManager : public Object
{
	std::map<std::string, Scene*> scenes;

public:

	void Init();
	Scene* LoadScene();

};