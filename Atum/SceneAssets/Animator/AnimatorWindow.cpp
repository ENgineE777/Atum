
#ifdef EDITOR

#include "AnimatorWindow.h"
#include "Animator.h"
#include "Support/Timer.h"

EUIPanel* DataTrack::timeline = nullptr;

AnimatorWindow* AnimatorWindow::instance = nullptr;

void AnimatorWindow::Init()
{
	timeOffset = 0.0f;
	owner = nullptr;
	wnd = new EUIWindow("Animator", EUIWindow::Popup, true, 40, 40, 1024, 600);

	EUILayout* wnd_lt = new EUILayout(wnd, true);

	EUIPanel* toolsPanel = new EUIPanel(wnd_lt, 10, 10, 100, 30);
	wnd_lt->SetChildSize(toolsPanel, 40, false);

	cb_addObject = new EUIComboBox(toolsPanel, 10, 10, 110, 95);
	cb_addObject->SetListener(AddPlayerID, this, 0);

	ClassFactoryTrackPlayer* decl = ClassFactoryTrackPlayer::First();

	while (decl)
	{
		cb_addObject->AddItem(decl->GetName());
		decl = decl->Next();
	}

	cb_addObject->SetCurString(-1);

	btn_delObject = new EUIButton(toolsPanel, "Del", 125, 10, 35, 22);
	btn_delObject->SetListener(DelPLayerID, this, 0);

	btn_addKey = new EUIButton(toolsPanel, "Add Key", 185, 10, 50, 22);
	btn_addKey->SetListener(AddKeyID, this, 0);

	btn_delKey = new EUIButton(toolsPanel, "Del Key", 240, 10, 50, 22);
	btn_delKey->SetListener(DelKeyID, this, 0);

	btn_playKey = new EUIButton(toolsPanel, "Play", 295, 10, 50, 22);
	btn_playKey->SetListener(PlayID, this, 0);

	lb_fromTime = new EUILabel(toolsPanel, "From", 360, 14, 30, 22);
	tb_fromTime = new EUIEditBox(toolsPanel, "0.0", 390, 11, 60, 20, EUIEditBox::InputUFloat);
	tb_fromTime->SetListener(FromTimeID, this, 0);

	lb_curTime = new EUILabel(toolsPanel, "Cur Time", 460, 14, 50, 22);
	tb_curTime = new EUIEditBox(toolsPanel, "0.0", 510, 11, 60, 22, EUIEditBox::InputUFloat);
	tb_curTime->SetListener(CurTimeID, this, 0);

	lb_toTime = new EUILabel(toolsPanel, "To", 590, 14, 30, 22);
	tb_toTime = new EUIEditBox(toolsPanel, "0.0", 610, 11, 60, 20, EUIEditBox::InputUFloat);
	tb_toTime->SetListener(ToTimeID, this, 0);

	lb_timeLenght = new EUILabel(toolsPanel, "Time Length", 680, 14, 60, 22);
	tb_timeLenght = new EUIEditBox(toolsPanel, "0.0", 740, 11, 60, 20, EUIEditBox::InputUFloat);
	tb_timeLenght->SetListener(TimeLengthID, this, 0);

	EUIPanel* timelinePanel = new EUIPanel(wnd_lt, 10, 10, 100, 30);

	EUILayout* timeline_lt = new EUILayout(timelinePanel, false);

	timeline_panel = new EUIPanel(timeline_lt, 10, 100, 400, 200);
	timeline_panel->SetListener(TimeLineID, this, EUIWidget::OnDraw | EUIWidget::OnUpdate | EUIWidget::OnResize);
	DataTrack::timeline = timeline_panel;

	vert_scroll = new EUIScrollBar(timeline_lt, false, 710, 65, 15, 240);
	vert_scroll->SetListener(-1, this, 0);
	timeline_lt->SetChildSize(vert_scroll, 20, false);

	prop_dock = new EUICategories(timeline_lt, 730, 35, 200, 500);
	timeline_lt->SetChildSize(prop_dock, 200, false);

	EUIPanel* scrollPanel = new EUIPanel(wnd_lt, 10, 10, 100, 30);
	wnd_lt->SetChildSize(scrollPanel, 20, false);

	EUILayout* sroll_lt = new EUILayout(scrollPanel, false);

	EUIPanel* dummyPanel_lf = new EUIPanel(sroll_lt, 10, 10, 100, 30);
	sroll_lt->SetChildSize(dummyPanel_lf, 150, false);

	horz_scroll = new EUIScrollBar(sroll_lt, true, 0, 0, 10, 10);
	horz_scroll->SetListener(-1, this, 0);

	EUIPanel* dummyPanel_rg = new EUIPanel(sroll_lt, 10, 10, 100, 30);
	sroll_lt->SetChildSize(dummyPanel_rg, 220, false);

	LOGFONT lf = { 0 };
	GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

	StringUtils::Copy(lf.lfFaceName, LF_FACESIZE, "Tahoma");
	lf.lfHeight = -10;

	font = CreateFontIndirect(&lf);
}

