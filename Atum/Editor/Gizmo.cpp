
#include "Gizmo.h"
#include "SceneObjects/2D/Sprite.h"
#include "EditorDrawer.h"
#include "Services/Core/Core.h"

Gizmo* Gizmo::inst = nullptr;

Gizmo::Transform2D::Transform2D(Vector2* set_pos, Vector2* set_size, Vector2* set_offset, float* set_depth, float* set_rotation, Vector2* set_axis, Matrix set_mat_parent)
{
	pos = set_pos;
	size = set_size;
	offset = set_offset;
	rotation = set_rotation;
	depth = set_depth;
	axis = set_axis;
	mat_parent = set_mat_parent;
}

Gizmo::Transform2D::Transform2D(::Transform2D& trans)
{
	pos = &trans.pos;
	size = &trans.size;
	offset = &trans.offset;
	rotation = &trans.rotation;
	depth = &trans.depth;
	axis = &trans.axis;
	mat_parent = trans.mat_parent;
}

void Gizmo::Transform2D::BuildMatrices()
{
	mat_local.Identity();

	if (rotation)
	{
		mat_local.RotateZ(*rotation);
	}

	mat_local.Pos() = Vector((!axis || axis->x > 0.0f) ? pos->x : -pos->x - size->x,
		(!axis || axis->y > 0.0f) ? pos->y : -pos->y - size->y,
		depth ? *depth : 0.5f);

	mat_global = mat_local * mat_parent;
}

void Gizmo::Init()
{
	inst = this;
}

bool Gizmo::IsTrans2D()
{
	return use_trans2D;
}

void Gizmo::SetTrans2D(Transform2D trans, int actions, bool set_ignore_2d_camera)
{
	use_trans2D = true;

	trans2D = trans;
	pos2d = *trans2D.pos;

	trans2D_actions = actions;
	ignore_2d_camera = set_ignore_2d_camera;
	enabled = true;

	delta_move = 0.0f;
}

void Gizmo::SetTrans3D(Matrix set_transform)
{
	use_trans2D = false;

	transform = set_transform;
	enabled = true;
}

Matrix& Gizmo::GetTrans3D()
{
	return transform;
}

bool Gizmo::IsEnabled()
{
	return enabled;
}

void Gizmo::Disable()
{
	enabled = false;
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

	core.render.DebugLine(dir, color, tr, color, false);

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

		core.render.DebugLine(dir, color, pos + tr, color, false);
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
	core.render.GetTransform(Render::View, view);

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
				core.render.DebugLine(pos, color_gray, pos2, color_gray, false);
			}
			else
			{
				core.render.DebugLine(pos, color, pos2, color, false);
			}
		}

		last_dx = dx;
		last_dz = dz;
	}
}

bool Gizmo::CheckInersection(Vector pos, Vector pos2, Vector2 ms,
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
		pos_post = core.render.TransformToScreen(pos, 1);
		pos2_post = core.render.TransformToScreen(pos2, 1);
		
		float x1 = fmin(pos_post.x , pos2_post.x );
		float x2 = fmax(pos_post.x , pos2_post.x );

		float y1 = fmax(pos_post.y , pos2_post.y );
		float y2 = fmin(pos_post.y , pos2_post.y );

		float sz = 0.025f;
		x1 += -sz;
		x2 +=  sz;
		y1 += -sz;
		y2 +=  sz;

		if (x1 < ms.x && ms.x < x2 &&
			y1 < ms.y && ms.y < y2)
		{
			ms_dir = Vector2(pos2_post.x - pos_post.x, pos2_post.y - pos_post.y);
			ms_dir.Normalize();

			return true;
		}
	}

	return false;
}

void Gizmo::CheckSelectionTrans2D(Vector2 ms)
{
	if (trans2D_actions & trans_2d_scale)
	{
		for (int i = 0; i < 8; i++)
		{
			if (ancorns[i].x - 7 < ms.x && ms.x < ancorns[i].x + 7 &&
				ancorns[i].y - 7 < ms.y && ms.y < ancorns[i].y + 7)
			{
				selAxis = i + 1;
				break;
			}
		}
	}

	if (trans2D_actions & trans_2d_anchorn)
	{
		if (origin.x - 7 < ms.x && ms.x < origin.x + 7 &&
			origin.y - 7 < ms.y && ms.y < origin.y + 7)
		{
			selAxis = 10;
			moved_origin = origin;
		}
	}

	if (selAxis == -1)
	{
		if (IsInsideTriangle(ms, ancorns[0], ancorns[1], ancorns[2]) ||
			IsInsideTriangle(ms, ancorns[0], ancorns[2], ancorns[3]))
		{
			//SetCursor(cr_move);

			if (trans2D_actions & trans_2d_move)
			{
				selAxis = 0;
			}
		}
		else
		{
			//SetCursor(cr_rotate);

			if (trans2D_actions & trans_2d_rotate)
			{
				selAxis = 9;
			}
		}
	}
}

