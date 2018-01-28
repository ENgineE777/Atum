
#include "TransformDataTrack.h"
#include "TransformDataTrackWidgets.h"

#ifdef EDITOR
#include "Services/Render/Render.h"
#endif

TransformDataTrack::TransformDataTrack(TrackPlayer* own, const char* nm, Matrix* set_value, float rad, bool orinet,
                                       ChangeKeyFunc changeKey_Func) : TemplDataTrack<TransformKey>(own, nm, changeKey_Func)
{
	radius = rad;
	orient_by_spline = orinet;
	curve_count = 0;
	value = set_value;

#ifdef EDITOR
	wigets = NULL;
#endif
}

TransformDataTrack::~TransformDataTrack()
{
	keys.clear();
	values.clear();

	if (curve_count)
	{
		delete[] curve;
		curve = 0;
		curve_count = 0;
	}
}

void TransformDataTrack::Load(JSONReader* stream, int num)
{
	keys.resize(num);
	values.resize(num);

	for (int i=0; i < num; i++)
	{
		if (!stream->EnterBlock("TransformData")) break;

		stream->Read("pos", values[i].pos);
		stream->Read("rot", values[i].rot);

		stream->Read("time", keys[i].time);

		keys[i].blend = BlendLinear;
		stream->Read("blend",(int&)keys[i].blend);

		stream->LeaveBlock();
	}

	Prepare();
}

void TransformDataTrack::Save(JSONWriter* stream)
{
	stream->StartArray("TransformData");

	for (int i=0; i < keys.size(); i++)
	{
		stream->StartBlock(nullptr);

		stream->Write("pos", values[i].pos);
		stream->Write("rot", values[i].rot);

		stream->Write("time", keys[i].time);
		stream->Write("blend", (int&)keys[i].blend);
		
		stream->FinishBlock();
	}

	stream->FinishArray();
}

void TransformDataTrack::BezierFunc(Vector& p1, Vector& p2,Vector& p3,Vector& p4, Vector& pos, float t)
{
	float s=1-t;
	float t2=t*t;
	float t3=t2*t;

	pos = ((p1*s+3*t*p2)*s+3*t2*p3)*s+t3*p4;
}

void TransformDataTrack::CubicFunc(int index, Vector& pos, float s)
{
	pos = ((values[index].d * s + values[index].c) * s + values[index].b) * s + values[index].a;
}

void TransformDataTrack::CubicFuncAxeX(int index, Vector& pos, float s)
{
	pos = ((values[index].dX * s + values[index].cX) * s + values[index].bX) * s + values[index].aX;
}

void TransformDataTrack::CubicFuncQuat(int index, Quaternion& quat, float s)
{
	quat.x = ((values[index].dQ.x * s + values[index].cQ.x) * s + values[index].bQ.x) * s + values[index].aQ.x;
	quat.y = ((values[index].dQ.y * s + values[index].cQ.y) * s + values[index].bQ.y) * s + values[index].aQ.y;
	quat.z = ((values[index].dQ.z * s + values[index].cQ.z) * s + values[index].bQ.z) * s + values[index].aQ.z;
	quat.w = ((values[index].dQ.w * s + values[index].cQ.w) * s + values[index].bQ.w) * s + values[index].aQ.w;
}

