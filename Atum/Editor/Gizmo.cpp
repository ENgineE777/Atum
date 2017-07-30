
#include "Gizmo.h"
#include "Services/Render/Render.h"

Vector TransformToScreen(const Vector& pos, int type)
{
	Matrix view;
	render.GetTransform(Render::View, view);

	Matrix view_proj;
	render.GetTransform(Render::WrldViewProj, view_proj);

	Vector pre_ps = pos * view;
	Vector4 ps2 = view_proj.MulVertex4(pos);
	Vector ps;
	ps.Set(ps2.x / ps2.w, ps2.y / ps2.w, ps2.z);

	if (type == 0)
	{
		ps.x = ps.x;
		ps.y = ps.y;
		ps.z = pre_ps.z;
	}
	else
		if (type == 1 || type == 2)
		{
			ps.x = 0.5f + ps.x*0.5f;
			ps.y = 0.5f - ps.y*0.5f;
			ps.z = pre_ps.z;

			if (type == 2)
			{
				//ps.x *= GetWidth();
				//ps.y *= GetHeight();
			}
		}

	return ps;
}

Gizmo::Gizmo()
{
	bViewPortResized = false;
	mode = 1;
	selAxis = -1;
	enabled = false;
	useLocalSpace = false;
	scale = 1.0f;
	transform.Identity();
	mousedPressed = false;
	prev_mx = prev_my = 0;
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
	int nums = 32;

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
		pos_post = TransformToScreen(pos, 1);
		pos2_post = TransformToScreen(pos2, 1);
		
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
			ms_dir = Vector(pos2_post.x - pos_post.x, pos2_post.y - pos_post.y, 0.0f);
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

			if (CheckInersection(pos, pos2,
								mx, my, tr, false,
								view, view_proj))
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
	if (!enabled) return;

	selAxis = -1;

	if (MouseProcess(0, mx, my)) selAxis = 0;
	if (MouseProcess(1, mx, my)) selAxis = 1;
	if (MouseProcess(2, mx, my)) selAxis = 2;
}

void Gizmo::MouseMove(float mx, float my)
{
	if (selAxis == -1 || !enabled)
	{
		return;
	}

	Vector cur_dir(mx, my, 0);
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
	if (mousedPressed)
	{
		MouseMove(mx - prev_mx, my - prev_my);
		prev_mx = mx;
		prev_my = my;
	}
	else
	{
		MouseProcess(mx, my);
	}
}

void Gizmo::OnLeftMouseDown(float mx, float my)
{
	if (selAxis != -1)
	{
		prev_mx = mx;
		prev_my = my;
		mousedPressed = true;
	}
}

void Gizmo::OnLeftMouseUp()
{
	mousedPressed = false;
}