
#include "Gizmo.h"
#include "Services/Controls/Controls.h"
#include "Services/Render/Render.h"
#include "SceneAssets/Sprite.h"

Gizmo* Gizmo::inst = nullptr;

void Gizmo::Init()
{
	inst = this;
	anchorn = render.LoadTexture("settings\\editor\\gizmo_anch.png");
	anchorn->SetFilters(Texture::FilterType::Point, Texture::FilterType::Point);
	center = render.LoadTexture("settings\\editor\\gizmo_center.png");
	center->SetFilters(Texture::FilterType::Point, Texture::FilterType::Point);
}

bool Gizmo::IsInsideTriangle(Vector2 s, Vector2 a, Vector2 b, Vector2 c)
{
	float as_x = s.x - a.x;
	float as_y = s.y - a.y;

	bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;

	if ((c.x - a.x)*as_y - (c.y - a.y)*as_x > 0 == s_ab) return false;
	if ((c.x - b.x)*(s.y - b.y) - (c.y - b.y)*(s.x - b.x) > 0 != s_ab) return false;

	return true;
}

Color Gizmo::CheckColor(int axis)
{
	Color color;

	if (axis == selAxis)
	{
		color = COLOR_YELLOW;
	}
	else
	{
		if (axis == 0)
		{
			color = COLOR_RED;
		}
		else
		if (axis == 1)
		{
			color = COLOR_GREEN;
		}
		else
		{
			color = COLOR_BLUE;
		}
	}

	return color;
}

void Gizmo::DrawAxis(int axis)
{
	Vector tr = transform.Pos();

	Color color = CheckColor(axis);
	Vector dir;
	dir.Set(0.0f);

	if (axis == 0)
	{
		dir.x = scale;
	}
	else
	if (axis == 1)
	{
		dir.y = scale;
	}
	else
	{
		dir.z = scale;
	}

	if (useLocalSpace)
	{
		dir = transform.MulNormal(dir);
	}

	dir += tr;

	render.DebugLine(dir, color, tr, color, false);

	float hgt = 0.85f * scale;
	float r = 0.05f * scale;
	int nums = 64;

	for (int i = 0; i < nums; i++)
	{
		float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
		float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

		Vector pos;

		if (axis == 0)
		{
			pos = Vector(hgt, dx, dz); 
		}
		else
		if (axis == 1)
		{
			pos = Vector(dx, hgt, dz);
		}
		else
		{
			pos = Vector(dx, dz, hgt);
		}

		if (useLocalSpace)
		{
			pos = transform.MulNormal(pos);
		}

		render.DebugLine(dir, color, pos + tr, color, false);
	}
}

void Gizmo::DrawCircle(int axis)
{
	Color color = CheckColor(axis);
	Color color_gray = Color(color.r * 0.4f, color.g * 0.4f, color.b * 0.4f);

	Matrix mat;
	if (useLocalSpace)
	{
		mat = transform;
	}
	else
	{
		mat.Pos() = transform.Pos();
	}

	Matrix view;
	render.GetTransform(Render::View, view);

	Vector tr = mat.Pos();
	Vector trans = tr * view;

	float last_dx = -scale * 2;
	float last_dz = -scale * 2;
	int nums = 32;

	for (int i = 0; i < nums+1; i++)
	{
		float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * scale;
		float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * scale;

		if (last_dx > -scale * 1.5f)
		{
			Vector pos;
			Vector pos2;

			if (axis == 0)
			{
				pos.Set(0.0f, last_dx, last_dz);
				pos2.Set(0.0f, dx, dz);
			}
			else
			if (axis == 1)
			{
				pos.Set(last_dx, 0.0f, last_dz);
				pos2.Set(dx, 0.0f, dz);
			}
			else
			{
				pos.Set(last_dx, last_dz, 0.0f);
				pos2.Set(dx, dz, 0.0f);
			}

			pos = mat.MulNormal(pos);
			pos2 = mat.MulNormal(pos2);

			pos += tr;
			pos2 += tr;

			Vector pos_post = pos * view;
			Vector pos2_post = pos2 * view;

			if (pos_post.z > trans.z && pos2_post.z > trans.z)
			{
				render.DebugLine(pos, color_gray, pos2, color_gray, false);
			}
			else
			{
				render.DebugLine(pos, color, pos2, color, false);
			}
		}

		last_dx = dx;
		last_dz = dz;
	}
}

