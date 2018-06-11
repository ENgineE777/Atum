
#ifdef EDITOR

#include "Support/stb/stb_image.h"
#include "SpriteWindow.h"
#include "Services/Controls/Controls.h"

Sprite::Data* SpriteWindow::sprite = nullptr;
SpriteWindow* SpriteWindow::instance = nullptr;

void SpriteWindow::StartEdit(Sprite::Data* ed_sprite)
{
	if (!instance)
	{
		instance = new SpriteWindow();
		instance->Init();
	}

	sprite = ed_sprite;
	instance->Prepare();
}

void SpriteWindow::StopEdit()
{
	if (instance)
	{
		instance->Show(false);
	}
}

void SpriteWindow::Init()
{
	wnd = new EUIWindow("Sprite Editor", EUIWindow::PopupWithCloseBtn, false, 10, 35, 940, 640);
	wnd->SetListener(-1, this, 0);

	EUILayout* wnd_lt = new EUILayout(wnd, false);
	EUIPanel* panel = new EUIPanel(wnd_lt, 0, 0, 100, 100);
	wnd_lt->SetChildSize(panel, 200, false);

	btn_zoom_in = new EUIButton(panel, "+", 5, 5, 20, 20);
	btn_zoom_in->SetListener(-1, this, 0);

	btn_zoom_out = new EUIButton(panel, "-", 30, 5, 20, 20);
	btn_zoom_out->SetListener(-1, this, 0);

	btn_act_pixels = new EUIButton(panel, "Actual Pixels", 60, 5, 70, 20);
	btn_act_pixels->SetListener(-1, this, 0);

	btn_fit = new EUIButton(panel, "Fit", 135, 5, 60, 20);
	btn_fit->SetListener(-1, this, 0);

	EUILabel* prop_label = nullptr;

	image_size_label = new EUILabel(panel, "Image Size: 0 x 0", 5, 40, 195, 20);

	prop_label = new EUILabel(panel, "Source", 5, 70, 90, 20);

	load_image = new EUIButton(panel, "", 95, 70, 80, 20);
	load_image->SetListener(-1, this, 0);

	del_image = new EUIButton(panel, "X", 175, 70, 20, 20);
	del_image->SetListener(-1, this, 0);

	prop_x_label = new EUILabel(panel, "x", 5, 100, 90, 20);
	prop_x_ebox = new EUIEditBox(panel, "0", 95, 100, 100, 20, EUIEditBox::InputInteger);
	prop_x_ebox->SetListener(-1, this, 0);
	prop_y_label = new EUILabel(panel, "y", 5, 130, 90, 20);
	prop_y_ebox = new EUIEditBox(panel, "0", 95, 130, 100, 20, EUIEditBox::InputInteger);
	prop_y_ebox->SetListener(-1, this, 0);
	prop_w_label = new EUILabel(panel, "width", 5, 160, 90, 20);
	prop_w_ebox = new EUIEditBox(panel, "0", 95, 160, 100, 20, EUIEditBox::InputUInteger);
	prop_w_ebox->SetListener(-1, this, 0);
	prop_h_label = new EUILabel(panel, "height", 5, 190, 90, 20);
	prop_h_ebox = new EUIEditBox(panel, "0", 95, 190, 100, 20, EUIEditBox::InputUInteger);
	prop_h_ebox->SetListener(-1, this, 0);

	prop_label = new EUILabel(panel, "Tile Mode", 5, 220, 90, 20);
	texture_mode = new EUIComboBox(panel, 95, 220, 100, 250);
	texture_mode->SetListener(-1, this, 0);
	texture_mode->AddItem("clamp");
	texture_mode->AddItem("wrap");
	texture_mode->AddItem("mirror");
	texture_mode->SetCurString(0);

	prop_label = new EUILabel(panel, "Filter", 5, 250, 90, 20);
	texture_filter = new EUIComboBox(panel, 95, 250, 100, 250);
	texture_filter->SetListener(-1, this, 0);
	texture_filter->AddItem("point");
	texture_filter->AddItem("linear");
	texture_filter->SetCurString(0);

	prop_label = new EUILabel(panel, "Type", 5, 280, 90, 20);
	cb_type = new EUIComboBox(panel, 95, 280, 100, 250);
	cb_type->SetListener(-1, this, 0);
	cb_type->AddItem("Image");
	cb_type->AddItem("3 Horz slice");
	cb_type->AddItem("3 Vert slice");
	cb_type->AddItem("9 slice");
	cb_type->AddItem("Frames");
	cb_type->SetCurString(0);

	num_frame_label = new EUILabel(panel, "Frames Count", 5, 310, 90, 20);
	num_frame_ebox = new EUIEditBox(panel, "1", 95, 310, 100, 20, EUIEditBox::InputUInteger);
	num_frame_ebox->SetListener(-1, this, 0);

	frame_time_label = new EUILabel(panel, "Frames Time", 5, 340, 90, 20);
	frame_time_ebox = new EUIEditBox(panel, "0", 95, 340, 100, 20, EUIEditBox::InputUFloat);
	frame_time_ebox->SetListener(-1, this, 0);

	cur_frame_label = new EUILabel(panel, "Current Frame", 5, 370, 90, 20);
	cur_frame_ebox = new EUIEditBox(panel,"0", 95, 370, 80, 20, EUIEditBox::InputUInteger);
	cur_frame_ebox->SetListener(-1, this, 0);
	btn_del_frame = new EUIButton(panel, "X", 175, 370, 20, 20);
	btn_del_frame->SetListener(-1, this, 0);

	show_anim_label = new EUILabel(panel, "Frames Time", 5, 400, 90, 20);
	show_anim_box = new EUICheckBox(panel, "", 95, 400, 90, 20);
	show_anim_box->SetChecked(show_anim);
	show_anim_box->SetListener(-1, this, 0);

	cur_frame_time_label = new EUILabel(panel, "Frame Time", 5, 430, 90, 20);
	cur_frame_time_ebox = new EUIEditBox(panel, "1", 95, 430, 100, 20, EUIEditBox::InputFloat);
	cur_frame_time_ebox->SetListener(-1, this, 0);

	pivot_x_label = new EUILabel(panel, "Pivot x", 5, 460, 90, 20);
	pivot_x_ebox = new EUIEditBox(panel, "0", 95, 460, 100, 20, EUIEditBox::InputInteger);
	pivot_x_ebox->SetListener(-1, this, 0);
	pivot_y_label = new EUILabel(panel, "Pivot y", 5, 490, 90, 20);
	pivot_y_ebox = new EUIEditBox(panel, "1", 95, 490, 100, 20, EUIEditBox::InputInteger);
	pivot_y_ebox->SetListener(-1, this, 0);

	img_wgt = new EUIPanel(wnd_lt, 270, 5, 700, 550);
	img_wgt->SetListener(-1, this, EUIWidget::OnDraw);
}

