#include "mesh.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <Defines.h>
_VOXEL_BEGIN
Mesh::Mesh(const std::string& mesh_path):AssetItem(mesh_path)
{
}
Mesh::Mesh()
{
}
GenMeshGfxFuncPtr Mesh::gen_mesh_gfx_func = nullptr;
DestroyGfxAssetFuncPtr Mesh::destroy_mesh_gfx_func = nullptr;
std::shared_ptr<Mesh> Mesh::quad = nullptr;
void* Mesh::gen_gfx_handler(std::vector<VertexInfo> filters)
{
	 if (handler != nullptr)
	 {
		 return handler;
	 }
	std::vector<float> vertexData;
	std::vector<glm::vec3>::iterator ptr;
	std::vector<glm::vec2>::iterator uvptr = uv.begin();
	std::vector<glm::vec4>::iterator colorPtr = color.begin();
	std::vector<glm::vec3>::iterator normalPtr = normal.begin();
	std::vector<glm::vec3>::iterator tangentPtr = tangent.begin();
	bool hasColor = color.size() > 0;
	bool hasUV = uv.size();
	bool hasNormal = normal.size();
	bool hasTangent = tangent.size();
	for (ptr = vertex.begin(); ptr< vertex.end(); ptr++)
	{
		for (int j = 0; j < filters.size(); j++)
		{
			auto filter = filters[j];
			if (filter.ele == MeshElement::VERTEX)
			{
				vertexData.push_back((ptr)->x);
				vertexData.push_back((ptr)->y);
				vertexData.push_back((ptr)->z);
			}
			if (filter.ele == MeshElement::COLOR)
			{
				if (!hasColor)
				{
					vertexData.push_back(1); vertexData.push_back(1); vertexData.push_back(1); vertexData.push_back(1);
				}
				else
				{
					vertexData.push_back((colorPtr)->x);
					vertexData.push_back((colorPtr)->y);
					vertexData.push_back((colorPtr)->z);
					vertexData.push_back((colorPtr)->w);
				}
			}
			if (filter.ele == MeshElement::UV)
			{
				if (!hasUV)
				{
					vertexData.push_back(0); vertexData.push_back(0);
				}
				else
				{
					vertexData.push_back((uvptr)->x);
					vertexData.push_back((uvptr)->y);
				}
			}
			if (filter.ele == MeshElement::NORMAL)
			{
				if (!hasNormal)
				{
					vertexData.push_back(0); vertexData.push_back(1); vertexData.push_back(0);
				}
				else
				{
					vertexData.push_back((normalPtr)->x);
					vertexData.push_back((normalPtr)->y);
					vertexData.push_back((normalPtr)->z);
				}
			}
			if (filter.ele == MeshElement::TANGENT)
			{
				if (!hasTangent)
				{
					vertexData.push_back(1); vertexData.push_back(0); vertexData.push_back(0);
				}
				else
				{
					vertexData.push_back((tangentPtr)->x);
					vertexData.push_back((tangentPtr)->y);
					vertexData.push_back((tangentPtr)->z);
				}
			}
		}
		if(hasUV)
			uvptr++;
		if(hasColor)
			colorPtr++;
		if (hasNormal)
			normalPtr++;
		if (hasTangent)
			tangentPtr++;
	}
	handler = gen_mesh_gfx_func(vertexData, this->triangle);
	return handler;
}
std::shared_ptr< Mesh> Mesh::load(const std::string& mesh_path)
{
	 std::ifstream fin(mesh_path.c_str());
	 if (!fin.fail())
	 {
		 fin.close();
	 }
	 else
	 {
		 return nullptr;
	 }
	 Assimp::Importer importer;
	 auto* g_scene = importer.ReadFile(mesh_path, aiProcessPreset_TargetRealtime_Quality);
	 auto src_mesh = g_scene->mMeshes[0];
	 auto m = load(src_mesh);
	 importer.FreeScene();
	 return m;
}

std::shared_ptr<Mesh> Mesh::load(const aiMesh* src_mesh)
{
	auto m = std::make_shared<Mesh>();
	for (int i = 0; i < src_mesh->mNumVertices; i++)
	{
		auto* v = (src_mesh->mVertices + i);
		m->vertex.push_back(glm::vec3(v->x, v->y, v->z));
	}
	if (src_mesh->mNumUVComponents[0] > 0)
	{
		for (int i = 0; i < src_mesh->mNumVertices; i++)
		{
			aiVector3D* v = &src_mesh->mTextureCoords[0][i];
			m->uv.push_back(glm::vec2(v->x, v->y));
		}
	}
	if (src_mesh->HasFaces())
	{
		for (int i = 0; i < src_mesh->mNumFaces; i++)
		{
			int idx1 = *((src_mesh->mFaces + i)->mIndices);
			int idx2 = *((src_mesh->mFaces + i)->mIndices + 1);
			int idx3 = *((src_mesh->mFaces + i)->mIndices + 2);
			/* std::cout << m->vertex[idx1].x << "," << m->vertex[idx1].y << "," << m->vertex[idx1].z << ";"
				 << m->vertex[idx2].x << "," << m->vertex[idx2].y << "," << m->vertex[idx2].z << ";"
				 << m->vertex[idx3].x << "," << m->vertex[idx3].y << "," << m->vertex[idx3].z << ";" << std::endl;*/
			m->triangle.push_back(*((src_mesh->mFaces + i)->mIndices));
			m->triangle.push_back(*((src_mesh->mFaces + i)->mIndices + 1));
			m->triangle.push_back(*((src_mesh->mFaces + i)->mIndices + 2));
		}
	}
	if (src_mesh->HasNormals())
	{
		for (int i = 0; i < src_mesh->mNumVertices; i++)
		{
			auto* v = (src_mesh->mNormals + i);
			m->normal.push_back(glm::vec3(v->x, v->y, v->z));
		}
	}
	if (src_mesh->HasTangentsAndBitangents())
	{
		for (int i = 0; i < src_mesh->mNumVertices; i++)
		{
			auto* v = (src_mesh->mTangents + i);
			m->tangent.push_back(glm::vec3(v->x, v->y, v->z));
		}
	}
	if (src_mesh->HasVertexColors(0))
	{
		for (int i = 0; i < src_mesh->mNumVertices; i++)
		{
			auto v = (src_mesh->mColors[0] + i);
			m->color.push_back(glm::vec4(v->r, v->g, v->b, v->a));
		}
	}
	return m;
}

 Mesh::~Mesh()
 {
	 destroy_mesh_gfx_func(handler);
 }
 _VOXEL_END