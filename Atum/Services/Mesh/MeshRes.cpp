
#include "MeshRes.h"
#include "Services/Core/Core.h"
#include "MeshPrograms.h"
#include "Services/Scene/ExecuteLevels.h"

#include "Support/fbx/ofbx.h"

void Mesh::Instance::Init(Mesh* model, TaskExecutor::SingleTaskPool* setTaskPool)
{
	res = model;

	taskPool = setTaskPool;
	taskPool->AddTask(ExecuteLevels::Geometry, this, (Object::Delegate)&Mesh::Instance::Render);

	//render.AddDelegate("toshadow", this, (Object::Delegate)&Model::Drawer::Render, 0);
	//render.AddDelegate("geometry", this, (Object::Delegate)&Model::Drawer::Render, 0);
	//render.AddDelegate("shgeometry", this, (Object::Delegate)&Model::Drawer::ShRender, 0);
}

Vector3 Mesh::Instance::GetBBMin()
{
	return res->bb_min;
}

Vector3 Mesh::Instance::GetBBMax()
{
	return res->bb_max;
}

void Mesh::Instance::SetPosition(Vector2 pos)
{
	Vector3 centerPos = (res->bb_max + res->bb_min) * 0.5f;

	transform.Identity();
	transform.Pos() = -centerPos;
	transform.Pos().x += pos.x;
	transform.Pos().y = res->bb_min.y + 0.0f;
	transform.Pos().z += pos.y;
}

void Mesh::Instance::Render(float dt)
{
	Render(MeshPrograms::GetTranglPrg());
}

void Mesh::Instance::ShRender(float dt)
{
	Render(MeshPrograms::GetShdTranglPrg());
}

void Mesh::Instance::Render(Program* prg)
{
	if (!show)
	{
		return;
	}

	Matrix trans;
	core.render.GetTransform(Render::WrldViewProj, trans);

	core.render.GetDevice()->SetProgram(prg);

	prg->SetMatrix(Shader::Type::Vertex, "trans", &transform, 1);
	prg->SetMatrix(Shader::Type::Vertex, "view_proj", &trans, 1);
	prg->SetVector(Shader::Type::Pixel, "color", (Vector4*)&color, 1);

	core.render.GetDevice()->SetVertexDecl(res->vdecl);

	for (int i = 0; i < res->meshes.size(); i++)
	{
		SubMesh &mesh = res->meshes[i];

		if (!mesh.indices)
		{
			continue;
		}

		core.render.GetDevice()->SetVertexBuffer(0, mesh.vertices);
		core.render.GetDevice()->SetIndexBuffer(mesh.indices);

		prg->SetTexture(Shader::Type::Pixel, "diffuseMap", mesh.texture != -1 ? res->textures[mesh.texture] : nullptr);
		core.render.GetDevice()->Draw(Device::TrianglesList, 0, mesh.num_triangles);
	}
}

void Mesh::Instance::GetLocatorTransform(const char* name, Matrix& loc_transform)
{
	if (res->locators.count(name))
	{
		loc_transform = res->locators[name] * transform;
	}
	else
	{
		loc_transform = transform;
	}
}

void Mesh::Instance::Release()
{
	taskPool->DelAllTasks(this);

	if (core.meshes.DecRef(res))
	{
		res->Release();
	}

	delete this;
}

bool Mesh::Load(const char* filename)
{
	VertexDecl::ElemDesc desc[] = { { VertexDecl::Float3, VertexDecl::Position, 0 },{ VertexDecl::Float2, VertexDecl::Texcoord, 0 },{ VertexDecl::Float3, VertexDecl::Texcoord, 1 } };
	vdecl = core.render.GetDevice()->CreateVertexDecl(3, desc);

	return LoadFBX(filename);
}

