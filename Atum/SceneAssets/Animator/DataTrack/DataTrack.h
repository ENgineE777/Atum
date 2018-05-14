#pragma once

#include "Support/json/JSONReader.h"
#include "Support/json/JSONWriter.h"

#include <vector>

#ifdef EDITOR

#include <EUI.h>
#include "Editor/Gizmo.h"

#endif

class TrackPlayer;
typedef void (TrackPlayer::*ChangeKeyFunc)();

class DataTrack
{
public:

	enum Blend
	{
		BlendNone = 0,
		BlendLinear = 1,
		BlendEasyIn = 2,
		BlendEasyOut = 3
	};

	struct Key
	{
		Blend blend;
		float time;
	};

	std::vector<Key> keys;

#ifdef EDITOR
	enum AllowedBlend
	{
		AllowedBlendNone,
		AllowedBlendOnlyLinear,
		AllowedBlendAll
	};
	static EUIPanel* timeline;
#endif

	virtual ~DataTrack() {};

	virtual const char* GetName() = 0;
	virtual void Load(JSONReader& reader, int num) = 0;
	virtual void Save(JSONWriter& writer) = 0;
	virtual void Reset() = 0;
	virtual void CalcValue(int index, float blend) = 0;
	virtual void SetTime(float time) = 0;
	virtual int  GetKeysCount() = 0;

#ifdef EDITOR
	virtual Key GetKey(int key)
	{
		return keys[key];
	};
	virtual int  AddKey(float time) = 0;
	virtual void SetDefaultValue(int index) = 0;
	virtual void SetCurrentValue(int index) = 0;
	virtual void DelKey(int index) = 0;
	virtual void InitControls(EUIWidget* parent) = 0;

	virtual void StartEditKey(int key) = 0;
	virtual void StopEditKey() = 0;

	virtual void SetKeyTime(int key, float time) = 0;
	virtual void DataFromGizmo() {};
	
	virtual void DebugDraw() {};
#endif
};

template<typename DataType>
class TemplDataTrack : public DataTrack
{
protected:

	int cur_index;
	float prevTime;
	TrackPlayer* owner;
	std::vector<DataType> values;

	char name[64];
	ChangeKeyFunc changeKeyFunc;

	DataType* value;

public:

	DataType def_value;
#ifdef EDITOR
	AllowedBlend blendRage = AllowedBlendOnlyLinear;
#endif

	TemplDataTrack(TrackPlayer* own, const char* nm, ChangeKeyFunc changeKey_Func)
	{
		owner = own;
		strcpy(name, nm);
		changeKeyFunc = changeKey_Func;
		cur_index = 0;
		prevTime = -0.1f;
	};

	virtual ~TemplDataTrack()
	{
		keys.clear();
	};

	virtual const char* GetName()
	{ 
		return name;
	};
	
	virtual void Reset()
	{ 
		cur_index = 0;
	};

	virtual void SetTime(float time)
	{
		if ((int)keys.size() == 0) return;

		if ((int)keys.size() == 1 || keys[0].time >= time)
		{
			CalcValue(0, -1.0f);
			if (prevTime == 0.f)
			{
				if (prevTime <= keys[0].time &&  keys[0].time < time && prevTime != time)
				{
					if (changeKeyFunc)
					{
						(owner->*changeKeyFunc)();
					}
				}
			}
			else
			if (prevTime < keys[0].time &&  keys[0].time <= time && prevTime != time)
			{
				if (changeKeyFunc)
				{
					(owner->*changeKeyFunc)();
				}
			}
		}
		else
		{
			if (keys[(int)keys.size()-1].time<=time)
			{
				if (cur_index<=(int)keys.size() - 1)
				{
					while (cur_index<=(int)keys.size() - 1)
					{
						if (changeKeyFunc)
						{
							(owner->*changeKeyFunc)();
						}

						cur_index++;
					}
				}

				CalcValue((int)keys.size() - 1, -1.0f);
			}
			else
			{
				while (keys[cur_index].time <= time)
				{
					if (changeKeyFunc)
					{
						(owner->*changeKeyFunc)();
					}

					cur_index++;
				}

				if (keys[cur_index - 1].blend)
				{
					CalcValue(cur_index,
					          (time - keys[cur_index - 1].time) /
					          (keys[cur_index].time - keys[cur_index - 1].time));
				}
				else
				{
					CalcValue(cur_index - 1, -1.0f);
				}
			}
		}

		prevTime = time;
	}

	virtual int GetKeysCount()
	{
		return (int)keys.size();
	}

#ifdef EDITOR
	virtual void SetDefaultValue(int index)
	{
		values[index] = def_value;
	}

	virtual void SetCurrentValue(int index)
	{
		values[index] = *value;
	}

	virtual int AddKey(float time)
	{
		for (int i=0;i<(int)keys.size();i++)
		{
			if (fabs(keys[i].time - time)<0.01f)
			{
				return -1;
				break;
			}
		}

		keys.push_back(Key());
		values.push_back(DataType());
		
		int index = (int)keys.size() - 1;
		keys[index].time = time;
		keys[index].blend = (blendRage != AllowedBlendNone) ? BlendLinear : BlendNone;

		for (int i = (int)keys.size() - 1; i > 0; i--)
		{
			if (keys[i].time < keys[i-1].time)
			{
				std::swap(keys[i], keys[i-1]);
				std::swap(values[i], values[i - 1]);

				index--;
				continue;
			}

			break;
		}

		if (keys.size() == 1)
		{
			SetDefaultValue(index);
		}
		else
		if (index == 0)
		{
			values[index] = values[index + 1];
		}
		else
		if (index == keys.size() - 1)
		{
			values[index] = values[index - 1];
		}
		else
		{
			SetCurrentValue(index);
		}

		return index;
	}

	virtual void DelKey(int index)
	{
		if (!keys[index].blend)
		{
			if (index>0)
			{
				keys[index-1].blend = BlendNone;
			}
		}

		keys.erase(keys.begin() + index);
		values.erase(values.begin() + index);

		cur_index = 0;
	}

	virtual void SetKeyTime(int key, float time)
	{
		keys[key].time = time;
	}
#endif
};