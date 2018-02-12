#pragma once

#ifdef EDITOR

#include <EUI.h>
#include "Sprite.h"

class SpriteWindow : public EUIWidget::Listener
{
	enum Drag
	{
		DragNone,
		DragRects,
		DragField
	};

	EUIComboBox* cb_type;
	EUIComboBox* texture_mode;
	EUIComboBox* texture_filter;

	EUIButton*   btn_zoom_in;
	EUIButton*   btn_zoom_out;
	EUIButton*   btn_act_pixels;
	EUIButton*   btn_fit;
	EUIButton*   load_image;
	EUIButton*   del_image;
	EUILabel*    image_size_label;

	EUILabel*    cur_frame_label;
	EUIEditBox*  cur_frame_ebox;
	EUILabel*    num_frame_label;
	EUIEditBox*  num_frame_ebox;

	EUILabel*    frame_time_label;
	EUIEditBox*  frame_time_ebox;
	EUILabel*    cur_frame_time_label;
	EUIEditBox*  cur_frame_time_ebox;
	EUILabel*    pivot_x_label;
	EUIEditBox*  pivot_x_ebox;
	EUILabel*    pivot_y_label;
	EUIEditBox*  pivot_y_ebox;

	EUILabel*    prop_x_label;
	EUIEditBox*  prop_x_ebox;
	EUILabel*    prop_y_label;
	EUIEditBox*  prop_y_ebox;
	EUILabel*    prop_w_label;
	EUIEditBox*  prop_w_ebox;
	EUILabel*    prop_h_label;
	EUIEditBox*  prop_h_ebox;

	Vector2 sprite_pos;
	Vector2 sprite_size;
	Vector2 sprite_offset_x;
	Vector2 sprite_offset_y;

	HBITMAP image = 0;
	uint8_t* tex_data = nullptr;
	byte* imageBits = nullptr;
	float pixel_density = 1.0f;
	Vector2 points[16];
	Vector2 origin = 0.0f;
	Vector2 delta_mouse = 0.0f;
	Drag drag = DragNone;
	int rect_width;
	int rect_height;
	int sel_row = -1;
	int sel_col = -1;
	Vector2 prev_ms;

	int     cur_frame = 0;
	int     num_frames = 1;
	std::vector<Vector2> frames;

public:

	EUIWindow* wnd;
	EUIPanel*  img_wgt;

	static Sprite::Data* sprite;
	static SpriteWindow* instance;

	static void StartEdit(Sprite::Data* ed_sprite);
	static void StopEdit();

	void Init();
	void Show(bool sh);
	void FillPoints(int index, int stride, float val, bool vert);
	void ResizeSpriteRect();
	void UpdateSpriteRect();
	void SetImage(const char* img, bool need_refill);
	void UpdateImageBackground();
	void Prepare();
	void ShowFrameWidgets();
	void SetCurFrame(int frame);

	void SelectRect();
	void FillRects();
	void MoveRects(Vector2 delta);
	void UpdateAnimRect();
	void UpdateSavedPos();
	void ActualPixels();
	void FitImage();
	void MakeZoom(bool zoom_in);

	virtual void OnDraw(EUIWidget* sender);
	virtual void OnComboBoxChange(EUIWidget* sender, int index);
	virtual void OnEditBoxStopEditing(EUIWidget* sender);
	virtual void OnLeftMouseDown(EUIWidget* sender, int mx, int my);
	virtual void OnMouseMove(EUIWidget* sender, int mx, int my);
	virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
	virtual void OnRightMouseDown(EUIWidget* sender, int mx, int my);
	virtual void OnRightMouseUp(EUIWidget* sender, int mx, int my);
	virtual void OnKey(EUIWidget* sender, int key);
	virtual void OnWinClose(EUIWidget* sender);
};

#endif