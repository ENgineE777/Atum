
#include "NavMesh2DGen.h"
#include "Services/Render/Render.h"
#include "SceneObjects/2D/SpriteInst.h"
#include "SceneObjects/2D/Phys2DComp.h"

#include "Support/libtess2/tesselator.h"

#ifdef EDITOR
void RegenNavMesh(void* owner)
{
	NavMesh2DGen* navMesh = (NavMesh2DGen*)owner;
	navMesh->ConstructNavMesh();
}
#endif

CLASSREG(SceneObject, NavMesh2DGen, "NavMesh2DGen")

META_DATA_DESC(NavMesh2DGen)
	BASE_SCENE_OBJ_PROP(NavMesh2DGen)
#ifdef EDITOR
	CALLBACK_PROP(NavMesh2DGen, RegenNavMesh, "Prop", "Regen")
#endif
	STRING_ENUM_PROP(NavMesh2DGen, obstacles_group, FillGroupsList, "Prop", "CasterGroup")
	FLOAT_PROP(NavMesh2DGen, collider_radius, 1.0f, "Prop", "collider_radius", "Radius of a colider")
META_DATA_DESC_END()

void* stdAlloc(void* userData, unsigned int size)
{
	return malloc(size);
}

void stdFree(void* userData, void* ptr)
{
	free(ptr);
}

void NavMesh2DGen::ConstructNavMesh()
{
	polygons.clear();
	vertices.clear();

	vector<Scene::Group*> out_groups;
	GetScene()->GetGroup(out_groups, "Phys2DComp");

	TESStesselator* tess = 0;
	TESSalloc ma;
	const int nvp = 3;

	memset(&ma, 0, sizeof(ma));
	ma.memalloc = stdAlloc;
	ma.memfree = stdFree;
	ma.extraVertices = 256; // realloc not provided, allow 256 extra vertices.

	Vector2 min_border = FLT_MAX;
	Vector2 max_border = FLT_MIN;

	tess = tessNewTess(&ma);

	tessSetOption(tess, TESS_CONSTRAINED_DELAUNAY_TRIANGULATION, 1);

	Vector2 rect[4];

	Vector2 dirs[] = { { 0.0, 0.0f },{ 1.0, 0.0f },{ 1.0, 1.0f },{ 0.0, 1.0f } };

	for (auto group : out_groups)
	{
		for (auto& entry : group->objects)
		{
			SpriteInst* sprite_inst = (SpriteInst*)entry;

			if (!StringUtils::IsEqual(sprite_inst->group_name.c_str(), obstacles_group.c_str()))
			{
				continue;
			}

			bool is_static = false;

			for (auto& comp : sprite_inst->components)
			{
				Phys2DCompInst* phys = dynamic_cast<Phys2DCompInst*>(comp);

				if (phys)
				{
					is_static = ((Phys2DComp*)phys->asset_comp)->body_type == Phys2DComp::BodyType::StaticBody;
					break;
				}
			}

			if (is_static)
			{
				auto offset = (sprite_inst->trans.offset * sprite_inst->trans.size + Vector2(collider_radius));
				auto size = (sprite_inst->trans.size + Vector2(collider_radius * 2.0f));

				for (auto& inst : sprite_inst->instances)
				{
					Vector2 pos = inst.GetPos() - offset;

					for (int i = 0; i < 4; i++)
					{
						rect[i] = pos + dirs[3 - i] * size;

						min_border.x = fmin(min_border.x, rect[i].x);
						min_border.y = fmin(min_border.y, rect[i].y);

						max_border.x = fmax(max_border.x, rect[i].x);
						max_border.y = fmax(max_border.y, rect[i].y);
					}

					tessAddContour(tess, 2, &rect[0].x, sizeof(float) * 2, 4);
				}
			}
		}
	}

	{
		Vector2 pos = min_border;
		Vector2 size = max_border - min_border;

		for (int i = 0; i < 4; i++)
		{
			rect[i] = pos + dirs[i] * size;
		}

		tessAddContour(tess, 2, &rect[0].x, sizeof(float) * 2, 4);
	}

	if (tessTesselate(tess, TESS_WINDING_POSITIVE, TESS_BOUNDARY_CONTOURS, 0, 0, 0))
	{
		const float* verts = tessGetVertices(tess);
		const int* elems = tessGetElements(tess);
		const int nelems = tessGetElementCount(tess);

		for (int i = 0; i < nelems; ++i)
		{
			int b = elems[i * 2];
			int n = elems[i * 2 + 1];
			tessAddContour(tess, 2, &verts[b * 2], sizeof(float) * 2, n);
		}

		if (tessTesselate(tess, TESS_WINDING_POSITIVE, TESS_CONNECTED_POLYGONS, nvp, 2, 0))
		{
			const float* verts = tessGetVertices(tess);
			const int* elems = tessGetElements(tess);
			const int nelems = tessGetElementCount(tess);

			vertices.resize(tessGetVertexCount(tess));

			for (int i = 0; i < vertices.size(); ++i)
			{
				int index = i * 2;
				vertices[i].x = verts[index];
				vertices[i].y = verts[index + 1];
			}

			polygons.resize(nelems);

			for (int i = 0; i < nelems; ++i)
			{
				auto& polygon = polygons[i];
				int index = i * nvp * 2;

				polygon.center = 0.0f;

				for (int j = 0; j < nvp && elems[index + j] != TESS_UNDEF; j++)
				{
					polygon.edges.emplace_back();
					auto&  edge = polygon.edges[polygon.edges.size() - 1];

					edge.index = elems[index + j];

					polygon.center += vertices[edge.index];

					int conn = elems[index + j + nvp];

					if (conn != -1)
					{
						edge.C = &polygons[conn];
					}
				}

				polygon.center = polygon.center / (float)polygon.edges.size();
			}
		}
	}

	for (auto& polygon : polygons)
	{
		for (int i = 0; i < polygon.edges.size(); i++)
		{
			int index = polygon.edges[i].index;
			int index_to = polygon.edges[(i + 1) % polygon.edges.size()].index;

			if (polygon.edges[i].C)
			{
				polygon.edges[i].C_edge = polygon.edges[i].C->FindEdge(index, index_to);
			}
		}
	}

	tessDeleteTess(tess);
}

