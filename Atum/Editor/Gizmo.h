
#pragma once

#include "Support/Support.h"

class Gizmo
{
public:
	bool   bViewPortResized;
	Matrix transform;
	int    mode;
	int    selAxis;
	bool   useLocalSpace;
	Vector ms_dir;
	bool   enabled;
	float  scale;
	bool   mousedPressed;
	float  prev_mx, prev_my;

	Vector2 origin;
	Vector2 ancorns[8];

	Matrix center_transform;
	Matrix rot_matrix;

	Vector2 prev_ms;

	Gizmo();
	Color CheckColor(int axis);
	void DrawAxis(int axis);
	void DrawCircle(int axis);
	bool CheckInersection(Vector pos, Vector pos2,
							float mx, float my,
							Vector trans, bool check_trans,
							Matrix view, Matrix view_proj);

	bool MouseProcess(int axis, float mx, float my);
	void MouseProcess(float mx, float my);
	void MouseMove(float mx, float my);

	void OnMouseMove(float mx, float my);
	void OnLeftMouseDown(float mx, float my);
	void OnLeftMouseUp();

	void Render();
};