bool Gizmo::CheckInersection(Vector pos, Vector pos2,
                             float mx, float my,
                             Vector trans, bool check_trans,
                             Matrix view, Matrix view_proj)
{
	if (useLocalSpace)
	{
		pos = transform.MulNormal(pos);
		pos2 = transform.MulNormal(pos2);
	}

	Vector tr = transform.Pos();

	pos += tr;
	pos2 += tr;

	Vector pos_post = pos * view;
	Vector pos2_post = pos2 * view;

	bool proceed = true;
	if (check_trans)
	{
		if (pos_post.z > trans.z && pos2_post.z > trans.z)
		{
			proceed = false;
		}
	}

	if (proceed)
	{
		pos_post = render.TransformToScreen(pos, 1);
		pos2_post = render.TransformToScreen(pos2, 1);
		
		float x1 = fmin(pos_post.x , pos2_post.x );
		float x2 = fmax(pos_post.x , pos2_post.x );

		float y1 = fmax(pos_post.y , pos2_post.y );
		float y2 = fmin(pos_post.y , pos2_post.y );

		float sz = 0.025f;
		x1 += -sz;
		x2 +=  sz;
		y1 += -sz;
		y2 +=  sz;

		if (x1 < mx && mx < x2 &&
			y1 < my && my < y2)
		{
			ms_dir = Vector2(pos2_post.x - pos_post.x, pos2_post.y - pos_post.y);
			ms_dir.Normalize();

			return true;
		}
	}

	return false;
}

bool Gizmo::MouseProcess(int axis, float mx, float my)
{
	Matrix view;
	render.GetTransform(Render::View, view);

	Matrix view_proj;
	render.GetTransform(Render::Projection, view_proj);
	view_proj = view * view_proj;

	if (mode == 0)
	{
		Vector dir;
		dir.Set(0.0f);

		if (axis == 0)
		{
			dir.x = scale;
		}
		else
		if (axis == 1)
		{
			dir.y = scale;
		}
		else
		{
			dir.z = scale;
		}

		for (int i=0; i<7; i++)
		{
			Vector pos = dir * (float)i * (1.0f / 7.0f);
			Vector pos2 = dir * (float)(i+1) * (1.0f / 7.0f);
			Vector tr(0.0f);

			if (CheckInersection(pos, pos2, mx, my, tr, false, view, view_proj))
			{
				return true;
			}
		}
	}
	else
	if (mode == 1)
	{
		Vector trans = transform.Pos() * view;

		float r = scale;
		float last_dx = -r * 2;
		float last_dz = -r * 2;
		int nums = 32;

		for (int i = 0; i < nums + 1; i++)
		{
			float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
			float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

			if (last_dx > -r * 1.5f)
			{
				Vector pos;
				Vector pos2;

				if (axis == 0)
				{
					pos = Vector(0.0f, last_dx, last_dz);
					pos2 = Vector(0.0f, dx, dz);
				}
				else
				if (axis == 1)
				{
					pos = Vector(last_dx, 0.0f, last_dz);
					pos2 = Vector(dx, 0.0f, dz);
				}
				else
				{
					pos = Vector(last_dx, last_dz, 0.0f);
					pos2 = Vector(dx, dz, 0.0f);
				}

				if (CheckInersection(pos, pos2,
				                     mx, my, trans, true,
				                     view, view_proj))
				{
					return true;
				}
			}

			last_dx = dx;
			last_dz = dz;
		}
	}

	return false;
}

void Gizmo::MouseProcess(float mx, float my)
{
	selAxis = -1;

	if (MouseProcess(0, mx, my)) selAxis = 0;
	if (MouseProcess(1, mx, my)) selAxis = 1;
	if (MouseProcess(2, mx, my)) selAxis = 2;
}