void NavMesh2DGen::Draw(float dt)
{ 
	if (GetScene()->Playing() || GetState() == Invisible)
	{
		return;
	}

	float scale = core.render.GetDevice()->GetHeight() / 1024.0f;

	vector<Scene::Group*> out_groups;
	GetScene()->GetGroup(out_groups, "Phys2DComp");

	Vector2 cam_pos = Sprite::ed_cam_pos - Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;

	/*for (auto group : out_groups)
	{
		for (auto& entry : group->objects)
		{
			SpriteInst* sprite_inst = (SpriteInst*)entry;

			if (!StringUtils::IsEqual(sprite_inst->group_name.c_str(), obstacles_group.c_str()))
			{
				continue;
			}

			bool is_static = false;

			for (auto& comp : sprite_inst->components)
			{
				Phys2DCompInst* phys = dynamic_cast<Phys2DCompInst*>(comp);

				if (phys)
				{
					is_static = ((Phys2DComp*)phys->asset_comp)->body_type == Phys2DComp::BodyType::StaticBody;
					break;
				}
			}

			if (is_static)
			{
				auto offset = (sprite_inst->trans.offset * sprite_inst->trans.size + Vector2(collider_radius))* scale;
				auto size = (sprite_inst->trans.size + Vector2(collider_radius * 2.0f))* scale;

				for (auto& inst : sprite_inst->instances)
				{
					Vector2 p1 = inst.GetPos() * scale - Sprite::ed_cam_pos + Vector2((float)core.render.GetDevice()->GetWidth(), (float)core.render.GetDevice()->GetHeight()) * 0.5f;
					core.render.DebugSprite(nullptr, p1 - offset, size, COLOR_GREEN);
				}
			}
		}
	}*/

	for (auto& polygon : polygons)
	{
		for (int i = 0; i < polygon.edges.size(); i++)
		{
			int index = polygon.edges[i].index;
			int index_to = polygon.edges[(i + 1) % polygon.edges.size()].index;

			core.render.DebugLine2D(vertices[index] * scale - cam_pos, COLOR_CYAN, vertices[index_to] * scale - cam_pos, COLOR_CYAN);

			if (polygon.edges[i].C)
			{
				core.render.DebugLine2D(polygon.center * scale - cam_pos, COLOR_GREEN, polygon.edges[i].C->center * scale - cam_pos, COLOR_GREEN);
			}
		}
	}
}