void AnimatorWindow::Show(bool show)
{
	wnd->Show(show);
}

void AnimatorWindow::PrepareWidgets(Animator* anim)
{
	Clear();

	owner = anim;

	if (!owner) return;

	char txt[128];

	StringUtils::Printf(txt, 128, "%4.2f", owner->timeLenght);
	tb_timeLenght->SetText(txt);

	StringUtils::Printf(txt, 128, "%4.2f", owner->edCurTime);
	tb_curTime->SetText(txt);

	for (int i=0;i<owner->players.size();i++)
	{
		owner->players[i]->InitControls(prop_dock);
	}

	UpdateHorzBar();
	UpdateVertBar();
}

void AnimatorWindow::Clear()
{
	SetSelection(-1, -1, -1);
	timeOffset = 0.0f;
}

void AnimatorWindow::OnLeftMouseDown(EUIWidget* sender, int mx, int my)
{
	int player = -1;
	int track = -1;
	int key = -1;
	
	if (my > CaptionHeight)
	{
		int line = (int)(((float)my - CaptionHeight) / RowHeight) + vert_scroll->GetPosition();

		int index = 0;

		for(int i = 0; i<(int)owner->players.size(); i++)
		{
			TrackPlayer* plr = owner->players[i];

			if (index == line)
			{
				player = i;
				break;
			}

			index++;

			for (int j = 0; j<(int)plr->tracks.size(); j++)
			{
				if (index == line)
				{
					player = i;
					track = j;

					break;
				}

				index++;
			}

			if (player != -1)
			{
				break;
			}
		}

		if (player == -1)
		{
			return;
		}
	
		if (track != -1)
		{
			DataTrack* tr = owner->players[player]->tracks[track];
 
			for (int k=0; k<tr->GetKeysCount(); k++)
			{
				int pos = CalcKeyPos(tr->GetKey(k).time - horz_scroll->GetPosition() * 0.1f);

				if (pos < mx && mx < pos + 5)
				{
					key = k;
					drag_mode = DragKey;
					break;
				}
			}
		}

		if (player != selPlayer || track != selTrack || key != selKey)
		{
			SetSelection(player, track, key);
		}
		else
		if (mx < LeftRowWidth)
		{
			if (selPlayer != -1 && selTrack == -1)
			{
				owner->players[selPlayer]->SetActive(!owner->players[selPlayer]->IsActive());
				timeline_panel->Redraw();
			}
		}
	}

	if (mx >= LeftRowWidth)
	{
		float pos = mx - 152.0f;
		pos = PixelToTime(pos);
		if (pos<0) pos = 0.0f;
		pos += horz_scroll->GetPosition() * 0.1f;

		if (my > CaptionHeight)
		{
			SetCurTime(pos);
		}
		else
		{
			if (fabs(pos - owner->fromTime) < 0.01f && my < CaptionHeight)
			{
				drag_mode = DragFromTime;
			}
			else
			if (fabs(pos - owner->toTime) < 0.01f && my < CaptionHeight)
			{
				drag_mode = DragToTime;
			}
			else
			{
				drag_mode = DragCurTime;
				SetCurTime(pos);
			}
		}
	}

	timeline_panel->CaptureMouse();
}