void SpriteWindow::Show(bool sh)
{
	wnd->Show(sh);
}

void SpriteWindow::FillPoints(int index, int stride, float val, bool vert)
{
	if (vert)
	{
		points[index + stride * 0] = Vector2(val, sprite_pos.y);
		points[index + stride * 1] = Vector2(val, sprite_pos.y - sprite_offset_y.x);
		points[index + stride * 2] = Vector2(val, sprite_pos.y - sprite_size.y + sprite_offset_y.y);
		points[index + stride * 3] = Vector2(val, sprite_pos.y - sprite_size.y);
	}
	else
	{
		points[index + 0] = Vector2(sprite_pos.x, val);
		points[index + 1] = Vector2(sprite_pos.x + sprite_offset_x.x, val);
		points[index + 2] = Vector2(sprite_pos.x + sprite_size.x - sprite_offset_x.y, val);
		points[index + 3] = Vector2(sprite_pos.x + sprite_size.x, val);
	}
}

void SpriteWindow::SetImage(const char* img, bool need_refill)
{
	if (image)
	{
		DeleteObject(image);
		image = 0;

		free(tex_data);
		tex_data = nullptr;
	}

	sprite->tex_name = img;
	sprite->LoadTexture();

	load_image->SetText(img);

	if (!sprite->texture)
	{
		return;
	}

	BITMAPINFO bminfo;
	ZeroMemory(&bminfo, sizeof(bminfo));
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biWidth = sprite->width;
	bminfo.bmiHeader.biHeight = sprite->height;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;

	HDC hdcScreen = GetDC(NULL);
	image = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, (void**)&imageBits, NULL, 0);
	ReleaseDC(NULL, hdcScreen);

	Buffer buffer;

	if (!buffer.Load(img))
	{
		return;
	}

	uint8_t* ptr = buffer.GetData();

	int bytes;
	int width;
	int height;
	tex_data = stbi_load_from_memory(ptr, buffer.GetSize(), &width, &height, &bytes, STBI_rgb_alpha);

	if (need_refill)
	{
		sprite_pos = Vector2(0, (float)sprite->height);
		sprite_size = Vector2((float)sprite->width, (float)sprite->height);
		sprite_offset_x = 10.0f;
		sprite_offset_y = 10.0f;

		if (sprite->rects.size() == 0)
		{
			ResizeSpriteRect();
		}

		FillRects();
		UpdateAnimRect();
		UpdateSavedPos();
		UpdateSpriteRect();
		FitImage();
	}

	char str[128];
	StringUtils::Printf(str, 128, "Image Size: %i x %i", sprite->width, sprite->height);
	image_size_label->SetText(str);
}

void SpriteWindow::UpdateImageBackground()
{
	Color back_colors[2];
	back_colors[0] = COLOR_GRAY;
	back_colors[1] = COLOR_LIGHT_GRAY;

	float grid_size = floor(20.0f / pixel_density + 0.5f);

	for (int j = 0; j < sprite->height; j++)
	{
		byte* rows = &imageBits[(sprite->height - 1 - j) * sprite->width * 4];

		for (int i = 0; i < sprite->width; i++)
		{
			int sel_back_colors = (int)(i / grid_size) % 2 + (int)(j / grid_size) % 2;

			if (sel_back_colors == 2)
			{
				sel_back_colors = 0;
			}

			uint8_t* color = &tex_data[(i + j * sprite->width) * 4];
			float k = color[3] / 255.0f;

			rows[i * 4 + 0] = (int)(k * color[2] + back_colors[sel_back_colors].b * 255 * (1 - k));
			rows[i * 4 + 1] = (int)(k * color[1] + back_colors[sel_back_colors].g * 255 * (1 - k));
			rows[i * 4 + 2] = (int)(k * color[0] + back_colors[sel_back_colors].r * 255 * (1 - k));
			rows[i * 4 + 3] = 255;
		}
	}
}