bool Gizmo::CheckSelectionTrans3D(int axis, Vector2 ms)
{
	Matrix view;
	core.render.GetTransform(Render::View, view);

	Matrix view_proj;
	core.render.GetTransform(Render::Projection, view_proj);
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

		int count = 7;

		for (int i=0; i<count; i++)
		{
			Vector pos = dir * (float)i * (1.0f / (float)count);
			Vector pos2 = dir * (float)(i+1) * (1.0f / (float)count);
			Vector tr(0.0f);

			if (CheckInersection(pos, pos2, ms, tr, false, view, view_proj))
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

				if (CheckInersection(pos, pos2, ms, trans, true, view, view_proj))
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

void Gizmo::CheckSelectionTrans3D(Vector2 ms)
{
	ms.x /= (float)core.render.GetDevice()->GetWidth();
	ms.y /= (float)core.render.GetDevice()->GetHeight();

	for (int i = 0; i < 3; i++)
	{
		if (CheckSelectionTrans3D(i, ms))
		{
			selAxis = i;
		}
	}
}

void Gizmo::MoveTrans2D(Vector2 ms)
{
	ms *= (1024.0f / core.render.GetDevice()->GetHeight());

	if (ms.Length() < 0.001f)
	{
		return;
	}

	if (selAxis == 0)
	{
		pos2d.x += (!trans2D.axis || trans2D.axis->x > 0.0f) ? ms.x : -ms.x;
		pos2d.y += (!trans2D.axis || trans2D.axis->y > 0.0f) ? ms.y : -ms.y;

		Vector2 prev_pos = *trans2D.pos;
		*trans2D.pos = MakeAligned(pos2d);
		delta_move += *trans2D.pos - prev_pos;
	}
	else
	if (selAxis == 9 && core.controls.DebugKeyPressed("Z", Controls::Active))
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

			if (trans2D.rotation)
			{
				*trans2D.rotation += k;
			}
		}
	}
	else
	if (selAxis == 10)
	{
		moved_origin += ms * core.render.GetDevice()->GetHeight() / 1024.0f;
	}
	else
	if (selAxis > 0)
	{
		float dist = ms.Length();

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

		dot1 = ms.x * trans2D.mat_global.Vx().x + ms.y * trans2D.mat_global.Vx().y;
		dot2 = ms.x * trans2D.mat_global.Vy().x + ms.y * trans2D.mat_global.Vy().y;

		trans2D.size->x += dist * k1 * dot1;
		trans2D.size->y += dist * k2 * dot2;
	}
}