void Gizmo::MouseMove(float mx, float my)
{
	if (trans2D)
	{
		Vector2 ms = Vector2(mx, my);

		if (selAxis == 0)
		{
			pos2d.x += (trans2D->axis.x > 0.0f) ? ms.x : -ms.x;
			pos2d.y += (trans2D->axis.y > 0.0f) ? ms.y : -ms.y;
		}
		else
		if (selAxis == 9 && controls.DebugKeyPressed("KEY_LALT", Controls::Active))
		{
			Vector2 p1 = prev_ms + ms - origin;
			p1.Normalize();

			Vector2 p2 = prev_ms - origin;
			p2.Normalize();

			float k = p1.x * p2.x + p1.y * p2.y;

			if (fabs(k) < 1.0f)
			{
				k = acosf(k);

				Vector2 p3(-p2.y, p2.x);

				float k2 = p1.x * p3.x + p1.y * p3.y;

				if (k2 < 0)
				{
					k *= -1.0f;
				}

				trans2D->rotation += k;
			}
		}
		else
		if (selAxis == 10)
		{
			moved_origin += ms;
		}
		else
		if (selAxis > 0)
		{
			float dist = ms.Length();

			if (dist > 0.01f)
			{
				ms.Normalize();

				float k1 = 1.0f;
				float k2 = 1.0f;

				if (selAxis == 5 || selAxis == 7)
				{
					k1 = 0.0f;
				}

				if (selAxis == 1 || selAxis == 2 || selAxis == 5)
				{
					k2 = -1.0f;
				}

				if (selAxis == 0 || selAxis== 1 || selAxis == 4 || selAxis == 8)
				{
					k1 = -1.0f;
				}

				if (selAxis == 6 || selAxis == 8)
				{
					k2 = 0.0f;
				}

				float dot1 = 0.0f;
				float dot2 = 0.0f;

				dot1 = ms.x * trans2D->mat_global.Vx().x + ms.y * trans2D->mat_global.Vx().y;
				dot2 = ms.x * trans2D->mat_global.Vy().x + ms.y * trans2D->mat_global.Vy().y;

				trans2D->size.x += dist * k1 * dot1;
				trans2D->size.y += dist * k2 * dot2;
			}
		}

		trans2D->pos.x = align.x * ((int)(pos2d.x / align.x));
		trans2D->pos.y = align.y * ((int)(pos2d.y / align.y));

		return;
	}

	if (selAxis == -1)
	{
		return;
	}

	Vector2 cur_dir(mx, my);
	float da = cur_dir.Length();
	cur_dir.Normalize();

	da *= cur_dir.Dot(ms_dir);

	if (mode == 0)
	{
		da *= scale * 16;

		if (selAxis == 0)
		{
			transform._41 += da;
		}
		else
		if (selAxis == 1)
		{
			transform._42 += da;
		}
		else
		{
			transform._43 += da;
		}
	}
	else
	if (mode == 1)
	{
		da *= -5.0f;

		Matrix rot;

		if (selAxis == 0)
		{
			rot.RotateX(da);
		}
		else
		if (selAxis == 1)
		{
			rot.RotateY(-da);
		}
		else
		{
			rot.RotateZ(da);
		}

		if (useLocalSpace)
		{
			transform = rot * transform;
		}
		else
		{
			Vector tr = transform.Pos();
			transform = transform * rot;
			transform.Pos() = tr;
		}
	}
}

