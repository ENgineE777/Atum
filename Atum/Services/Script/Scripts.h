#pragma once
#include "ScriptContext.h"

class Scripts
{
public:
	asIScriptEngine *engine = nullptr;

public:
	void Init();
	void Start();
	ScriptContext* CreateContext();
	void Stop();
};

extern Scripts scripts;