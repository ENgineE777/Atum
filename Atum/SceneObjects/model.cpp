
#include <windows.h>
#include "model.h"
#include "programs.h"

void Model::Drawer::Init(Model* model, TaskExecutor::SingleTaskPool* taskPool)
{
	res = model;

	taskPool->AddTask(0, this, (Object::Delegate)&Model::Drawer::Render);
	//render.AddDelegate("toshadow", this, (Object::Delegate)&Model::Drawer::Render, 0);
	//render.AddDelegate("geometry", this, (Object::Delegate)&Model::Drawer::Render, 0);
	//render.AddDelegate("shgeometry", this, (Object::Delegate)&Model::Drawer::ShRender, 0);
}

void Model::Drawer::SetColor(Vector4& set_color)
{
	color = set_color;
}

void Model::Drawer::SetPosition(Vector2 pos)
{
	Vector centerPos = (res->bb_max + res->bb_min) * 0.5f;

	world.Identity();
	world.Pos() = -centerPos;
	world.Pos().x += pos.x;
	world.Pos().y = res->bb_min.y + 0.0f;
	world.Pos().z += pos.y;
}

void Model::Drawer::SetTransform(Matrix& mat)
{
	world = mat;
}

void Model::Drawer::Show(bool set)
{
	show = set;
}

void Model::Drawer::Render(float dt)
{
	Render(Programs::prg);
}

void Model::Drawer::ShRender(float dt)
{
	Render(Programs::shprg);
}

void Model::Drawer::Render(Program* prg)
{
	if (!show)
	{
		return;
	}

	Matrix trans;
	render.GetTransform(Render::WrldViewProj, trans);

	prg->Apply();

	prg->VS_SetMatrix("trans", &world, 1);
	prg->VS_SetMatrix("view_proj", &trans, 1);
	prg->PS_SetVector("color", &color, 1);

	for (int i = 0; i < res->meshes.size(); i++)
	{
		Mesh &mesh = res->meshes[i];
		
		render.GetDevice()->SetVertexBuffer(0, mesh.buffer);

		prg->PS_SetTexture("diffuseMap", res->textures[mesh.texture]);
		render.GetDevice()->Draw(Device::TrianglesList, 0, mesh.num_triangles);
	}
}