void Gizmo::Render()
{
	if (!enabled) return;

	if (trans2D)
	{
		float scale = render.GetDevice()->GetHeight() / 1024.0f;

		trans2D->BuildMatrices();

		Vector p1, p2;

		for (int phase = 1; phase <= 2; phase++)
		{
			if (!allow_transform && phase != 1)
			{
				continue;
			}

			for (int i = 0; i < 4 * phase; i++)
			{
				if (i == 0)
				{
					p1 = Vector(0, 0, 0);
					p2 = Vector(trans2D->size.x, 0, 0);
				}
				else
				if (i == 1)
				{
					p1 = Vector(trans2D->size.x, 0, 0);
					p2 = Vector(trans2D->size.x, trans2D->size.y, 0);
				}
				else
				if (i == 2)
				{
					p1 = Vector(trans2D->size.x, trans2D->size.y, 0);
					p2 = Vector(0, trans2D->size.y, 0);
				}
				else
				if (i == 3)
				{
					p1 = Vector(0, trans2D->size.y, 0);
					p2 = Vector(0, 0, 0);
				}
				else
				if (i == 4)
				{
					p1 = Vector(trans2D->size.x * 0.5f, 0, 0);
				}
				else
				if (i == 5)
				{
					p1 = Vector(trans2D->size.x, trans2D->size.y * 0.5f, 0);
				}
				else
				if (i == 6)
				{
					p1 = Vector(trans2D->size.x * 0.5f, trans2D->size.y, 0);
				}
				else
				if (i == 7)
				{
					p1 = Vector(0, trans2D->size.y * 0.5f, 0);
				}

				p1 -= Vector(trans2D->offset.x * trans2D->size.x, trans2D->offset.y * trans2D->size.y, 0);
				p1 = p1 * trans2D->mat_global;
				p2 -= Vector(trans2D->offset.x * trans2D->size.x, trans2D->offset.y * trans2D->size.y, 0);
				p2 = p2 * trans2D->mat_global;

				p1 -= Vector(Sprite::ed_cam_pos.x - render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - render.GetDevice()->GetHeight() * 0.5f, 0) / scale;
				p2 -= Vector(Sprite::ed_cam_pos.x - render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - render.GetDevice()->GetHeight() * 0.5f, 0) / scale;

				if (phase == 1)
				{
					render.DebugLine2D(Vector2(p1.x, p1.y) * scale, COLOR_WHITE, Vector2(p2.x, p2.y) * scale, COLOR_WHITE);
				}
				else
				{
					ancorns[i] = Vector2(p1.x, p1.y) * scale;
					render.DebugSprite(anchorn, ancorns[i] - Vector2(4.0f), Vector2(8.0f));
				}
			}
		}

		if (allow_transform)
		{
			p1 = Vector(0.0f, 0.0f, 0.0f);
			p1 = p1 * trans2D->mat_global;
			p1 -= Vector(Sprite::ed_cam_pos.x - render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - render.GetDevice()->GetHeight() * 0.5f, 0) / scale;

			origin = Vector2(p1.x, p1.y) * scale;

			render.DebugSprite(center, ((selAxis == 10) ? moved_origin : origin) - Vector2(4.0f), Vector2(8.0f));
		}

		return;
	}

	Matrix view;
	render.GetTransform(Render::View, view );

	Matrix view_proj;
	render.GetTransform(Render::Projection, view_proj );
	view_proj = view * view_proj;
	
	Vector pos = transform.Pos();
	float z=pos.x*view_proj._13+pos.y*view_proj._23+pos.z*view_proj._33+view_proj._43;    

	scale = 0.1f * (1.0f + z);
	scale = fabs(scale);

	if (mode == 0)
	{
		DrawAxis(0);
		DrawAxis(1);
		DrawAxis(2);
	}
	else
	{
		DrawCircle(0);
		DrawCircle(1);
		DrawCircle(2);
	} 
}

void Gizmo::OnMouseMove(float mx, float my)
{
	if (!enabled || !allow_transform) return;

	if (mousedPressed)
	{
		MouseMove(mx - prev_ms.x, my - prev_ms.y);
		prev_ms.x = mx;
		prev_ms.y = my;
	}
	else
	{
		MouseProcess(mx, my);
	}
}

void Gizmo::OnLeftMouseDown(float mx, float my)
{
	if (selAxis != -1 || trans2D)
	{
		if (trans2D)
		{
			selAxis = -1;

			for (int i = 0; i<8; i++)
			{
				if (ancorns[i].x - 7 < mx && mx < ancorns[i].x + 7 &&
				    ancorns[i].y - 7 < my && my < ancorns[i].y + 7)
				{
					//SetCursor(cr_resize);
					selAxis = i + 1;
					break;
				}
			}

			if (origin.x - 7 < mx && mx < origin.x + 7 &&
				origin.y - 7 < my && my < origin.y + 7)
			{
				selAxis = 10;
				moved_origin = origin;
			}

			if (selAxis == -1)
			{
				if (IsInsideTriangle(Vector2(mx, my), ancorns[0], ancorns[1], ancorns[2]) ||
				    IsInsideTriangle(Vector2(mx, my), ancorns[0], ancorns[2], ancorns[3]))
				{
					//SetCursor(cr_move);
					selAxis = 0;
				}
				else
				{
					//SetCursor(cr_rotate);
					selAxis = 9;
				}
			}
		}

		prev_ms.x = mx;
		prev_ms.y = my;
		mousedPressed = true;
	}
}

void Gizmo::OnLeftMouseUp()
{
	mousedPressed = false;

	if (selAxis == 10 && trans2D)
	{
		Matrix inv = trans2D->mat_global;
		inv.InverseComplette();

		float scale = render.GetDevice()->GetHeight() / 1024.0f;

		moved_origin /= scale;
		moved_origin += Vector2(Sprite::ed_cam_pos.x - render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - render.GetDevice()->GetHeight() * 0.5f) / scale;

		Vector pos = Vector(moved_origin.x, moved_origin.y, 0.0f) * inv / Vector(trans2D->size.x, trans2D->size.y, 1.0f);
		trans2D->offset += Vector2(pos.x, pos.y);
		trans2D->pos += Vector2(pos.x, pos.y) * trans2D->size;
		pos2d = trans2D->pos;
		selAxis = -1;
	}
}