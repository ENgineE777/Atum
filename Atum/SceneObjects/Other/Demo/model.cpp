
#include "model.h"
#include "programs.h"
#include "Services/File/FileInMemory.h"
#include "Services/Scene/ExecuteLevels.h"

void Model::Drawer::Init(Model* model, TaskExecutor::SingleTaskPool* taskPool)
{
	res = model;

	taskPool->AddTask(ExecuteLevels::Geometry, this, (Object::Delegate)&Model::Drawer::Render);
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
	Render(Programs::GetTranglPrg());
}

void Model::Drawer::ShRender(float dt)
{
	Render(Programs::GetShdTranglPrg());
}

void Model::Drawer::Render(Program* prg)
{
	if (!show)
	{
		return;
	}

	Matrix trans;
	core.render.GetTransform(Render::WrldViewProj, trans);

	core.render.GetDevice()->SetProgram(prg);

	prg->SetMatrix(Shader::Type::Vertex, "trans", &world, 1);
	prg->SetMatrix(Shader::Type::Vertex, "view_proj", &trans, 1);
	prg->SetVector(Shader::Type::Pixel, "color", &color, 1);

	core.render.GetDevice()->SetVertexDecl(res->vdecl);

	for (int i = 0; i < res->meshes.size(); i++)
	{
		Mesh &mesh = res->meshes[i];
		
		core.render.GetDevice()->SetVertexBuffer(0, mesh.buffer);

		prg->SetTexture(Shader::Type::Pixel, "diffuseMap", res->textures[mesh.texture]);
		core.render.GetDevice()->Draw(Device::TrianglesList, 0, mesh.num_triangles);
	}
}

void Model::LoadModelMS3D(const char* filename)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float2, VertexDecl::Texcoord, 0 },{ VertexDecl::Float3, VertexDecl::Texcoord, 1 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(3, desc);

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

	FileInMemory file;
	
	if (!file.Load(filename))
	{
		return;
	}

	char id[10];
	file.Read(&id, 10);
	if (strncmp(id, "MS3D000000", 10) != 0)
	{
		return;
	}

	int version;
	file.Read(&version, sizeof(int));
	if (version != 4)
	{
		return;
	}

	unsigned short numVertices;
	file.Read(&numVertices, sizeof(unsigned short));

	std::vector<MS3DVertex> vertices;
	vertices.resize(numVertices);

	for (int i = 0; i < numVertices; i++)
	{
		file.Read(&vertices[i].flags, sizeof(unsigned char));
		file.Read(&vertices[i].vertex, sizeof(float) * 3);
		file.Read(&vertices[i].boneID, sizeof(char));
		file.Read(&vertices[i].refCount, sizeof(unsigned char));
	}

	unsigned short numTriangles;
	file.Read(&numTriangles, sizeof(unsigned short));

	std::vector<MS3DTriangle> triangles;
	triangles.resize(numTriangles);

	for (int i = 0; i < numTriangles; i++)
	{
		file.Read(&triangles[i].flags, sizeof(unsigned short));
		file.Read(triangles[i].vertexIndices, sizeof(unsigned short) * 3);
		file.Read(triangles[i].vertexNormals, sizeof(float) * 3 * 3);
		file.Read(triangles[i].s, sizeof(float) * 3);
		file.Read(triangles[i].t, sizeof(float) * 3);
		file.Read(&triangles[i].smoothingGroup, 1);
		file.Read(&triangles[i].groupIndex, 1);
	}

	unsigned short numGroups;
	file.Read(&numGroups, sizeof(unsigned short));

	std::vector<MS3DGroup> groups;
	groups.resize(numGroups);

	bb_max = Vector(-100000.0f);
	bb_min = Vector(100000.0f);

	for (int i = 0; i < numGroups; i++)
	{
		file.Read(&groups[i].flags, sizeof(unsigned char));
		file.Read(groups[i].name, 32);

		unsigned short numGroupTriangles;
		file.Read(&numGroupTriangles, sizeof(unsigned short));
		groups[i].triangleIndices.resize(numGroupTriangles);

		if (numGroupTriangles > 0)
		{
			file.Read(&groups[i].triangleIndices[0], sizeof(unsigned short) * numGroupTriangles);
		}

		file.Read(&groups[i].materialIndex, 1);

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
		mesh.buffer = core.render.GetDevice()->CreateBuffer(numGroupTriangles * 3, sizeof(ModelVertex));

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
	file.Read(&numMaterials, sizeof(unsigned short));

	std::vector<MS3DMaterial> materials;
	materials.resize(numMaterials);

	textures.resize(numMaterials);

	for (int i = 0; i < numMaterials; i++)
	{
		file.Read(materials[i].name, sizeof(char) * 32);
		file.Read(materials[i].ambient, sizeof(float) * 4);
		file.Read(materials[i].diffuse, sizeof(float) * 4);
		file.Read(materials[i].specular, sizeof(float) * 4);
		file.Read(materials[i].emissive, sizeof(float) * 4);
		file.Read(&materials[i].shininess, sizeof(float));
		file.Read(&materials[i].transparency, sizeof(float));
		file.Read(&materials[i].mode, 1);
		file.Read(materials[i].texture, 128);
		file.Read(materials[i].alphamap, 128);

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

		textures[i] = core.render.LoadTexture(path);
	}
}