void SpriteWindow::ResizeSpriteRect()
{
	int count = 1;

	if (sprite->type == Sprite::ThreeSlicesHorz || sprite->type == Sprite::ThreeSlicesVert)
	{
		count = 3;
	}

	if (sprite->type == Sprite::NineSlices)
	{
		count = 9;
	}

	if (sprite->type == Sprite::Frames)
	{
		count = (int)frames.size() / 4;
	}

	sprite->rects.resize(count);
}

void SpriteWindow::UpdateSpriteRect()
{
	int index = 0;

	Vector2 image_size((float)sprite->width, (float)sprite->height);

	if (sprite->type == Sprite::Frames)
	{
		for (int i = 0; i<sprite->rects.size(); i++)
		{
			Sprite::Rect& rect = sprite->rects[i];

			rect.pos = frames[i * 4 + 0];
			rect.size = frames[i * 4 + 3] - frames[i * 4 + 0];
			rect.size.y = -rect.size.y;

			rect.uv = rect.pos / image_size;
			rect.duv = rect.size / image_size;
		}

		return;
	}

	if (sprite->rects.size() == 0)
	{
		return;
	}

	for (int i = 0; i<rect_height - 1; i++)
		for (int j = 0; j < rect_width - 1; j++)
		{
			int indx = rect_width * i + j;
			Sprite::Rect& rect = sprite->rects[index];

			rect.pos = points[indx];
			rect.size = points[indx + 1 + rect_width] - points[indx];
			rect.size.y = -rect.size.y;

			rect.uv = rect.pos / image_size;
			rect.duv = rect.size / image_size;

			index++;
		}
}

void SpriteWindow::Prepare()
{
	if (sprite->type == Sprite::Frames)
	{
		num_frames = (int)sprite->rects.size();
	}
	else
	{
		num_frames = 1;
	}

	cur_frame = 0;
	frames.resize(num_frames * 4);

	SetImage(sprite->tex_name.c_str(), false);
	Show(true);
	ShowFrameWidgets();

	SelectRect();

	if (sprite->texture)
	{
		int index = 0;

		if (sprite->type == Sprite::Frames)
		{
			for (int i = 0; i<sprite->rects.size(); i++)
			{
				Sprite::Rect& rect = sprite->rects[i];

				frames[i * 4 + 0] = rect.pos;
				frames[i * 4 + 1] = Vector2(rect.pos.x + rect.size.x, rect.pos.y);
				frames[i * 4 + 2] = Vector2(rect.pos.x, rect.pos.y - rect.size.y);
				frames[i * 4 + 3] = Vector2(rect.pos.x + rect.size.x, rect.pos.y - rect.size.y);
			}
		}

		for (int i = 0; i<rect_height - 1; i++)
			for (int j = 0; j < rect_width - 1; j++)
			{
				int indx = rect_width * i + j;
				Sprite::Rect& rect = sprite->rects[index];

				points[indx] = rect.pos;

				if (j == rect_width - 2)
				{
					points[indx + 1] = Vector2(rect.pos.x + rect.size.x, rect.pos.y);
				}

				if (i == rect_height - 2)
				{
					points[indx + rect_width] = Vector2(rect.pos.x, rect.pos.y - rect.size.y);
				}

				if (j == rect_width - 2 && i == rect_height - 2)
				{
					points[indx + rect_width + 1] = Vector2(rect.pos.x + rect.size.x, rect.pos.y - rect.size.y);
				}

				index++;
			}

		UpdateSavedPos();
	}

	if (sprite->type == Sprite::Frames)
	{
		cur_frame_ebox->SetText("0");
		num_frame_ebox->SetText(num_frames);

		frame_time_ebox->SetText(sprite->frame_time);
		cur_frame_time_ebox->SetText(sprite->rects[0].frame_time);
		pivot_x_ebox->SetText((int)sprite->rects[0].offset.x);
		pivot_y_ebox->SetText((int)sprite->rects[0].offset.y);
	}

	cb_type->SetCurString(sprite->type);
	texture_mode->SetCurString(sprite->mode);
	texture_filter->SetCurString(sprite->filter);

	FitImage();
}

void SpriteWindow::ShowFrameWidgets()
{
	bool show = sprite->type == Sprite::Frames;

	cur_frame_label->Show(show);
	cur_frame_ebox->Show(show);
	btn_del_frame->Show(show);
	num_frame_label->Show(show);
	num_frame_ebox->Show(show);

	show_anim_label->Show(show);
	show_anim_box->Show(show);

	frame_time_label->Show(show);
	frame_time_ebox->Show(show);
	cur_frame_time_label->Show(show);
	cur_frame_time_ebox->Show(show);
}