void Model::LoadModelMS3D(const char* filename)
{
	// File header
	struct MS3DHeader
	{
		char ID[10];
		int  version;
	};

	// Vertex information
	struct MS3DVertex
	{
		unsigned char  flags;
		float vertex[3];
		char  boneID;
		unsigned char  refCount;
	};

	struct MS3DTriangle
	{
		unsigned short flags;
		unsigned short vertexIndices[3];
		float vertexNormals[3][3];
		float s[3], t[3];
		unsigned char  smoothingGroup;
		unsigned char  groupIndex;
	};

	struct MS3DGroup
	{
		unsigned char flags;
		char name[32];
		std::vector<unsigned short> triangleIndices;
		char materialIndex;
		std::vector<char> comment;
	};

	struct MS3DMaterial
	{
		char  name[32];
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float emissive[4];
		float shininess;
		float transparency;
		unsigned char  mode;
		char  texture[128];
		char  alphamap[128];
	};

	FILE* file = fopen(filename, "rb");

	if (!file)
	{
		return;
	}

	char id[10];
	fread(id, sizeof(char), 10, file);
	if (strncmp(id, "MS3D000000", 10) != 0)
	{
		fclose(file);
		return;
	}

	int version;
	fread(&version, sizeof(int), 1, file);
	if (version != 4)
	{
		fclose(file);
		return;
	}

	// vertices
	unsigned short numVertices;
	fread(&numVertices, sizeof(unsigned short), 1, file);

	std::vector<MS3DVertex> vertices;

	vertices.resize(numVertices);
	for (int i = 0; i < numVertices; i++)
	{
		fread(&vertices[i].flags, sizeof(unsigned char), 1, file);
		fread(&vertices[i].vertex, sizeof(float), 3, file);
		fread(&vertices[i].boneID, sizeof(char), 1, file);
		fread(&vertices[i].refCount, sizeof(unsigned char), 1, file);
	}

	// triangles
	unsigned short numTriangles;
	fread(&numTriangles, sizeof(unsigned short), 1, file);

	std::vector<MS3DTriangle> triangles;

	triangles.resize(numTriangles);

	for (int i = 0; i < numTriangles; i++)
	{
		fread(&triangles[i].flags, sizeof(unsigned short), 1, file);
		fread(triangles[i].vertexIndices, sizeof(unsigned short), 3, file);
		fread(triangles[i].vertexNormals, sizeof(float), 3 * 3, file);
		fread(triangles[i].s, sizeof(float), 3, file);
		fread(triangles[i].t, sizeof(float), 3, file);
		fread(&triangles[i].smoothingGroup, sizeof(unsigned char), 1, file);
		fread(&triangles[i].groupIndex, sizeof(unsigned char), 1, file);
	}

	unsigned short numGroups;
	fread(&numGroups, sizeof(unsigned short), 1, file);

	std::vector<MS3DGroup> groups;
	groups.resize(numGroups);

	bb_max = Vector(-100000.0f);
	bb_min = Vector(100000.0f);

	for (int i = 0; i < numGroups; i++)
	{
		fread(&groups[i].flags, sizeof(unsigned char), 1, file);
		fread(groups[i].name, sizeof(char), 32, file);

		unsigned short numGroupTriangles;
		fread(&numGroupTriangles, sizeof(unsigned short), 1, file);
		groups[i].triangleIndices.resize(numGroupTriangles);

		if (numGroupTriangles > 0)
			fread(&groups[i].triangleIndices[0], sizeof(unsigned short), numGroupTriangles, file);

		fread(&groups[i].materialIndex, sizeof(char), 1, file);

		if (groups[i].name[0] == 'l' && groups[i].name[1] == 'o' &&
			groups[i].name[2] == 'c' && groups[i].name[3] == '_')
		{
			Vector bb_max = Vector(-100000.0f);
			Vector bb_min = Vector(100000.0f);

			for (int t = 0; t < numGroupTriangles; t++)
			{
				for (int j = 0; j<3; j++)
				{
					int tri = groups[i].triangleIndices[t];
					int index = triangles[tri].vertexIndices[j];
					Vector pos;
					pos.x = vertices[index].vertex[0];
					pos.y = vertices[index].vertex[1];
					pos.z = vertices[index].vertex[2];

					bb_max.Max(pos);
					bb_min.Min(pos);
				}
			}

			locator = (bb_max + bb_min) * 0.5f;

			continue;
		}

		meshes.push_back(Mesh());
		Mesh& mesh = meshes[meshes.size() - 1];

		mesh.texture = groups[i].materialIndex;
		mesh.num_triangles = numGroupTriangles;
		mesh.buffer = render.GetDevice()->CreateBuffer(numGroupTriangles * 3, sizeof(ModelVertex));

		ModelVertex* mesh_vertices = (ModelVertex*)mesh.buffer->Lock();

		for (int t = 0; t < numGroupTriangles; t++)
		{
			for (int j = 0; j<3; j++)
			{
				int tri = groups[i].triangleIndices[t];
				int index = triangles[tri].vertexIndices[j];
				mesh_vertices[t * 3 + j].pos.x = vertices[index].vertex[0];
				mesh_vertices[t * 3 + j].pos.y = vertices[index].vertex[1];
				mesh_vertices[t * 3 + j].pos.z = vertices[index].vertex[2];

				mesh_vertices[t * 3 + j].normal.x = triangles[tri].vertexNormals[j][0];
				mesh_vertices[t * 3 + j].normal.y = triangles[tri].vertexNormals[j][1];
				mesh_vertices[t * 3 + j].normal.z = triangles[tri].vertexNormals[j][2];

				mesh_vertices[t * 3 + j].uv = Vector2(triangles[tri].s[j], triangles[tri].t[j]);

				bb_max.Max(mesh_vertices[t * 3 + j].pos);
				bb_min.Min(mesh_vertices[t * 3 + j].pos);
			}
		}

		mesh.buffer->Unlock();
	}

	// materials
	unsigned short numMaterials;
	fread(&numMaterials, sizeof(unsigned short), 1, file);

	std::vector<MS3DMaterial> materials;
	materials.resize(numMaterials);

	textures.resize(numMaterials);

	for (int i = 0; i < numMaterials; i++)
	{
		fread(materials[i].name, sizeof(char), 32, file);
		fread(&materials[i].ambient, sizeof(float), 4, file);
		fread(&materials[i].diffuse, sizeof(float), 4, file);
	    fread(&materials[i].specular, sizeof(float), 4, file);
		fread(&materials[i].emissive, sizeof(float), 4, file);
		fread(&materials[i].shininess, sizeof(float), 1, file);
		fread(&materials[i].transparency, sizeof(float), 1, file);
		fread(&materials[i].mode, sizeof(unsigned char), 1, file);
		fread(materials[i].texture, sizeof(char), 128, file);
		fread(materials[i].alphamap, sizeof(char), 128, file);

		// set alpha
		materials[i].ambient[3] = materials[i].transparency;
		materials[i].diffuse[3] = materials[i].transparency;
		materials[i].specular[3] = materials[i].transparency;
		materials[i].emissive[3] = materials[i].transparency;

		char* ptr = materials[i].texture;

		char* next = strstr(ptr, "\\");

		while (next)
		{
			ptr = next;
			ptr++;

			next = strstr(ptr, "\\");

			if (!next)
			{
				next = strstr(ptr, "/");
			}
		}

		char path[128];
		sprintf(path, "Media//%s", ptr);

		textures[i] = render.LoadTexture(path);
	}
}