void AnimatorWindow::OnMouseMove(EUIWidget* sender, int mx, int my)
{
	if (sender->GetID() == TimeLineID)
	{
		static int mxPrev = mx;
		static int myPrev = my;

		if (drag_mode != DragNone)
		{
			float pos = mx - 152.0f;
			pos = PixelToTime(pos);
			if (pos<0) pos = 0.0f;
			pos += horz_scroll->GetPosition() * 0.1f;

			if (drag_mode == DragKey)
			{
				DataTrack* track = owner->players[selPlayer]->tracks[selTrack];

				bool allow = true;

				if (pos > owner->timeLenght + 0.01f)
				{
					allow = false;
				}

				if (selKey > 0 && track->GetKey(selKey - 1).time + 0.01f >pos)
				{
					allow = false;
				}

				if (selKey < track->GetKeysCount() - 1 && 
					track->GetKey(selKey + 1).time - 0.01f < pos)
				{
					allow = false;
				}

				if (allow)
				{
					track->SetKeyTime(selKey, pos);
				}
			}

			if (drag_mode == DragKey || drag_mode == DragCurTime)
			{
				SetCurTime(pos);
			}

			if (drag_mode == DragFromTime)
			{
				SetFromTime(pos);
			}

			if (drag_mode == DragToTime)
			{
				SetToTime(pos);
			}
		}

		mxPrev = mx;
		myPrev = my;
	}
}

void AnimatorWindow::OnLeftMouseUp(EUIWidget* sender, int mx, int my)
{
	switch (sender->GetID())
	{
		case DelPLayerID:
		{
			if (selPlayer != -1)
			{
				int delPlayer = selPlayer;
				SetSelection(-1, -1, -1);
				owner->players.erase(owner->players.begin() + delPlayer);
				UpdateVertBar();
				timeline_panel->Redraw();
			}

			break;
		}
		case PlayID:
		{
			play = !play;
			timeline_panel->Redraw();
			break;
		}
		case TimeLineID:
		{
			drag_mode = DragNone;
			timeline_panel->ReleaseMouse();
			break;
		}
		case AddKeyID:
		{
			AddKey();
			break;
		}
		case DelKeyID:
		{
			DelKey();
			break;
		}
	}
}

void AnimatorWindow::OnEditBoxStopEditing(EUIEditBox* sender)
{
	switch (sender->GetID())
	{
		case CurTimeID:
		{
			float val = (float)atof(tb_curTime->GetText());

			if (fabs(val - owner->edCurTime)>0.001f && !play)
			{
				SetCurTime(val);
			}

			break;
		}
		case TimeLengthID:
		{
			float val = (float)atof(tb_timeLenght->GetText());

			if (fabs(val - owner->timeLenght)>0.001f)
			{
				owner->timeLenght = val;
				//owner->ReverseTieValues();

				SetTimeLenght(owner->timeLenght);
				SetToTime(owner->timeLenght);
			}
		}
		case FromTimeID:
		{
			float val = (float)atof(tb_fromTime->GetText());

			if (fabs(val - owner->fromTime)>0.001f && !play)
			{
				SetFromTime(val);
			}

			break;
		}
		case ToTimeID:
		{
			float val = (float)atof(tb_toTime->GetText());

			if (fabs(val - owner->toTime)>0.001f && !play)
			{
				SetToTime(val);
			}

			break;
		}
	}
}

void AnimatorWindow::OnComboBoxSelChange(EUIComboBox* sender, int index)
{
	if (sender->GetID() == AddPlayerID)
	{
		TrackPlayer* player = ClassFactoryTrackPlayer::Create(cb_addObject->GetCurString());

		if (player)
		{
			player->owner = owner;

			player->SetName(cb_addObject->GetText());
			player->SetType(cb_addObject->GetText());

			player->Init();
			player->InitControls(prop_dock);

			player->GetMetaData()->Prepare(player);
			player->GetMetaData()->SetDefValuesPrepare();

			owner->players.push_back(player);
		}

		cb_addObject->SetCurString(-1);

		UpdateVertBar();
		timeline_panel->Redraw();
	}
}

void AnimatorWindow::OnSrollerPosChange(EUIScrollBar* sender, int pos)
{
	timeline_panel->Redraw();
}