void SpriteWindow::SetCurFrame(int frame)
{
	cur_frame = frame;

	for (int j = 0; j < 4; j++)
	{
		points[j] = frames[cur_frame * 4 + j];
	}

	cur_frame_ebox->SetText(cur_frame);
	cur_frame_time_ebox->SetText(sprite->rects[cur_frame].frame_time);
	pivot_x_ebox->SetText((int)sprite->rects[cur_frame].offset.x);
	pivot_y_ebox->SetText((int)sprite->rects[cur_frame].offset.y);
	UpdateSavedPos();
	img_wgt->Redraw();
}

void SpriteWindow::SelectRect()
{
	rect_width = 2;
	rect_height = 2;

	if (sprite->type == Sprite::ThreeSlicesVert)
	{
		rect_height = 4;
	}
	else
	if (sprite->type == Sprite::ThreeSlicesHorz)
	{
		rect_width = 4;
	}
	else
	if (sprite->type == Sprite::NineSlices)
	{
		rect_width = 4;
		rect_height = 4;
	}
}

void SpriteWindow::FillRects()
{
	switch (sprite->type)
	{
		case Sprite::Image:
		case Sprite::Frames:
		{
			points[0] = Vector2(sprite_pos.x, sprite_pos.y);
			points[1] = Vector2(sprite_pos.x + sprite_size.x, sprite_pos.y);
			points[2] = Vector2(sprite_pos.x, sprite_pos.y - sprite_size.y);
			points[3] = Vector2(sprite_pos.x + sprite_size.x, sprite_pos.y - sprite_size.y);

			break;
		}
		case Sprite::ThreeSlicesVert:
		{
			FillPoints(0, 2, sprite_pos.x, true);
			FillPoints(1, 2, (float)sprite_pos.x + sprite_size.x, true);

			break;
		}
		case Sprite::ThreeSlicesHorz:
		{
			FillPoints(0, 4, sprite_pos.y, false);
			FillPoints(4, 4, sprite_pos.y - sprite_size.y, false);

			break;
		}
		case Sprite::NineSlices:
		{
			FillPoints(0, 4, sprite_pos.x, true);
			FillPoints(1, 4, sprite_pos.x + sprite_offset_x.x, true);
			FillPoints(2, 4, sprite_pos.x + sprite_size.x - sprite_offset_x.y, true);
			FillPoints(3, 4, sprite_pos.x + sprite_size.x, true);

			break;
		}
	}
}

void SpriteWindow::ActualPixels()
{
	if (!image) return;

	pixel_density = 1.0f;

	origin.x = (img_wgt->GetWidth() - pixel_density * sprite->width) * 0.5f / pixel_density;
	origin.y = (img_wgt->GetHeight() - (img_wgt->GetHeight() - pixel_density * sprite->height) * 0.5f) / pixel_density;

	UpdateImageBackground();

	img_wgt->Redraw();

	delta_mouse = 0.0f;
}

void SpriteWindow::FitImage()
{
	if (!image)
	{
		pixel_density = 1.0f;
		origin.x = img_wgt->GetWidth() * 0.5f;
		origin.y = img_wgt->GetHeight() * 0.5f;
	}
	else
	{
		pixel_density = (float)(img_wgt->GetWidth() - 20) / (float)sprite->width;

		if ((float)sprite->height * pixel_density > img_wgt->GetHeight() - 20)
		{
			pixel_density = (float)(img_wgt->GetHeight() - 20) / (float)sprite->height;
		}

		origin.x = (img_wgt->GetWidth() - pixel_density * sprite->width) * 0.5f / pixel_density;
		origin.y = (img_wgt->GetHeight() - (img_wgt->GetHeight() - pixel_density * sprite->height) * 0.5f) / pixel_density;
	}

	UpdateImageBackground();

	img_wgt->Redraw();

	delta_mouse = 0.0f;
}

void SpriteWindow::MakeZoom(bool zoom_in)
{
	Vector2 origin_px = origin * pixel_density;
	origin_px.x = origin_px.x - img_wgt->GetWidth() * 0.5f / pixel_density;
	origin_px.y = origin_px.y + img_wgt->GetHeight() * 0.5f / pixel_density;

	pixel_density += zoom_in ? 0.5f : -0.5f;

	origin_px.x += img_wgt->GetWidth() * 0.5f / pixel_density;
	origin_px.y -= img_wgt->GetHeight() * 0.5f / pixel_density;
	origin = origin_px / pixel_density;

	UpdateImageBackground();

	img_wgt->Redraw();
}

void SpriteWindow::CheckStateOfBorder()
{
	if (!img_wgt->IsFocused() && border_drawed)
	{
		img_wgt->Redraw();
		border_drawed = false;
	}
}

