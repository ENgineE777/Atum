#pragma once

#ifdef EDITOR

#include <EUI.h>

class Animator;

class AnimatorWindow : public EUIWidget::Listener
{
public:

	enum Sizes
	{
		LeftRowWidth = 150,
		CaptionHeight = 30,
		RowHeight = 20
	};

	enum WgtID
	{
		AddPlayerID = 1001,
		DelPLayerID = 1002,
		AddKeyID = 1003,
		DelKeyID = 1004,
		PlayID = 1005,
		CurTimeID = 1006,
		TimeLengthID = 1007,
		FromTimeID = 1008,
		ToTimeID = 1009,
		ScrollHzID = 1010,
		ScrollVrID = 1011,
		TimeLineID = 1012
	};

	enum DragMode
	{
		DragNone,
		DragKey,
		DragFromTime,
		DragToTime,
		DragCurTime
	};

	EUIWindow* wnd;

	EUIComboBox* cb_addObject;
	EUIButton*   btn_delObject;

	EUIButton* btn_addKey;
	EUIButton* btn_delKey;
	EUIButton* btn_playKey;

	EUILabel*   lb_curTime;
	EUIEditBox* tb_curTime;

	EUILabel*   lb_timeLenght;
	EUIEditBox* tb_timeLenght;

	EUILabel*   lb_fromTime;
	EUIEditBox* tb_fromTime;

	EUILabel*   lb_toTime;
	EUIEditBox* tb_toTime;

	EUICategories*   prop_dock;
	EUIPanel*   timeline_panel;

	EUIScrollBar* horz_scroll;
	EUIScrollBar* vert_scroll;

	HFONT font;

	float timeOffset;

	Animator* owner;
	float scale = 100.0f;
	DragMode drag_mode = DragNone;
	int selPlayer = -1;
	int selTrack = -1;
	int selKey = -1;

	bool play = false;

	static AnimatorWindow* instance;
	
	void Init();
	void Show(bool sh);

	void PrepareWidgets(Animator* anim);
	void Clear();

	void OnLeftMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnMouseMove(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
	void OnResize(EUIWidget* sender) override;
	void OnUpdate(EUIWidget* sender) override;

	void SetSelection(int player, int track, int key);
	void SetCurTime(float time);
	void SetTimeLenght(float time);
	void SetFromTime(float time);
	void SetToTime(float time);

	void AddKey();
	void DelKey();

	void UpdateVertBar();
	void UpdateHorzBar();

	int CalcKeyPos(float ps_x);
	int CalcLinePos(int index);
	float PixelToTime(float px);
};

#endif