void TransformDataTrack::GenerateKoef(int start_index, int end_index)
{
	int n = end_index - start_index;

	if (n < 1) return;

	Vector* gamma = new Vector[n + 1];
	Vector* delta = new Vector[n + 1];
	Vector* D = new Vector[n + 1];

	int i;

	gamma[0] = 1.0f / 2.0f;

	for (i = 1; i < n; i++)
	{
		gamma[i] = 1.0f / (4.0f - gamma[i - 1]);
	}
	
	gamma[n] = 1.0f / (2.0f - gamma[n - 1]);

	delta[0] = 3.0f * (values[start_index+1].dir - values[start_index].dir) * gamma[0];

	for (i = 1; i < n; i++)
	{
		delta[i] = (3.0f * (values[start_index + i + 1].dir - values[start_index + i - 1].dir) - delta[i - 1]) * gamma[i];
	}
	
	delta[n] = (3.0f * (values[start_index + n].dir - values[start_index + n - 1].dir) - delta[n - 1]) * gamma[n];

	D[n] = delta[n];

	for (i = n - 1; i >= 0; i--)
	{
		D[i] = delta[i] - gamma[i] * D[i + 1];
	}

	for (i = 0; i < n; i++)
	{
		values[start_index + i].a = values[start_index + i].dir;
		values[start_index + i].b = D[i];
		values[start_index + i].c = 3 * (values[start_index + i + 1].dir - values[start_index + i].dir) - 2 * D[i] - D[i + 1];
		values[start_index + i].d = 2 * (values[start_index + i].dir - values[start_index + i + 1].dir) + D[i] + D[i + 1];
	}

	delete[] gamma;
	delete[] delta;
	delete[] D;
}

void TransformDataTrack::GenerateKoefAxeX(int start_index, int end_index)
{
	int n = end_index - start_index;

	if (n < 1) return;

	Vector* gamma = new Vector[n + 1];
	Vector* delta = new Vector[n + 1];
	Vector* D = new Vector[n + 1];

	int i;

	gamma[0] = 1.0f / 2.0f;

	for (i = 1; i < n; i++)
	{
		gamma[i] = 1.0f / (4.0f - gamma[i - 1]);
	}

	gamma[n] = 1.0f / (2.0f - gamma[n - 1]);

	delta[0] = 3.0f * (values[start_index+1].dirAxeX - values[start_index].dirAxeX) * gamma[0];

	for (i = 1; i < n; i++)
	{
		delta[i] = (3.0f * (values[start_index + i + 1].dirAxeX - values[start_index + i - 1].dirAxeX) - delta[i - 1]) * gamma[i];
	}

	delta[n] = (3.0f * (values[start_index + n].dirAxeX - values[start_index + n - 1].dirAxeX) - delta[n - 1]) * gamma[n];

	D[n] = delta[n];

	for (i = n - 1; i >= 0; i--)
	{
		D[i] = delta[i] - gamma[i] * D[i + 1];
	}

	for (i = 0; i < n; i++)
	{
		values[start_index + i].aX = values[start_index + i].dirAxeX;
		values[start_index + i].bX = D[i];
		values[start_index + i].cX = 3 * (values[start_index + i + 1].dirAxeX - values[start_index + i].dirAxeX) - 2 * D[i] - D[i + 1];
		values[start_index + i].dX = 2 * (values[start_index + i].dirAxeX - values[start_index + i + 1].dirAxeX) + D[i] + D[i + 1];
	}

	delete[] gamma;
	delete[] delta;
	delete[] D;
}

void TransformDataTrack::GenerateKoefQuat(int start_index, int end_index)
{
	int n = end_index - start_index;

	if (n<1) return;

	Quaternion* gamma = new Quaternion[n + 1];
	Quaternion* delta = new Quaternion[n + 1];
	Quaternion* D = new Quaternion[n + 1];

	int i;

	gamma[0] = 1.0f / 2.0f;
	
	for (i = 1; i < n; i++)
	{
		gamma[i] = 1.0f / (4.0f - gamma[i - 1]);
	}

	gamma[n] = 1.0f / (2.0f - gamma[n - 1]);

	delta[0] = 3.0f * (values[start_index+1].rot - values[start_index].rot) * gamma[0];

	for (i = 1; i < n; i++)
	{
		delta[i] = (3.0f * (values[start_index + i + 1].rot - values[start_index + i - 1].rot) - delta[i - 1]) * gamma[i];
	}

	delta[n] = (3.0f * (values[start_index + n].rot - values[start_index + n - 1].rot) - delta[n - 1]) * gamma[n];

	D[n] = delta[n];

	for (i = n - 1; i >= 0; i--)
	{
		D[i] = delta[i] - gamma[i] * D[i + 1];
	}

	for (i = 0; i < n; i++)
	{
		values[start_index + i].aQ = values[start_index + i].rot;
		values[start_index + i].bQ = D[i];
		values[start_index + i].cQ = 3 * (values[start_index + i + 1].rot - values[start_index + i].rot) - 2 * D[i] - D[i + 1];
		values[start_index + i].dQ = 2 * (values[start_index + i].rot - values[start_index + i+1].rot) + D[i] + D[i + 1];
	}

	delete[] gamma;
	delete[] delta;
	delete[] D;
}