void SpriteWindow::OnDraw(EUIWidget* sender)
{
	HWND handle = *((HWND*)sender->GetNative());

	RECT rc;
	HDC hdcMem;
	HBITMAP hbmMem, hbmOld;

	GetClientRect(handle, &rc);

	HDC wnd_hdc = GetDC(handle);
	hdcMem = CreateCompatibleDC(wnd_hdc);
	hbmMem = CreateCompatibleBitmap(wnd_hdc, rc.right - rc.left, rc.bottom - rc.top);
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

	HDC hdc = hdcMem;

	Color color = Color(0.8f, 0.8f, 0.8f);

	HBRUSH hbrBkgnd = CreateSolidBrush(RGB(color.r * 255, color.g * 255, color.b * 255));
	HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH green = CreateSolidBrush(RGB(0, 255, 0));
	HPEN pen_white = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HPEN pen_green = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	HPEN pen_red = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	HPEN pen_black = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN pen_axis = CreatePen(PS_SOLID, 2, RGB(55, 55, 55));
	HPEN pen_bcgr = CreatePen(PS_SOLID, 1, RGB(color.r * 255, color.g * 255, color.b * 255));

	HPEN pre_pen;

	SelectObject(hdc, hbrBkgnd);
	SelectObject(hdc, pen_bcgr);
	Rectangle(hdc, 0, 0, sender->GetWidth(), sender->GetHeight());
	SelectObject(hdc, pen_black);

	float wd = sprite->texture ? sprite->width * 1.1f : 512.0f;
	float ht = sprite->texture ? sprite->height * 1.1f : 512.0f;

	MoveToEx(hdc, (int)(origin.x * pixel_density), (int)((origin.y - ht) * pixel_density), 0);
	LineTo(hdc, (int)(origin.x * pixel_density), (int)((origin.y + ht) * pixel_density));

	MoveToEx(hdc, (int)((origin.x - wd) * pixel_density), (int)(origin.y * pixel_density), 0);
	LineTo(hdc, (int)((origin.x + wd) * pixel_density), (int)(origin.y * pixel_density));

	if (sender->IsFocused())
	{
		SelectObject(hdc, pen_red);

		for (int i = 0; i < 3; i++)
		{
			MoveToEx(hdc, 1, 1 + i, 0);
			LineTo(hdc, sender->GetWidth(), 1 + i);

			MoveToEx(hdc, 1, sender->GetHeight() - 1 - i, 0);
			LineTo(hdc, sender->GetWidth(), sender->GetHeight() - 1 - i);

			MoveToEx(hdc, 1 + i, 1, 0);
			LineTo(hdc, 1 + i, sender->GetHeight());

			MoveToEx(hdc, sender->GetWidth() - 1 - i, 1, 0);
			LineTo(hdc, sender->GetWidth() -1 - i, sender->GetHeight() - 1);
		}

		border_drawed = true;
	}


	if (sprite->texture)
	{
		pre_pen = (HPEN)SelectObject(hdc, hbrBkgnd);

		SelectObject(hdc, pre_pen);

		HDC MemDCExercising = CreateCompatibleDC(hdc);
		SelectObject(MemDCExercising, image);

		int cur_wdth = (int)(sprite->width * pixel_density);
		int cur_hgt = (int)(sprite->height * pixel_density);

		Vector2 pos = origin;
		pos.y -= sprite->height;

		StretchBlt(hdc, (int)(pos.x * pixel_density), (int)(pos.y * pixel_density), cur_wdth, cur_hgt, MemDCExercising, 0, 0, sprite->width, sprite->height, SRCCOPY);

		if (sprite->type == Sprite::Frames)
		{
			SelectObject(hdc, white);
			SelectObject(hdc, pen_white);

			for (int i=0; i<num_frames; i++)
			{
				MoveToEx(hdc, (int)((origin.x + frames[i * 4 + 0].x) * pixel_density), (int)((origin.y - frames[i * 4 + 0].y) * pixel_density), 0);
				LineTo(hdc, (int)((origin.x + frames[i * 4 + 1].x) * pixel_density), (int)((origin.y - frames[i * 4 + 1].y) * pixel_density));
				LineTo(hdc, (int)((origin.x + frames[i * 4 + 3].x) * pixel_density), (int)((origin.y - frames[i * 4 + 3].y) * pixel_density));

				LineTo(hdc, (int)((origin.x + frames[i * 4 + 2].x) * pixel_density), (int)((origin.y - frames[i * 4 + 2].y) * pixel_density));
				LineTo(hdc, (int)((origin.x + frames[i * 4 + 0].x) * pixel_density), (int)((origin.y - frames[i * 4 + 0].y) * pixel_density));
			}
		}

		for (int i = 0; i<rect_height; i++)
			for (int j = 0; j<rect_width; j++)
			{
				int index = rect_width * i + j;

				SelectObject(hdc, white);
				SelectObject(hdc, pen_white);

				if (j < rect_width - 1 && i < rect_height - 1)
				{
					MoveToEx(hdc, (int)((origin.x + points[index + rect_width].x) * pixel_density), (int)((origin.y - points[index + rect_width].y) * pixel_density), 0);
					LineTo(hdc, (int)((origin.x + points[index].x) * pixel_density), (int)((origin.y - points[index].y) * pixel_density));
					LineTo(hdc, (int)((origin.x + points[index + 1].x) * pixel_density), (int)((origin.y - points[index + 1].y) * pixel_density));

					if (i == rect_height - 2 || j == rect_width - 2)
					{
						LineTo(hdc, (int)((origin.x + points[index + rect_width + 1].x) * pixel_density), (int)((origin.y - points[index + rect_width + 1].y) * pixel_density));
						LineTo(hdc, (int)((origin.x + points[index + rect_width].x) * pixel_density), (int)((origin.y - points[index + rect_width].y) * pixel_density));
					}
				}

				if (sel_row == i && sel_col == j)
				{
					SelectObject(hdc, green);
					SelectObject(hdc, pen_white);
				}
				else
				{
					SelectObject(hdc, white);
					SelectObject(hdc, pen_white);
				}

				Rectangle(hdc, (int)((origin.x + points[index].x) * pixel_density - 4), (int)((origin.y - points[index].y) * pixel_density - 4),
				          (int)((origin.x + points[index].x) * pixel_density + 4), (int)((origin.y - points[index].y) * pixel_density + 4));
			}

		SelectObject(hdc, pen_green);

		if (sprite->type == Sprite::Frames)
		{
			SelectObject(hdc, pen_green);

			float pivot_x = (frames[cur_frame * 4 + 0].x + frames[cur_frame * 4 + 1].x) * 0.5f;
			float pivot_y = frames[cur_frame * 4 + 2].y;
			MoveToEx(hdc, (int)((origin.x + pivot_x) * pixel_density), (int)((origin.y - pivot_y) * pixel_density), 0);
			LineTo(hdc, (int)((origin.x + pivot_x) * pixel_density), (int)((origin.y - pivot_y + sprite->rects[cur_frame].offset.y) * pixel_density));
			LineTo(hdc, (int)((origin.x + pivot_x - sprite->rects[cur_frame].offset.x) * pixel_density), (int)((origin.y - pivot_y + sprite->rects[cur_frame].offset.y) * pixel_density));

			SelectObject(hdc, green);
			SelectObject(hdc, pen_white);

			Ellipse(hdc, (int)((origin.x + pivot_x) * pixel_density - 4), (int)((origin.y - pivot_y) * pixel_density - 4),
			        (int)((origin.x + pivot_x) * pixel_density + 4), (int)((origin.y - pivot_y) * pixel_density + 4));
			Ellipse(hdc, (int)((origin.x + pivot_x - sprite->rects[cur_frame].offset.x) * pixel_density - 4), (int)((origin.y - pivot_y + sprite->rects[cur_frame].offset.y) * pixel_density - 4),
			        (int)((origin.x + pivot_x - sprite->rects[cur_frame].offset.x) * pixel_density + 4), (int)((origin.y - pivot_y + sprite->rects[cur_frame].offset.y) * pixel_density + 4));
		}

		DeleteDC(MemDCExercising);
	}

	ReleaseDC(handle, hdc);
	DeleteObject(hbrBkgnd);
	DeleteObject(white);
	DeleteObject(green);
	DeleteObject(pen_white);
	DeleteObject(pen_green);
	DeleteObject(pen_axis);

	BitBlt(wnd_hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);

	ReleaseDC(handle, wnd_hdc);
}

