
#include "ShadowCaster2D.h"
#include "SpriteInst.h"

CLASSREG(SceneObject, ShadowCaster2D, "ShadowCaster2D")

META_DATA_DESC(ShadowCaster2D)
BASE_SCENE_OBJ_PROP(ShadowCaster2D)
STRING_ENUM_PROP(ShadowCaster2D, caster_group, FillGroupsList, "Prop", "CasterGroup")
FLOAT_PROP(ShadowCaster2D, pos.x, 0.0f, "Prop", "x", "X coordinate of a source of a shadow position")
FLOAT_PROP(ShadowCaster2D, pos.y, 0.0f, "Prop", "y", "Y coordinate of a source of a shadow position")
FLOAT_PROP(ShadowCaster2D, radius, 100.0f, "Prop", "radius", "Radius of a shadow")
FLOAT_PROP(ShadowCaster2D, alpha, 0.75f, "Prop", "alpha", "Transparency of a shadow")
FLOAT_PROP(ShadowCaster2D, depth, 0.5f, "Prop", "Depth", "Z value of a shadow")
META_DATA_DESC_END()

void ShadowCaster2D::Init()
{
	RenderTasks(false)->AddTask(ExecuteLevels::Sprites + 2, this, (Object::Delegate)&ShadowCaster2D::Draw);
}

void ShadowCaster2D::BindClassToScript()
{
	const char* brief = "Representation of omni shadow in 2D space\n"
		"\n"
		"Currently 2D space is a special mode and this class represents a omni shadow casted from a particular point in a that\n"
		"special space. 2D scene objects from particular scene group are playing role of shadow casters.Point of a shadow caster\n"
		"set via script.\n"
		"\n"
		"This class ::ShadowCaster2D is a representation on C++ side.\n";

	BIND_TYPE_TO_SCRIPT(ShadowCaster2D, brief)
	core.scripts.RegisterObjectProperty(script_class_name, "float pos_x", memberOFFSET(ShadowCaster2D, pos.x), "X coordinate of a source of a shadow position");
	core.scripts.RegisterObjectProperty(script_class_name, "float pos_y", memberOFFSET(ShadowCaster2D, pos.y), "Y coordinate of a source of a shadow position");
}

void ShadowCaster2D::Draw(float dt)
{
	if (state != State::Active)
	{
		return;
	}

	vector<Scene::Group*> out_groups;
	GetOwner()->GetGroup(out_groups, caster_group.c_str());

	for (auto group : out_groups)
	{
		float scale = core.render.GetDevice()->GetHeight() / 1024.0f;
		Vector2 cam_pos(Sprite::cam_pos.x * scale - core.render.GetDevice()->GetWidth() * 0.5f, Sprite::cam_pos.y * scale - 512 * scale);

		Vector2 center(pos.x * scale - cam_pos.x, pos.y * scale - cam_pos.y);
		Color color = COLOR_BLACK_A(alpha);

		for (auto& entry : group->objects)
		{
			SpriteInst* sprite_inst = dynamic_cast<SpriteInst*>(entry);

			if (sprite_inst)
			{
				Vector2 size = sprite_inst->trans.size * scale;
				Vector2 offset = sprite_inst->trans.offset * size;

				for (auto& inst : sprite_inst->instances)
				{
					auto pos = inst.GetPos();

					Vector2 points[] = { { pos.x * scale - cam_pos.x - offset.x, pos.y * scale - cam_pos.y - offset.y },
					                     { pos.x * scale - cam_pos.x - offset.x + size.x, pos.y * scale - cam_pos.y - offset.y },
					                     { pos.x * scale - cam_pos.x - offset.x + size.x, pos.y * scale - cam_pos.y - offset.y + size.y },
					                     { pos.x * scale - cam_pos.x - offset.x, pos.y * scale - cam_pos.y - offset.y + size.y }};

					for (int index = 0; index < 4; index++)
					{
						Vector2 p1 = points[index];
						Vector2 p2 = points[(index + 1) % 4];

						Vector2 dir = p2 - p1;
						dir.Normalize();

						Vector2 dir_to = p1 - center;
						dir_to.Normalize();

						Vector2 line_norm(dir.y, -dir.x);

						if (dir_to.Dot(line_norm) < 0.0f)
						{
							core.render.DebugTriangle2D(p1, p2, p1 + dir_to * 900.0f * scale, color);

							Vector2 dir_to2 = p2 - center;
							dir_to2.Normalize();

							core.render.DebugTriangle2D(p2, p2 + dir_to2 * 900.0f * scale, p1 + dir_to * 900.0f * scale, color);
						}
					}
				}
			}
		}
	}
}