void AnimatorWindow::OnDraw(EUIWidget* sender)
{
	HWND hwnd = *((HWND*)sender->GetNative());

	RECT rc;
	HDC hdcMem;
	HBITMAP hbmMem, hbmOld;

	GetClientRect(hwnd, &rc);

	HDC wnd_hdc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(wnd_hdc);
	hbmMem = CreateCompatibleBitmap(wnd_hdc, rc.right - rc.left, rc.bottom - rc.top);
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

	HDC hdc = hdcMem;

	SelectObject(hdc, font);
	SelectObject(hdc, GetStockObject(DC_BRUSH));

	SetDCBrushColor(hdc, RGB(252, 252, 252));
	SetBkColor(hdc, RGB(252, 252, 252));

	Rectangle(hdc, 0, 0, LeftRowWidth + 1, CaptionHeight + 1);
	Rectangle(hdc, LeftRowWidth, 0, (int)timeline_panel->GetWidth(), CaptionHeight + 1);

	float offset = horz_scroll->GetPosition() * 0.1f;
	float tm = offset;

	float tm_width = (timeline_panel->GetWidth() - LeftRowWidth - 4.0f) / scale;

	while (tm < owner->timeLenght + 0.001f && tm < offset + tm_width)
	{
		int pos = (int)((tm - offset) * scale) + LeftRowWidth + 4;
		int y = 24;

		if (fabs(tm -(int)(tm + 0.25f)) < 0.01f)
		{
			char str[16];
			sprintf(str, "%i", (int)(tm + 0.25f));

			TextOut(hdc, pos - 2, 3, str, (int)strlen(str));
			y = 20;
		}

		MoveToEx(hdc, pos, y, 0);
		LineTo(hdc, pos, CaptionHeight);

		tm += 0.1f;
	}

	int last_y = CaptionHeight;
	int active = 1;
	int index = 0;

	int max_lines = (int)((timeline_panel->GetHeight() - CaptionHeight) / RowHeight) + 1;

	for (int i=0; i<(int)owner->players.size(); i++)
	{
		TrackPlayer* player = owner->players[i];

		int draw_index = index - vert_scroll->GetPosition();

		active = player->IsActive();

		if (0 <= draw_index && draw_index <= max_lines)
		{
			if (selPlayer == i && selTrack == -1)
			{
				SetDCBrushColor(hdc, RGB(205 + 50 * active, 205 + 50 * active, 205 + 50 * active));
				SetBkColor(hdc, RGB(205 + 50 * active, 205 + 50 * active, 205 + 50 * active));
			}
			else
			{
				SetDCBrushColor(hdc, RGB(198 + 50 * active, 198 + 50 * active, 198 + 50 * active));
				SetBkColor(hdc, RGB(198 + 50 * active, 198 + 50 * active, 198 + 50 * active));
			}

			Rectangle(hdc, 0, CalcLinePos(draw_index), LeftRowWidth + 1, CalcLinePos(draw_index + 1) + 1);
			Rectangle(hdc, LeftRowWidth, CalcLinePos(draw_index), (int)timeline_panel->GetWidth() + 1, CalcLinePos(draw_index + 1) + 1);

			const char* act_text = active ? "(Active)" : "(Inactive)";
			TextOut(hdc, 90, CalcLinePos(draw_index) + 3, act_text, (int)strlen(act_text));

			TextOut(hdc, 10, CalcLinePos(draw_index) + 3, player->GetName(), (int)strlen(player->GetName()));
		}

		index++;
		draw_index++;

		for (int j = 0; j<(int)player->tracks.size(); j++)
		{
			if (0 <= draw_index && draw_index <= max_lines)
			{
				DataTrack* track = player->tracks[j];

				if (selPlayer == i && selTrack == j)
				{
					SetDCBrushColor(hdc, RGB(205 + 50 * active, 205 + 50 * active, 205 + 50 * active));
					SetBkColor(hdc, RGB(205 + 50 * active, 205 + 50 * active, 205 + 50 * active));
				}
				else
				{
					SetDCBrushColor(hdc, RGB(185 + 50 * active, 185 + 50 * active, 185 + 50 * active));
					SetBkColor(hdc, RGB(185 + 50 * active, 185 + 50 * active, 185 + 50 * active));
				}

				Rectangle(hdc, 0, CalcLinePos(draw_index), LeftRowWidth + 1, CalcLinePos(draw_index + 1) + 1);
				Rectangle(hdc, LeftRowWidth, CalcLinePos(draw_index), (int)timeline_panel->GetWidth() + 1, CalcLinePos(draw_index + 1) + 1);

				TextOut(hdc, 10, CalcLinePos(draw_index) + 3, track->GetName(), (int)strlen(track->GetName()));

				for (int p = 0; p < track->GetKeysCount(); p++)
				{
					DataTrack::Key key = track->GetKey(p);
					float tm = key.time - offset;

					if (key.blend != DataTrack::BlendNone && p != track->GetKeysCount() - 1)
					{
						float next_tm = track->GetKey(p + 1).time - offset;

						if (next_tm > 0.0f && tm < tm_width)
						{
							MoveToEx(hdc, CalcKeyPos(fmax(0.0f, tm)), CalcLinePos(draw_index) + 10, 0);
							LineTo(hdc, CalcKeyPos(fmin(next_tm, tm_width)), CalcLinePos(draw_index) + 10);
						}
					}

					if (owner->timeLenght < key.time) break;

					SetDCBrushColor(hdc, RGB(255,255,255));

					if (selPlayer == i && selTrack == j && selKey == p)
					{
						SetDCBrushColor(hdc, RGB(238, 80, 80));
					}

					if (key.time - offset < 0.0f) continue;
					if (key.time - offset > tm_width) continue;

					Rectangle(hdc, CalcKeyPos(key.time - offset), CalcLinePos(index - vert_scroll->GetPosition()) + 2,
					          CalcKeyPos(key.time - offset) + 5, CalcLinePos(index - vert_scroll->GetPosition()) + 18);
				}
			}

			index++;
			draw_index++;
		}

		last_y += 20 * (int)(owner->players[i]->tracks.size() + 1);
	}

	SetDCBrushColor(hdc, RGB(225, 225, 225));
	Rectangle(hdc, 0, last_y, (int)timeline_panel->GetWidth() + 1, (int)timeline_panel->GetHeight() + 1);

	if ((owner->edCurTime - offset >= 0.0f) && (owner->edCurTime - offset <= tm_width))
	{
		MoveToEx(hdc, CalcKeyPos(owner->edCurTime - offset) + 2, 12, 0);
		LineTo(hdc, CalcKeyPos(owner->edCurTime - offset) + 2, CalcLinePos(max_lines));
	}

	if ((owner->fromTime - offset >= 0.0f) && (owner->fromTime - offset <= tm_width))
	{
		MoveToEx(hdc, CalcKeyPos(owner->fromTime - offset) + 2, 12, 0);
		LineTo(hdc,  CalcKeyPos(owner->fromTime - offset) + 2, CalcLinePos(max_lines));
	}

	if ((owner->toTime - offset >= 0.0f) && (owner->toTime - offset <= tm_width))
	{
		MoveToEx(hdc, CalcKeyPos(owner->toTime - offset) + 2, 12, 0);
		LineTo(hdc, CalcKeyPos(owner->toTime - offset) + 2, CalcLinePos(max_lines));
	}

	if ((owner->fromTime - offset >= 0.0f) && (owner->fromTime - offset <= tm_width))
	{
		Rectangle(hdc, CalcKeyPos(owner->fromTime - offset), 2, CalcKeyPos(owner->fromTime - offset) + 5, 16);
	}

	if ((owner->toTime - offset >= 0.0f) && (owner->toTime - offset <= tm_width))
	{
		Rectangle(hdc, CalcKeyPos(owner->toTime - offset), 2, CalcKeyPos(owner->toTime - offset) + 5, 16);
	}

	SetDCBrushColor(hdc, RGB(255,255,255));

	if ((owner->edCurTime - offset >= 0.0f) && (owner->edCurTime - offset <= scale))
	{
		Rectangle(hdc, CalcKeyPos(owner->edCurTime - offset), 2, CalcKeyPos(owner->edCurTime - offset) + 5, 16);
	}

	BitBlt(wnd_hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);

	ReleaseDC(hwnd, wnd_hdc);
}