void SpriteWindow::MoveRects(Vector2 delta)
{
	if (sel_row == -1)
	{
		for (int i = 0; i<rect_height; i++)
			for (int j = 0; j<rect_width; j++)
			{
				points[rect_width * i + j] -= Vector2(delta.x, -delta.y);
			}
	}
	else
	{
		int index = rect_width * sel_row + sel_col;

		int border = 3;

		if (sel_col > 0 && sel_row == 0)
		{
			if (points[index - 1].x + border > points[index].x - delta.x)
			{
				delta.x = points[index].x - points[index - 1].x - border;
			}
		}

		if (sel_col == 0 && sel_row > 0)
		{
			if (points[index - rect_width].y - border < points[index].y + delta.y)
			{
				delta.y = points[index - rect_width].y - border - points[index].y;
			}
		}

		if (sel_col > 0 && sel_row > 0)
		{
			if (points[index - rect_width - 1].x + border > points[index].x - delta.x)
			{
				delta.x = points[index].x - points[index - rect_width - 1].x - border;
			}

			if (points[index - rect_width - 1].y - border < points[index].y + delta.y)
			{
				delta.y = points[index - rect_width - 1].y - border - points[index].y;
			}
		}

		if (sel_col < rect_width - 1 && sel_row == rect_height - 1)
		{
			if (points[index + 1].x - border < points[index].x - delta.x)
			{
				delta.x = points[index].x - points[index + 1].x + border;
			}
		}

		if (sel_col == rect_width - 1 && sel_row < rect_height - 1)
		{
			if (points[index + 1].y + border > points[index].y + delta.y)
			{
				delta.y = points[index + 1].y + border - points[index].y;
			}
		}

		if (sel_col < rect_width - 1 && sel_row < rect_height - 1)
		{
			if (points[index + rect_width + 1].x - border < points[index].x - delta.x)
			{
				delta.x = points[index].x - points[index + rect_width + 1].x + border;
			}

			if (points[index + rect_width + 1].y + border > points[index].y + delta.y)
			{
				delta.y = points[index + rect_width + 1].y + border - points[index].y;
			}
		}

		for (int i = 0; i<rect_height; i++)
		{
			points[rect_width * i + sel_col] -= Vector2(delta.x, 0.0f);
		}

		for (int j = 0; j<rect_width; j++)
		{
			points[rect_width * sel_row + j] += Vector2(0.0f, delta.y);
		}
	}

	UpdateSavedPos();
	UpdateAnimRect();
	UpdateSpriteRect();
}

