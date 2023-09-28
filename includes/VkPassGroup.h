#pragma once
#include <VkHelper.h>
#include<VkPass.h>
#include<VkAssetWrapper.h>
_VOXEL_BEGIN
class VkTextureWrapper;
class VkPass;
class VkPassGroup
{
public:
	std::vector <std::shared_ptr<VkPass>> passes;
	static void blit(VkCommandBuffer vkCmd, std::shared_ptr<GfxTexture>  source, std::shared_ptr<GfxTexture>  dest, std::shared_ptr<Material> mat);
	static void blit2(VkCommandBuffer vkCmd, std::shared_ptr<GfxTexture> source, std::shared_ptr <GfxTexture> dest);
	static void blit(VkCommandBuffer vkCmd, std::shared_ptr<GfxTexture> source, std::shared_ptr <GfxTexture> dest);
	VkPassGroup();
	~VkPassGroup()
	{
		passes.clear();
	}
};
_VOXEL_END