void TransformDataTrack::Prepare()
{
	for (int i = 0; i < (int)keys.size(); i++)
	{
		Matrix mat;
		values[i].rot.GetMatrix(mat);
		values[i].dir = mat.Vz();
		values[i].dirAxeX = mat.Vx();
		values[i].b_pt1 = values[i].b_pt2 = values[i].pos;
	}

	int start_index = 0;
	int index = 0;

	while (index<keys.size())
	{
		if (!keys[index].blend || index == keys.size() - 1)
		{
			GenerateKoef(start_index, index);
			GenerateKoefAxeX(start_index, index);
			GenerateKoefQuat(start_index, index);

			if (index - start_index == 1)
			{
				Vector dir = values[1].pos - values[0].pos;
				float dst = dir.Normalize();

				values[0].b_pt1 = values[0].pos + dir * dst * 0.33f;
				values[0].b_pt2 = values[0].pos + dir * dst * 0.66f;

				dir = values[1].dir - values[0].dir;
				dst = dir.Normalize();
			}
			else
			{
				for (int i = start_index; i < index - 1; i++)
				{
					Vector dir = values[i + 1].pos - values[i].pos;
					float dst = dir.Normalize();

					if (i < (int)values.size() - 2)
					{
						Vector tngt = values[i + 2].pos - values[i].pos;
						tngt.Normalize();
				
						values[i].b_pt2 = values[i + 1].pos - tngt * dst * 0.25f;

						dir = values[i + 2].pos - values[i + 1].pos;
						dst = dir.Normalize();
			
						values[i + 1].b_pt1 = values[i + 1].pos + tngt * dst * 0.25f;

						if (i == 0)
						{
							dir = values[i].b_pt2 - values[i].pos;
							dst = dir.Normalize();
					
							values[i].b_pt1 = values[i].pos + dir * dst * 0.5f;
						}

						if (i == index - 2)
						{
							dir = values[i + 2].pos - values[i + 1].b_pt1;
							dst = dir.Normalize();
					
							values[i + 1].b_pt2 = values[i + 1].b_pt1 + dir * dst * 0.5f;
						}
					}
				}
			}

			start_index = index + 1;
		}

		index++;
	}

	if (keys.size() > 1)
	{
		curve_count = ((int)keys.size() - 1) * 20 + 1;
		curve = new Vector[curve_count];
		curve[0] = values[0].pos;

		for (int j = 0; j < (int)keys.size() - 1; j++)
		{
			values[j].length = 0;

			Vector prev_pt = values[j].pos;

			for (int i = 0; i < 20; i++)
			{
				float k = (float)(i+1) / 20.0f;

				BezierFunc(values[j].pos, values[j].b_pt1, values[j].b_pt2, values[j + 1].pos, curve[j * 20 + i + 1], k);

				prev_pt = curve[j * 20 + i + 1] - prev_pt;

				values[j].length += prev_pt.Length();
				values[j].dst[i] = values[j].length;

				prev_pt = curve[j * 20 + i + 1];
			}

			values[j].speed = values[j].length / (keys[j+1].time - keys[j].time);

			if (j==0)
			{
				values[j].prev_speed = values[j].speed;
			}
			else
			{
				values[j].prev_speed = values[j-1].speed;
				if (values[j].prev_speed < 0.001f) values[j].prev_speed = values[j].speed;
			}
		}
	}
	else
	{
		curve = NULL;
	}
}

