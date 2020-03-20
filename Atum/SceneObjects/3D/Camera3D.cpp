
#include "Camera3D.h"

CLASSREG(SceneObject, Camera3D, "Camera3D")

META_DATA_DESC(Camera3D)
	BASE_SCENE_OBJ_PROP(Camera3D)
	FLOAT_PROP(Camera3D, fov, 45.0f, "Prop", "fov", "field of view")
META_DATA_DESC_END()

Camera3D::Camera3D() : SceneObject()
{
}

Camera3D::~Camera3D()
{
}

void Camera3D::BindClassToScript()
{
	const char* brief = "Representation of camera in 3D space\n"
	"\n"
	"Currently 3D space is a special mode and this class represents a camera in that special 2D mode.\n"
	"Camera auto follow a target. Position of a target should be set via script.\n"
	"\n"
	"This class ::Camera3D is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(Camera3D, brief)
	core.scripts.RegisterObjectMethod(script_class_name, "void SetPosTarget(Vector&in pos, Vector&in target)", WRAP_MFN(Camera3D, SetPosTarget), "Reseting position of camera to intinal position on scene start");
}

void Camera3D::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::Camera, this, (Object::Delegate)&Camera3D::Update);
}

bool Camera3D::Play()
{
	cur_proj_fov = fov;
	projection.BuildProjection(fov * Math::Radian, (float)core.render.GetDevice()->GetHeight() / (float)core.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);

	view = transform;
	view.Inverse();

	return true;
}

void Camera3D::Update(float dt)
{
	if (GetScene()->Playing())
	{
		if (GetState() == Active)
		{
			if (fabs(cur_proj_fov - fov) > 0.01f)
			{
				cur_proj_fov = fov;
				projection.BuildProjection(fov * Math::Radian, (float)core.render.GetDevice()->GetHeight() / (float)core.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
			}

			core.render.SetTransform(Render::View, view);
			core.render.SetTransform(Render::Projection, projection);
		}
	}
	else
	{
		float dist = 7.0f;
		float width = tanf(fov * Math::Radian * 0.5f) * dist;

		Vector edges[] = { Vector(0.0f), Vector(-width , width, dist), Vector(width, width, dist), Vector(width,-width, dist), Vector(-width,-width, dist)};

		for (int i = 0; i < 5; i++)
		{
			edges[i] = edges[i] * transform;
		}

		for (int i = 1; i < 5; i++)
		{
			core.render.DebugLine(edges[0], COLOR_GREEN, edges[i], COLOR_GREEN);
			core.render.DebugLine(edges[i], COLOR_GREEN, edges[i == 4 ? 1 : i + 1], COLOR_GREEN);
		}
	}
}

Matrix* Camera3D::Trans()
{
	return &transform;
}

bool Camera3D::Is3DObject()
{
	return true;
}

void Camera3D::SetPosTarget(Vector& pos, Vector& target)
{
	view.BuildView(pos, target, Vector(0, 1, 0));
}