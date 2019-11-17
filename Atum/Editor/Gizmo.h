
#pragma once

#include "Support/Support.h"
#include "EUI.h"

/**
\ingroup gr_code_editor
*/

class Gizmo
{
public:

	enum Trans2D
	{
		trans_2d_move    = 1,
		trans_2d_scale   = 2,
		trans_2d_rotate  = 4,
		trans_2d_anchorn = 8
	};

	static Gizmo* inst;

	int     mode = 1;
	bool    useLocalSpace = false;
	float   scale = 1.0f;
	Vector2 align2d = 0.0f;

	struct Transform2D
	{
		Vector2* pos = nullptr;
		Vector2* axis = nullptr;
		float*   depth = nullptr;
		Vector2* size = nullptr;
		float*   rotation = nullptr;
		Vector2* offset = nullptr;
		Matrix   mat_local;
		Matrix   mat_parent;
		Matrix   mat_global;

		Transform2D() = default;
		Transform2D(Vector2* set_pos, Vector2* set_size, Vector2* set_offset = nullptr, float* set_depth = nullptr, float* set_rotation = nullptr, Vector2* set_axis = nullptr, Matrix mat_parent = Matrix());
		Transform2D(::Transform2D& trans);
		void BuildMatrices();
	};

	Transform2D trans2D;

private:

	Matrix  transform;
	bool    mousedPressed = false;

	bool use_trans2D = false;
	int     trans2D_actions = 0;
	bool    ignore_2d_camera = true;
	Vector2 pos2d = 0.0f;
	Vector2 size2d = 0.0f;
	Vector2 origin;
	Vector2 moved_origin;
	Vector2 ancorns[8];

	bool    enabled = false;

	int     selAxis = -1;
	Vector2 ms_dir = 0.0f;
	Vector2 prev_ms = 0.0f;

	Color CheckColor(int axis);
	void DrawAxis(int axis);
	void DrawCircle(int axis);
	bool CheckInersection(Vector pos, Vector pos2, Vector2 ms,
	                      Vector trans, bool check_trans,
	                      Matrix view, Matrix view_proj);

	bool IsInsideTriangle(Vector2 s, Vector2 a, Vector2 b, Vector2 c);

	void CheckSelectionTrans2D(Vector2 ms);
	bool CheckSelectionTrans3D(int axis, Vector2 ms);
	void CheckSelectionTrans3D(Vector2 ms);

	void MoveTrans2D(Vector2 ms);
	void MoveTrans3D(Vector2 ms);

	void RenderTrans2D();
	void RenderTrans3D();

public:

	Vector2 delta_move = 0.0;

	void Init();

	void SetTrans2D(Transform2D trans, int actions = 0xffff, bool ignore_2d_camera = false);
	void SetTrans3D(Matrix transform);
	Matrix& GetTrans3D();
	bool IsTrans2D();
	bool IsEnabled();
	void Disable();

	void OnMouseMove(Vector2 ms);
	void OnLeftMouseDown();
	void OnLeftMouseUp();

	Vector2 MakeAligned(Vector2 pos);
	void Render();
};