void TransformDataTrack::CalcValue(int index, float blend)
{
	Quaternion rot;
	Vector pos;

	if (blend<-0.01f)
	{
		pos = values[index].pos;
		rot = values[index].rot;
	}
	else
	{
		if ((values[index - 1].dir.x * values[index].dir.x +
			values[index - 1].dir.y * values[index].dir.y +
			values[index - 1].dir.z * values[index].dir.z) > 0.9f)
		{
			rot.SLerp(values[index-1].rot, values[index].rot, blend);
		}
		else
		{
			Vector s_dir;
			CubicFunc(index - 1, s_dir, blend);

			Vector dirX;
			CubicFuncAxeX(index - 1, dirX, blend);

			Vector up;
			up = s_dir.Cross(dirX);

			Matrix view;
			view.BuildView(Vector(0.0f), s_dir, up);
			view.Inverse();
			rot.Set(view);
		}

		float dst = 0.0f;

		int num = 50;
		static float lens[50 + 1];

		float len = 0.0f;
		lens[0] = 0.0f;

		float weight = fabs(values[index-1].speed - values[index - 1].prev_speed);
		weight -= 3.0;
		if (weight<0.0f) weight = 0.0f;
		if (weight>10.0f) weight = 10.0f;
		weight = weight / 10.0f;

		for (int k=0; k<num; k++)
		{
			float blnd = (float)(k+1)/(float)(num);

			float cur_v = fabs(values[index - 1].prev_speed + (values[index-1].speed - values[index - 1].prev_speed) * blnd);

			len += cur_v * 1.0f / (float)(num);
			lens[k+1] = len;
		}

		float tr = values[index - 1].length / len;

		for (int k = 0; k <= num; k++)
		{
			lens[k] *= tr;
		}

		int t = (int)(blend * (float)(num));
		float kb = (blend - t * 1.0f / (float)(num)) / (1.0f / (float)(num));

		dst = lens[t] + (lens[t+1] - lens[t]) * kb;

		for (int sub_index = 0; sub_index < 20; sub_index++)
		{
			float k = values[index - 1].dst[sub_index];

			if (dst < k)
			{
				float k1 = 0;

				if (sub_index>0)
				{
					k1 = values[index - 1].dst[sub_index - 1];
				}

				blend = (dst - k1) / (k - k1);

				pos.Lerp(curve[(index - 1) * 20 + sub_index],
				         curve[(index - 1) * 20 + sub_index + 1], blend);

				if (orient_by_spline)
				{
					Matrix view;
					view.Vz() = curve[(index - 1) * 20 + sub_index] - curve[(index - 1) * 20 + sub_index + 1];
					view.Vz().Normalize();
					view.Vx() = view.Vz();
					//view.Vx().Rotate_PI2_CW();
					view.Vy() = view.Vx().Cross(view.Vz());
					view.Vy().y = fabs(view.Vy().y);

					rot.Set(view);
				}

				break;
			}
		}
	}

	rot.GetMatrix(*value);
	value->Pos() = pos;
}

#ifdef EDITOR
void TransformDataTrack::SetDefaultKey(int index)
{
	GrabCamera(index);
}

int TransformDataTrack::AddKey(float time)
{
	int ret = TemplDataTrack::AddKey(time);
	Prepare();
	return ret;
}

void TransformDataTrack::DelKey(int index)
{
	TemplDataTrack::DelKey(index);
	Prepare();
}

void TransformDataTrack::InitControls(EUIWidget* parent)
{
	wigets = new TransformDataTrackWidgets(this); 
	wigets->InitControls(parent);
}

void TransformDataTrack::StartEditKey(int key)
{
	edited_key = key;

	if (key !=-1)
	{
		wigets->blend = &keys[key].blend;
		wigets->StartEditKey(&values[key]);
	}

	DataToGizmo();
}

