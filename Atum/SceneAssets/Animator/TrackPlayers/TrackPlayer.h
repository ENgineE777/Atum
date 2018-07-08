#pragma once

#include "Support/ClassFactory.h"
#include "Support/MetaData/MetaData.h"

#include "../DataTrack/FloatDataTrack.h"
#include "../DataTrack/TransformDataTrack.h"
#include "../DataTrack/Transform2DDataTrack.h"
#include "../DataTrack/ColorDataTrack.h"


class TrackPlayer : public Object
{
	char name[64];
	char type[64];
	bool active = true;

public:

	void DataToGizmo() {};

	typedef void (TrackPlayer::*ChangeKeyFunc)();

	TrackPlayer();

	virtual ~TrackPlayer();

	virtual void Clear();

	virtual const char* GetName()
	{ 
		return name;
	};

	virtual void SetName(const char* nm)
	{ 
		StringUtils::Copy(name, 64, nm);
	};

	virtual const char* GetType()
	{ 
		return type;
	};

	virtual void SetType(const char* tp)
	{ 
		StringUtils::Copy(type, 64, tp);
	};

#ifdef EDITOR
	virtual void InitControls(EUIWidget* parent);
	virtual void DataFromGizmo();
	virtual void DebugDraw();
	bool IsActive() { return active; }
	virtual void SetActive(bool value);
#endif

	virtual void Init() = 0;

	virtual MetaData* GetMetaData() = 0;

	virtual void Load(JSONReader& stream);
	virtual void Save(JSONWriter& stream);

	virtual void Reset();
	virtual void SetTime(float time);

	virtual void Release();

	virtual void StartPlay(){};
	virtual void EndPlay(){};

	DataTrack* GetDataTrack(const char* name);

	class Animator* owner;
	std::vector<DataTrack*> tracks;
};

CLASSFACTORYDEF(TrackPlayer)
CLASSFACTORYDEF_END()