void AnimatorWindow::OnResize(EUIWidget* sender)
{
	UpdateHorzBar();
	UpdateVertBar();
}

void AnimatorWindow::OnUpdate(EUIWidget* sender)
{
	if (play)
	{
		owner->edCurTime += Timer::GetDeltaTime();

		if (owner->edCurTime > owner->toTime - 0.001f)
		{
			owner->edCurTime = owner->fromTime - 0.001f;
		}

		char str[32];
		sprintf(str, "%4.2f", owner->edCurTime);
		tb_curTime->SetText(str);
	}
}

void AnimatorWindow::SetSelection(int player, int track, int key)
{
	if (selPlayer != -1)
	{
		if (selTrack != -1 && selKey != -1)
		{
			owner->players[selPlayer]->tracks[selTrack]->StopEditKey();
		}
		else
		{
			owner->players[selPlayer]->GetMetaData()->HideWidgets();
		}
	}

	selPlayer = player;

	if (selPlayer != -1)
	{
		selTrack = track;
		selKey = key;

		if (selTrack != -1 && selKey != -1)
		{
			owner->players[selPlayer]->tracks[selTrack]->StartEditKey(selKey);
		}
		else
		{
			owner->players[selPlayer]->GetMetaData()->Prepare(owner->players[selPlayer]);
			owner->players[selPlayer]->GetMetaData()->PrepareWidgets(prop_dock);
		}
	}
	else
	{
		selTrack = -1;
		selKey = -1;
	}

	timeline_panel->Redraw();
}

