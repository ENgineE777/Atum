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

	Vector2 sprite_pos;
	Vector2 sprite_size;
	Vector2 sprite_offset_x;
	Vector2 sprite_offset_y;

	HBITMAP image = 0;
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
	void UpdateSpriteRect();
	void SetImage(const char* img);
	void Prepare();

	void SelectRect();
	void FillRects();
	void MoveRects(Vector2 delta);
	void UpdateSavedPos();
	void ActualPixels();
	void FitImage();
	void MakeZoom(bool zoom_in);

	virtual void OnDraw(EUIWidget* sender);
	virtual void OnComboBoxChange(EUIWidget* sender, int index);
	virtual void OnLeftMouseDown(EUIWidget* sender, int mx, int my);
	virtual void OnMouseMove(EUIWidget* sender, int mx, int my);
	virtual void OnLeftMouseUp(EUIWidget* sender, int mx, int my);
	virtual void OnRightMouseDown(EUIWidget* sender, int mx, int my);
	virtual void OnRightMouseUp(EUIWidget* sender, int mx, int my);
	virtual void SpriteWindow::OnWinClose(EUIWidget* sender);
};

#endif