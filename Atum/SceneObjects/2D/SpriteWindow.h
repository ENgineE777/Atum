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
	EUIButton*   btn_del_frame;
	EUILabel*    num_frame_label;
	EUIEditBox*  num_frame_ebox;

	EUILabel*    show_anim_label;
	EUICheckBox* show_anim_box;

	EUILabel*    frame_time_label;
	EUIEditBox*  frame_time_ebox;
	EUILabel*    cur_frame_time_label;
	EUIEditBox*  cur_frame_time_ebox;
	EUILabel*    pivot_x_label;
	EUIEditBox*  pivot_x_ebox;
	EUILabel*    pivot_y_label;
	EUIEditBox*  pivot_y_ebox;

	EUILabel*    prop_color_label;
	EUILabel*    prop_color_edlabel;

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

	bool    border_drawed = false;
	int     num_frames = 1;
	std::vector<Vector2> frames;

public:

	EUIWindow* wnd;
	EUIPanel*  img_wgt;
	int     cur_frame = 0;
	bool show_anim = true;

	Texture* checker_texture = nullptr;
	static Sprite::Data* sprite;
	static SpriteWindow* instance;

	static void StartEdit(Sprite::Data* ed_sprite);
	static void StopEdit();

	void Init();
	void Show(bool sh);
	void FillPoints(int index, int stride, float val, bool vert);
	void ResizeSpriteRect();
	void UpdateSpriteRect();
	void SetImage(const char* img);
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
	void MakeZoom(float zoom);
	void CheckStateOfBorder();
	void SetColorToLabel();

	void OnUpdate(EUIWidget* sender) override;
	void OnComboBoxSelChange(EUIComboBox* sender, int index) override;
	void OnEditBoxStopEditing(EUIEditBox* sender) override;
	void OnLeftMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnMouseMove(EUIWidget* sender, int mx, int my) override;
	void OnLeftMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnLeftDoubliClick(EUIWidget* sender, int mx, int my) override;
	void OnMouseWheel(EUIWidget* sender, int delta) override;
	void OnMiddleMouseDown(EUIWidget* sender, int mx, int my) override;
	void OnMiddleMouseUp(EUIWidget* sender, int mx, int my) override;
	void OnKey(EUIWidget* sender, int key) override;
	void OnWinClose(EUIWidget* sender) override;
};

#endif