void Gizmo::MoveTrans3D(Vector2 ms)
{
	if (selAxis == -1)
	{
		return;
	}

	Vector2 cur_dir(ms.x / (float)core.render.GetDevice()->GetWidth(), ms.y / (float)core.render.GetDevice()->GetHeight());
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

void Gizmo::RenderTrans2D()
{
	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	trans2D.BuildMatrices();

	Vector p1, p2;

	for (int phase = 1; phase <= 2; phase++)
	{
		for (int i = 0; i < 4 * phase; i++)
		{
			if (i == 0)
			{
				p1 = Vector(0, 0, 0);
				p2 = Vector(trans2D.size->x, 0, 0);
			}
			else
			if (i == 1)
			{
				p1 = Vector(trans2D.size->x, 0, 0);
				p2 = Vector(trans2D.size->x, trans2D.size->y, 0);
			}
			else
			if (i == 2)
			{
				p1 = Vector(trans2D.size->x, trans2D.size->y, 0);
				p2 = Vector(0, trans2D.size->y, 0);
			}
			else
			if (i == 3)
			{
				p1 = Vector(0, trans2D.size->y, 0);
				p2 = Vector(0, 0, 0);
			}
			else
			if (i == 4)
			{
				p1 = Vector(trans2D.size->x * 0.5f, 0, 0);
			}
			else
			if (i == 5)
			{
				p1 = Vector(trans2D.size->x, trans2D.size->y * 0.5f, 0);
			}
			else
			if (i == 6)
			{
				p1 = Vector(trans2D.size->x * 0.5f, trans2D.size->y, 0);
			}
			else
			if (i == 7)
			{
				p1 = Vector(0, trans2D.size->y * 0.5f, 0);
			}

			p1 -= Vector((trans2D.offset ? trans2D.offset->x : 0.5f) * trans2D.size->x, (trans2D.offset ? trans2D.offset->y : 0.5f) * trans2D.size->y, 0);
			p1 = p1 * trans2D.mat_global;
			p2 -= Vector((trans2D.offset ? trans2D.offset->x : 0.5f) * trans2D.size->x, (trans2D.offset ? trans2D.offset->y : 0.5f) * trans2D.size->y, 0);
			p2 = p2 * trans2D.mat_global;

			if (!ignore_2d_camera)
			{
				p1 -= Vector(Sprite::ed_cam_pos.x - core.render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - core.render.GetDevice()->GetHeight() * 0.5f, 0) / scale;
				p2 -= Vector(Sprite::ed_cam_pos.x - core.render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - core.render.GetDevice()->GetHeight() * 0.5f, 0) / scale;
			}

			if (phase == 1)
			{
				core.render.DebugLine2D(Vector2(p1.x, p1.y) * scale, COLOR_WHITE, Vector2(p2.x, p2.y) * scale, COLOR_WHITE);
			}
			else
			{
				ancorns[i] = Vector2(p1.x, p1.y) * scale;
				core.render.DebugSprite(editor_drawer.anchorn, ancorns[i] - Vector2(4.0f), Vector2(8.0f), selAxis == (i + 1) ? Color(1.0, 0.9f, 0.0f, 1.0f) : COLOR_WHITE);
			}
		}
	}

	p1 = Vector(0.0f, 0.0f, 0.0f);
	p1 = p1 * trans2D.mat_global;

	if (!ignore_2d_camera)
	{
		p1 -= Vector(Sprite::ed_cam_pos.x - core.render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - core.render.GetDevice()->GetHeight() * 0.5f, 0) / scale;
	}

	origin = Vector2(p1.x, p1.y) * scale;

	core.render.DebugSprite(editor_drawer.center, ((selAxis == 10) ? moved_origin : origin) - Vector2(4.0f), Vector2(8.0f));
}

void Gizmo::RenderTrans3D()
{
	Matrix view;
	core.render.GetTransform(Render::View, view);

	Matrix view_proj;
	core.render.GetTransform(Render::Projection, view_proj);
	view_proj = view * view_proj;

	Vector pos = transform.Pos();
	float z = pos.x*view_proj._13 + pos.y*view_proj._23 + pos.z*view_proj._33 + view_proj._43;

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

Vector2 Gizmo::MakeAligned(Vector2 pos)
{
	if (align2d.x > 0.01f && pos.x < 0.0f)
	{
		pos.x -= align2d.x;
	}

	if (align2d.y > 0.01f && pos.y < 0.0f)
	{
		pos.y -= align2d.y;
	}

	return Vector2((align2d.x > 0.01f) ? (align2d.x * ((int)(pos.x / align2d.x))) : pos.x,
	               (align2d.y > 0.01f) ? (align2d.y * ((int)(pos.y / align2d.y))) : pos.y);
}

void Gizmo::Render()
{
	if (!enabled) return;

	if (IsTrans2D())
	{
		RenderTrans2D();
	}
	else
	{
		RenderTrans3D();
	} 
}

void Gizmo::OnMouseMove(Vector2 ms)
{
	if (!enabled) return;

	if (mousedPressed)
	{
		if (IsTrans2D())
		{
			MoveTrans2D(ms - prev_ms);
		}
		else
		{
			MoveTrans3D(ms - prev_ms);
		}
	}
	else
	{
		selAxis = -1;

		if (IsTrans2D())
		{
			CheckSelectionTrans2D(ms);
		}
		else
		{
			CheckSelectionTrans3D(ms);
		}
	}

	prev_ms = ms;
}

void Gizmo::OnLeftMouseDown()
{
	if (selAxis != -1)
	{
		mousedPressed = true;
	}
}

void Gizmo::OnLeftMouseUp()
{
	mousedPressed = false;

	if (selAxis == 10 && IsTrans2D())
	{
		Matrix inv = trans2D.mat_global;
		inv.Inverse();

		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

		moved_origin /= scale;

		if (!ignore_2d_camera)
		{
			moved_origin += Vector2(Sprite::ed_cam_pos.x - core.render.GetDevice()->GetWidth() * 0.5f, Sprite::ed_cam_pos.y - core.render.GetDevice()->GetHeight() * 0.5f) / scale;
		}

		Vector pos = Vector(moved_origin.x, moved_origin.y, 0.0f) * inv / Vector(trans2D.size->x, trans2D.size->y, 1.0f);
		*trans2D.offset += Vector2(pos.x, pos.y);
		*trans2D.pos += Vector2(pos.x, pos.y) * (*trans2D.size);
		pos2d = *trans2D.pos;

		selAxis = -1;
	}
}