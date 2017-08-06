
#pragma once

#include "EUIWidget.h"
#include "Native/NativeLayout.h"

class EUILayout : public EUIWidget
{
	friend class WinLayout;

	struct ChildSize
	{
		float size;
		bool absolute;

		ChildSize()
		{
			size = -1.0f;
			absolute = false;
		}
	};

	std::vector<ChildSize> childs_size;
	NativeLayout* Native();
	bool vertical;

public:
	
	EUILayout(EUIWidget* parent, bool vertical);
	virtual ~EUILayout();

	void Resize();

	virtual void AddChild(EUIWidget* child);
	virtual void DelChild(EUIWidget* child);

	void SetChildSize(EUIWidget* child, float size, bool absolute);
};
