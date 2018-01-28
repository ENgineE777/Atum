
#pragma once

#include "Support/Support.h"

class Gizmo
{
public:
	static Gizmo* inst;

	Transform2D* trans2D = nullptr;

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

	class Texture* anchorn;
	class Texture* center;

	Vector2 origin;
	Vector2 ancorns[8];

	Matrix center_transform;
	Matrix rot_matrix;

	Vector2 prev_ms;

	Gizmo();
	void Init();
	Color CheckColor(int axis);
	void DrawAxis(int axis);
	void DrawCircle(int axis);
	bool CheckInersection(Vector pos, Vector pos2,
	                      float mx, float my,
	                      Vector trans, bool check_trans,
	                      Matrix view, Matrix view_proj);

	bool IsInsideTriangle(Vector2 s, Vector2 a, Vector2 b, Vector2 c);

	bool MouseProcess(int axis, float mx, float my);
	void MouseProcess(float mx, float my);
	void MouseMove(float mx, float my);

	void OnMouseMove(float mx, float my);
	void OnLeftMouseDown(float mx, float my);
	void OnLeftMouseUp();

	void Render();
};