void AnimatorWindow::SetCurTime(float time)
{
	float ct = ((int)(time * 10.0f) * 0.1f);

	if (fabs(owner->edCurTime - ct) < 0.0001f) return;

	owner->edCurTime = ct;

	if (owner->edCurTime< owner->fromTime)
	{
		owner->edCurTime = owner->fromTime;
	}

	if (owner->edCurTime> owner->toTime)
	{
		owner->edCurTime = owner->toTime;
	}

	char str[32];
	sprintf(str, "%4.2f", owner->edCurTime);
	tb_curTime->SetText(str);

	timeline_panel->Redraw();
}

void AnimatorWindow::SetTimeLenght(float time)
{
	if (fabs(owner->timeLenght - owner->toTime) < 0.01f || owner->toTime > time)
	{
		SetToTime(time);
	}

	if (owner->fromTime > owner->timeLenght)
	{
		SetFromTime(0.0f);
	}

	owner->timeLenght = time;

	if (owner->edCurTime > owner->timeLenght)
	{
		SetCurTime(owner->timeLenght);
	}

	UpdateHorzBar();
}

void AnimatorWindow::SetFromTime(float time)
{
	owner->fromTime = ((int)(time * 10.0f) * 0.1f);

	if (owner->fromTime < 0.0f)
	{
		owner->fromTime = 0.0f;
	}

	if (owner->fromTime > owner->edCurTime)
	{
		owner->fromTime = owner->edCurTime;
	}

	char str[32];
	sprintf(str, "%4.2f", owner->fromTime);
	tb_fromTime->SetText(str);

	timeline_panel->Redraw();
}

void AnimatorWindow::SetToTime(float time)
{
	owner->toTime = ((int)(time * 10.0f) * 0.1f);

	if (owner->toTime < owner->edCurTime)
	{
		owner->toTime = owner->edCurTime;
	}

	if (owner->toTime > owner->timeLenght)
	{
		owner->toTime = owner->timeLenght;
	}

	char str[32];
	sprintf(str, "%4.2f", owner->toTime);
	tb_toTime->SetText(str);

	timeline_panel->Redraw();
}

void AnimatorWindow::AddKey()
{
	if (selTrack == -1)
	{
		return;
	}

	SetSelection(selPlayer, selTrack, owner->players[selPlayer]->tracks[selTrack]->AddKey(owner->edCurTime));
}

void AnimatorWindow::DelKey()
{
	if (selKey == -1)
	{
		return;
	}

	owner->players[selPlayer]->tracks[selTrack]->DelKey(selKey);
	SetSelection(selPlayer, selTrack, -1);
}

void AnimatorWindow::UpdateVertBar()
{
	int count = 0;

	for (int i = 0; i<(int)owner->players.size(); i++)
	{
		count += 1 + (int)owner->players[i]->tracks.size();
	}

	count = (int)fmax(0.0f, count - (int)((timeline_panel->GetHeight() - CaptionHeight) / RowHeight));

	vert_scroll->SetLimit(1, count);
}

void AnimatorWindow::UpdateHorzBar()
{
	float tm_width = fmax(0.0f, owner->timeLenght - (timeline_panel->GetWidth() - LeftRowWidth - 4.0f) / scale);
	horz_scroll->SetLimit(1, (int)(tm_width * 10.0f + 3));
}

int AnimatorWindow::CalcKeyPos(float ps_x)
{
	return (int)(ps_x * scale) + LeftRowWidth + 2;
}

int AnimatorWindow::CalcLinePos(int index)
{
	return index * RowHeight + CaptionHeight;
}

float AnimatorWindow::PixelToTime(float px)
{
	return ((int)(px * 0.1f) * 10) / scale;
}

#endif