bool Mesh::LoadFBX(const char* filename)
{
	FileInMemory file;
	if (!file.Load(filename))
	{
		return false;
	}

	auto* scene = ofbx::load(file.GetData(), file.GetSize(), 1 << 0);

	if (!scene)
	{
		return false;
	}

	auto objects = scene->getAllObjects();

	for (int i = 0; i < scene->getAllObjectCount(); i++)
	{
		auto node = objects[i];
		
		if (node->getType() == ofbx::Object::Type::NULL_NODE)
		{
			auto fbx_mat = node->getGlobalTransform();
			Matrix mat;

			for (int j = 0; j < 16; j++)
			{
				mat.matrix[j] = (float)fbx_mat.m[j];
			}

			locators[node->name] = mat;
		}
	}

	meshes.resize(scene->getMeshCount());
 
	for (int i = 0; i<scene->getMeshCount(); i++)
	{
		auto* fbx_mesh = scene->getMesh(i);

		auto fbx_mat = fbx_mesh->getGlobalTransform();
		Matrix mat;

		for (int j = 0; j < 16; j++)
		{
			mat.matrix[j] = (float)fbx_mat.m[j];
		}

		auto& mesh = meshes[i];

		auto* geometry = fbx_mesh->getGeometry();

		if (fbx_mesh->getMaterialCount() > 0)
		{
			auto tex = fbx_mesh->getMaterial(0)->getTexture(ofbx::Texture::TextureType::DIFFUSE);

			if (tex)
			{
				auto texture_name = tex->getRelativeFileName();

				char name[256];
				int len = (int)(texture_name.end - texture_name.begin);
				memcpy(name, texture_name.begin, len);
				name[len] = 0;

				for (int i = 0; i < textures.size(); i++)
				{
					if (StringUtils::IsEqual(textures[i]->name.c_str(), name))
					{
						mesh.texture = i;
						break;
					}
				}

				if (mesh.texture == -1)
				{
					textures.push_back(core.render.LoadTexture(name));
					mesh.texture = (int)textures.size() - 1;
				}
			}
		}

		mesh.num_triangles = geometry->getIndexCount() / 3;

		mesh.indices = core.render.GetDevice()->CreateBuffer(geometry->getIndexCount(), sizeof(uint32_t));
		uint32_t* mesh_indices = (uint32_t*)mesh.indices->Lock();

		auto* indices = geometry->getFaceIndices();

		for (int j = 0; j < geometry->getIndexCount(); j+=3)
		{
			int index = indices[j];
			if (index < 0)
			{
				index = -index - 1;
			}

			mesh_indices[j] = index;
		}

		mesh.indices->Unlock();

		mesh.vertices = core.render.GetDevice()->CreateBuffer(geometry->getVertexCount(), sizeof(MeshVertex));
		MeshVertex* mesh_vertices = (MeshVertex*)mesh.vertices->Lock();

		auto* vertices = geometry->getVertices();
		auto* normals = geometry->getNormals();
		auto* uvs = geometry->getUVs();

		for (int j = 0; j < geometry->getVertexCount(); j++)
		{
			MeshVertex& vertex = mesh_vertices[j];
			auto& fbx_vertex = vertices[j];

			vertex.pos = Vector3((float)fbx_vertex.x, (float)fbx_vertex.y, (float)fbx_vertex.z) * mat;

			if (normals)
			{
				auto& fbx_normal = normals[j];
				vertex.normal = Vector3((float)fbx_normal.x, (float)fbx_normal.z, -(float)fbx_normal.y);
			}

			if (uvs)
			{
				auto& fbx_uv = uvs[j];
				vertex.uv = Vector2((float)fbx_uv.x, 1.0f - (float)fbx_uv.y);
			}

			bb_max.Max(vertex.pos);
			bb_min.Min(vertex.pos);
		}

		mesh.vertices->Unlock();
	}

	scene->destroy();

	return true;
}

void Mesh::Release()
{
	for (auto& mesh : meshes)
	{
		RELEASE(mesh.indices);
		RELEASE(mesh.vertices);
	}

	for (auto texture : textures)
	{
		RELEASE(texture);
	}

	delete this;
}