
#pragma once

#include "Support/Support.h"

class Gizmo
{
public:
	static Gizmo* inst;

	Transform2D* trans2D = nullptr;

	Matrix  transform;
	int     mode = 1;
	int     selAxis = -1;
	bool    useLocalSpace = false;
	Vector2 ms_dir = 0.0f;
	Vector2 prev_ms = 0.0f;
	bool    enabled = false;
	bool    allow_transform = true;
	float   scale = 1.0f;
	bool    mousedPressed = false;

	class Texture* anchorn = nullptr;
	class Texture* center = nullptr;

	Vector2 origin;
	Vector2 ancorns[8];

	Matrix center_transform;
	Matrix rot_matrix;

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