void SpriteWindow::UpdateAnimRect()
{
	for (int j = 0; j < 4; j++)
	{
		frames[cur_frame * 4 + j] = points[j];
	}
}

void SpriteWindow::UpdateSavedPos()
{
	sprite_pos = points[0];
	sprite_size = points[rect_height * rect_width - 1] - points[0];
	sprite_size.y = -sprite_size.y;

	if (rect_width > 2)
	{
		sprite_offset_x.x = points[1].x - points[0].x;
		sprite_offset_x.y = points[3].x - points[2].x;
	}

	if (rect_height > 2)
	{
		sprite_offset_y.x = points[0].y - points[rect_width].y;
		sprite_offset_y.y = points[2 * rect_width].y - points[3 * rect_width].y;
	}

	prop_x_ebox->SetText((int)sprite_pos.x);
	prop_y_ebox->SetText((int)(sprite->height - sprite_pos.y));

	prop_w_ebox->SetText((int)sprite_size.x);
	prop_h_ebox->SetText((int)sprite_size.y);
}

void SpriteWindow::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	if (sender == texture_mode)
	{
		sprite->mode = (Texture::TextureAddress)texture_mode->GetCurStringIndex();
	}

	if (sender == texture_filter)
	{
		sprite->filter = (Texture::FilterType)texture_filter->GetCurStringIndex();
	}

	if (sender == cb_type)
	{
		sprite->type = (Sprite::Type)cb_type->GetCurStringIndex();
		ShowFrameWidgets();
		SelectRect();
		ResizeSpriteRect();
		FillRects();
		UpdateAnimRect();
		UpdateSpriteRect();
		img_wgt->Redraw();
	}
}

void SpriteWindow::OnEditBoxStopEditing(EUIEditBox* sender)
{
	float delta_x = 0.0f;
	float delta_y = 0.0f;

	if (sender == prop_x_ebox)
	{
		delta_x = sprite_pos.x - prop_x_ebox->GetAsInt();
	}

	if (sender == prop_y_ebox)
	{
		delta_y = (sprite->height - prop_y_ebox->GetAsInt()) - sprite_pos.y;
	}

	if (fabs(delta_x) > 0.0f || fabs(delta_y) > 0.0f)
	{
		for (int i = 0; i < rect_height; i++)
			for (int j = 0; j < rect_width; j++)
			{
				points[rect_width * i + j] -= Vector2(delta_x, -delta_y);
			}

		UpdateSavedPos();
		UpdateAnimRect();
		UpdateSpriteRect();
		img_wgt->Redraw();
	}

	if (sender == prop_w_ebox || sender == prop_h_ebox)
	{
		if (sender == prop_w_ebox)
		{
			sprite_size.x = prop_w_ebox->GetAsFloat();
		}

		if (sender == prop_h_ebox)
		{
			sprite_size.y = prop_h_ebox->GetAsFloat();
		}

		FillRects();
		UpdateAnimRect();
		UpdateSpriteRect();

		img_wgt->Redraw();
	}

	if (sender == cur_frame_ebox)
	{
		SetCurFrame((int)fmin(atoi(cur_frame_ebox->GetText()), num_frames - 1));
	}

	if (sender == num_frame_ebox)
	{
		int prev_cur_frames = cur_frame;
		int prev_num_frames = num_frames;
		num_frames = (int)fmax(atoi(num_frame_ebox->GetText()), 1);
		num_frame_ebox->SetText(num_frames);

		frames.resize(num_frames * 4);
		ResizeSpriteRect();

		if (prev_num_frames < num_frames)
		{
			for (int i = prev_num_frames - 1; i >= prev_cur_frames; i--)
			{
				for (int j = 0; j < 4; j++)
				{
					frames[(i + 1) * 4 + j] = frames[i * 4 + j];
				}
			}

			for (int i = prev_num_frames + 1; i < num_frames; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					frames[i * 4 + j] = frames[(prev_num_frames) * 4 + j];
				}
			}
		}

		if (cur_frame >= num_frames)
		{
			SetCurFrame(num_frames - 1);
		}
		else
		if (prev_num_frames < num_frames)
		{
			SetCurFrame(prev_cur_frames + 1);
		}

		UpdateSpriteRect();
		img_wgt->Redraw();
	}

	if (sender == frame_time_ebox)
	{
		sprite->frame_time = fmax(0.025f, frame_time_ebox->GetAsFloat());
	}

	if (sender == pivot_x_ebox)
	{
		sprite->rects[cur_frame].offset.x = pivot_x_ebox->GetAsFloat();
		img_wgt->Redraw();
	}

	if (sender == pivot_y_ebox)
	{
		sprite->rects[cur_frame].offset.y = pivot_y_ebox->GetAsFloat();
		img_wgt->Redraw();
	}
}