void TransformDataTrack::DataToGizmo()
{
	if (edited_key != -1)
	{
		Matrix tr;
		values[edited_key].rot.GetMatrix(tr);
		tr.Pos() = values[edited_key].pos;
	
		Gizmo::inst->enabled = true;
		Gizmo::inst->transform = tr;
	}
}

void TransformDataTrack::DataFromGizmo()
{
	if (edited_key != -1)
	{
		if ((values[edited_key].pos - Gizmo::inst->transform.Pos()).Length()>0.0001f)
		{
			wigets->StartEditKey(&values[edited_key]);
		}

		values[edited_key].rot.Set(Gizmo::inst->transform);
		values[edited_key].pos = Gizmo::inst->transform.Pos();

		Prepare();
	}
}

void TransformDataTrack::StopEditKey()
{
	edited_key = -1;
	Gizmo::inst->enabled = false;

	if (wigets)	wigets->StopEditKey();
}

void TransformDataTrack::DrawKey(Matrix& view_proj, Color& color, TransformKey& key)
{
	Matrix transform;
	key.rot.GetMatrix(transform);
	transform.Pos() = key.pos;

	float z = key.pos.x*view_proj._13 + key.pos.y*view_proj._23 + key.pos.z*view_proj._33 + view_proj._43;

	float scale = 0.015f * (1.0f + z);
	scale = fabs(scale);

	scale = 0.095f * (1.0f + z);

	Vector pos = key.pos;
	Vector pos_up = pos + transform.Vy() * scale * 0.3f;
	Vector dir = transform.Vz() * scale + pos;

	Vector dirx = transform.Vx() * scale * 0.3f;
	Vector dirxInvert = -dirx;
	dirx += pos;
	dirxInvert += pos;

	render.DebugTriangle(pos_up, dirxInvert, dir, color);
	render.DebugTriangle(pos_up, dir, dirx, color);
	render.DebugTriangle(dir, dirxInvert, dirx, color);
	render.DebugTriangle(pos_up, dirx, dirxInvert, color);
}

void TransformDataTrack::DebugDraw()
{
	if (curve)
	{
		for (int i = 0; i < curve_count - 1; i++)
		{
			int index = (int)((float)i / 20.f);

			if (!keys[index].blend) continue;

			render.DebugLine(curve[ i ], COLOR_GREEN, curve[i+1], COLOR_GREEN);
		}
	}

	if (!keys.size())
	{
		return;
	}

	Matrix view;
	render.GetTransform(Render::View, view);

	Matrix view_proj;
	render.GetTransform(Render::Projection, view_proj);
	view_proj = view * view_proj;

	bool blend = true;
	for (int i = 0; i < keys.size(); i++)
	{
		Color color = i == edited_key ? COLOR_MAGNETA : COLOR_YELLOW;

		DrawKey(view_proj, color, values[i]);

		if (!blend) 
		{
			blend = (keys[i].blend != BlendNone);
			continue;
		}

		blend = (keys[i].blend != BlendNone);
		
		if (i == 0) continue;

		/*if (DebugDraw)
		{
			for (int j = 0; j < 20; j++)
			{
				float k = (float)j / 20;
				TransformTrackPoint temp = cur_val;
				CalcValue(i, k);
				Vector axeZ(0.f, 0.f, 1.f);
				cur_val.rot.Rotate(axeZ);
				axeZ += cur_val.pos;

				render.DebugLine(cur_val.pos, COLOR_WHITE, axeZ, COLOR_WHITE);
				cur_val = temp;
			}
		}*/
	}
}

void TransformDataTrack::GrabCamera(int index)
{
	Matrix view;
	render.GetTransform(Render::View, view);
	view.Inverse();

	values[index].pos = view.Pos();
	values[index].rot.Set(view);

	values[index].pos += view.Vz() * radius;
}

#endif

