#include<GLTFReader.h>
#include<Camera.h>
#include <assimp/postprocess.h>
_VOXEL_BEGIN
glm::vec3 to_glm_v3(aiVector3D aiv)
{
	return glm::vec3(aiv.x, aiv.y, aiv.z);
}
aiVector3D to_ai_v3(glm::vec3 aiv)
{
	return aiVector3D(aiv.x, aiv.y, aiv.z);
}
void matrix_aabb(aiAABB&aabb, aiMatrix4x4& m)
{
	aabb.mMin = m * aabb.mMin;
	aabb.mMax = m * aabb.mMax;
	aabb.mMin.x = fmin(aabb.mMin.x, aabb.mMax.x);
	aabb.mMin.y = fmin(aabb.mMin.y, aabb.mMax.y);
	aabb.mMin.z = fmin(aabb.mMin.z, aabb.mMax.z);

	aabb.mMax.x = fmax(aabb.mMin.x, aabb.mMax.x);
	aabb.mMax.y = fmax(aabb.mMin.y, aabb.mMax.y);
	aabb.mMax.z = fmax(aabb.mMin.z, aabb.mMax.z);
}
void encapsulate(aiAABB& aabb, aiAABB& aabb2)
{
	aabb.mMin.x = fmin(aabb.mMin.x, aabb2.mMin.x);
	aabb.mMin.y = fmin(aabb.mMin.y, aabb2.mMin.y);
	aabb.mMin.z = fmin(aabb.mMin.z, aabb2.mMin.z);

	aabb.mMax.x = fmax(aabb.mMax.x, aabb2.mMax.x);
	aabb.mMax.y = fmax(aabb.mMax.y, aabb2.mMax.y);
	aabb.mMax.z = fmax(aabb.mMax.z, aabb2.mMax.z);
}
std::shared_ptr<Texture> GLTFReader::GetTex(const aiScene* src_scene, aiMaterial* src_mat, aiTextureType textureType, int idx)
{
	aiString texPath;
	auto rst = src_mat->GetTexture(textureType, idx, &texPath);
	if (rst == aiReturn::aiReturn_FAILURE)
	{
		return nullptr;
	}
	auto texName = texPath.C_Str();
	bool srgb = textureType == aiTextureType::aiTextureType_BASE_COLOR || textureType == aiTextureType::aiTextureType_DIFFUSE;
	if ('*' == texPath.data[0])
	{
		auto tex = src_scene->GetEmbeddedTexture(texName);
		std::string texPath = std::string(texName) + "_" + std::to_string(VoxelRendering::Util2::getuid());
		if (tex->mHeight == 0)
		{
			return VoxelRendering::Resources::get_texture(texPath, reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, false, srgb, false);
		}
		else {
			return VoxelRendering::Resources::get_texture(texPath, reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth * tex->mHeight, false, srgb, false);
		}
	}
	return nullptr;
}
aiMatrix4x4  GLTFReader::GetMatrix(aiNode* node)
{
	auto matrix = node->mTransformation;
	if (node->mParent != nullptr)
	{
		matrix = matrix * node->mParent->mTransformation;
		node = node->mParent;
	}
	return matrix;
}
void  GLTFReader::ReadNode(aiNode* node, std::function<void(aiNode*)> func)
{
	if (node != nullptr)
	{
		func(node);
	}
	else
	{
		return;
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		auto child = node->mChildren[i];
		ReadNode(child, func);
	}
}
void GLTFReader::Read(World* world, std::string&& path)
{
	Read(world, path);
}
void GLTFReader::Read(World* world, std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(Resources::basePath + path, aiPostProcessSteps::aiProcess_MakeLeftHanded| aiProcess_GenBoundingBoxes);
	aiAABB* aabb = nullptr;
	auto func = [&](aiNode* node)
	{
		for (size_t j = 0; j < node->mNumMeshes; j++)
		{
			auto meshIdx = node->mMeshes[j];
			const aiMesh* mesh = scene->mMeshes[meshIdx];
			auto src_mat = scene->mMaterials[mesh->mMaterialIndex];
			auto vmesh = Resources::get_mesh(VoxelRendering::Util2::getuid(), mesh);
			auto vmat = VoxelRendering::Resources::get_material(VoxelRendering::Util2::getuid());
			vmat->set_shader(VoxelRendering::Resources::get_shader("assets/shaders/simple"));
			for (auto& pair : Material::material_tex_finder)
			{
				auto teuxtreTypes = pair.second;
				for (auto& t : teuxtreTypes)
				{
					auto tex = GetTex(scene, src_mat, t, 0);
					if (tex != nullptr)
					{
						vmat->set_texture(pair.first, tex);
						break;
					}
				}
			}
			vmat->set_texture(VoxelRendering::Material::DEFAULT_SKYBOX_MAP,
				VoxelRendering::Resources::get_texture(VoxelRendering::Material::DEFAULT_SKYBOX_PATH, true, true));
			auto local2world = GetMatrix(node);
			if (aabb == nullptr)
			{
				aabb = new aiAABB();
				*aabb = mesh->mAABB;
				matrix_aabb(*aabb, local2world);
			}
			else {
				auto temp = mesh->mAABB;
				matrix_aabb(temp, local2world);
				encapsulate(*aabb, temp);
			}
			aiVector3D t, r, s;
			local2world.Decompose(s, r, t);
			auto go = std::make_shared<VoxelRendering::GObject>();
			go->addComp<MeshRendererComp>(vmat, vmesh);
			auto trans = go->addComp<Transform>();
			auto q = glm::quat(glm::vec3(r.x, r.y, r.z));
			trans->setPRS(glm::vec3(t.x, t.y, t.z), q, glm::vec3(s.x, s.y, s.z));
			world->push_object(std::move(go));
		}
	};
	ReadNode(scene->mRootNode, func);
	auto mainCamera = Camera::main_camera;
	auto trans = Camera::main_camera->go->get_comp<Transform>();
	auto size = aabb->mMax - aabb->mMin;
	auto center = aabb->mMax + aabb->mMin;
	auto gcenter = glm::vec3(center.x/2., center.y / 2., center.z / 2.);
	glm::vec3 gsize = glm::vec3(size.x, size.y, size.z);
	auto dist = sqrt(gsize.x * gsize.x + gsize.y * gsize.y + gsize.z * gsize.z) / 2;
	auto forward = -glm::normalize(glm::vec3(0, 1, -2));
	trans->setPos(gcenter - forward*dist*1.5f);
	trans->setQuat(glm::quatLookAtLH(glm::vec3(forward.x, forward.y, forward.z), glm::vec3(0,1,0)));
	delete aabb;
	importer.FreeScene();
}
_VOXEL_END