void SpriteWindow::OnLeftMouseDown(EUIWidget* sender, int mx, int my)
{
	if (sender != img_wgt) return;

	img_wgt->SetFocused();

	Vector2 ps(prev_ms.x - origin.x * pixel_density, origin.y * pixel_density - prev_ms.y);

	if (sprite->type == Sprite::Frames)
	{
		Vector2 point = frames[cur_frame * 4] * pixel_density;
		Vector2 point2 = frames[cur_frame * 4 + 3] * pixel_density;

		int sel_frame = -1;

		if (point.x < ps.x && ps.x < point2.x &&
			point2.y < ps.y && ps.y < point.y)
		{
			sel_frame = cur_frame;
		}

		if (sel_frame == -1)
		{
			for (int i = 0; i < num_frames; i++)
			{
				Vector2 point = frames[i * 4] * pixel_density;
				Vector2 point2 = frames[i * 4 + 3] * pixel_density;

				if (point.x < ps.x && ps.x < point2.x &&
					point2.y < ps.y && ps.y < point.y)
				{
					sel_frame = i;
				}
			}

			if (sel_frame != -1)
			{
				SetCurFrame(sel_frame);
			}
		}
	}

	drag = DragRects;
	prev_ms = Vector2((float)mx, (float)my);

	sel_col = -1;
	sel_row = -1;

	for (int i = 0; i<rect_height; i++)
		for (int j = 0; j<rect_width; j++)
		{
			Vector2 point = points[rect_width * i + j];
			point *= pixel_density;

			if (point.x - 7 < ps.x && ps.x < point.x + 7 &&
				point.y - 7 < ps.y && ps.y < point.y + 7)
			{
				sel_row = i;
				sel_col = j;
			}
		}

	//img_wgt->CaptureMouse();

	if (sel_row != -1)
	{
		img_wgt->Redraw();
	}
}

void SpriteWindow::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender != img_wgt) return;

	delta_mouse.x += (prev_ms.x - mx) / pixel_density;
	delta_mouse.y += (prev_ms.y - my) / pixel_density;

	Vector2 delta(0.0f);

	if (fabs(delta_mouse.x) > 1.0f)
	{
		delta.x = floor(delta_mouse.x);
		delta_mouse.x = 0.0f;
	}

	if (fabs(delta_mouse.y) > 1.0f)
	{
		delta.y = floor(delta_mouse.y);
		delta_mouse.y = 0.0f;
	}

	if (drag == DragField)
	{
		origin -= delta;
	}
	else
	if (drag == DragRects)
	{
		MoveRects(delta);
	}

	prev_ms = Vector2((float)mx, (float)my);

	if (drag != DragNone)
	{
		img_wgt->Redraw();
	}
}

void SpriteWindow::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	if (sender == load_image)
	{
		const char* file_name = EUI::OpenOpenDialog(wnd->GetNative(), "Image files", "*");

		char cur_dir[2048];
		GetCurrentDirectory(512, cur_dir);

		if (file_name)
		{
			char cropped_path[1024];
			StringUtils::GetCropPath(cur_dir, file_name, cropped_path, 1024);
			StringUtils::RemoveFirstChar(cropped_path);

			SetImage(cropped_path, true);
			img_wgt->Redraw();
		}
	}

	if (sender == del_image)
	{
		SetImage("", false);
		img_wgt->Redraw();
	}

	if (sender == btn_zoom_in)
	{
		MakeZoom(true);
	}

	if (sender == btn_zoom_out)
	{
		if (pixel_density > 1.0f)
		{
			MakeZoom(false);
		}
	}

	if (sender == btn_act_pixels)
	{
		ActualPixels();
	}

	if (sender == btn_fit)
	{
		FitImage();
	}

	if (sender == btn_del_frame && num_frames > 1)
	{
		for (int i = cur_frame + 1; i < num_frames; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				frames[(i - 1) * 4 + j] = frames[i * 4 + j];
			}
		}

		num_frames--;
		frames.resize(num_frames * 4);
		ResizeSpriteRect();

		SetCurFrame((int)fmin(cur_frame, num_frames - 1));
		num_frame_ebox->SetText(num_frames);

		UpdateSpriteRect();
		img_wgt->Redraw();
	}

	if (sender == show_anim_box)
	{
		show_anim = !show_anim;
	}

	if (sender == img_wgt)
	{
		sel_col = -1;
		sel_row = -1;
		drag = DragNone;
		//img_wgt->ReleaseMouse();
		img_wgt->Redraw();
	}
}

void SpriteWindow::OnRightMouseDown(EUIWidget* sender, int mx, int my)
{
	drag = DragField;
	prev_ms = Vector2((float)mx, (float)my);
	img_wgt->CaptureMouse();
}

void SpriteWindow::OnRightMouseUp(EUIWidget* sender, int mx, int my)
{
	drag = DragNone;
	img_wgt->ReleaseMouse();
}

void SpriteWindow::OnKey(EUIWidget* sender, int key)
{
	Vector2 delta = 0.0f;

	if (!border_drawed)
	{
		return;
	}

	switch (key)
	{
		case VK_UP : delta.y = 1.0f; break;
		case VK_DOWN: delta.y = -1.0f; break;
		case VK_LEFT: delta.x = 1.0f; break;
		case VK_RIGHT: delta.x = -1.0f; break;
	}

	if (fabs(delta.x) > 0.0f || fabs(delta.y) > 0.0f)
	{
		MoveRects(delta);
		img_wgt->Redraw();
	}
}

void SpriteWindow::OnWinClose(EUIWidget* sender)
{
	instance = nullptr;
}

#endif