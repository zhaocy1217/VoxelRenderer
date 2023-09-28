#pragma once
#include <optional>
#include <array>
#include <vector>
#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"
#include <set>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/quaternion.hpp>
#define _VOXEL_BEGIN namespace VoxelRendering {
#define _VOXEL_END   }
_VOXEL_BEGIN
class GfxTexture;
class Material;
class TextureBaseInfo;
class GfxRenderDataset;
class PostProcessing;
typedef void* TexGfxHandler;
typedef void* (*GenMeshGfxFuncPtr)(std::vector<float>&, std::vector<unsigned int>&);
typedef void (*DestroyGfxAssetFuncPtr)(void* handler);
typedef std::function<void(std::shared_ptr<GfxTexture>, std::shared_ptr<GfxTexture>, std::shared_ptr<Material>)> BlitFunc;
typedef std::function<void(std::shared_ptr<GfxTexture>, std::shared_ptr<GfxTexture>)> Blit2Func;
typedef std::function<void(std::vector<GfxRenderDataset*>&, PostProcessing*, std::function<void()>)> DrawFunc;
//GfxTexture* RenderContext::create_gfxtex(TextureBaseInfo& texinfo)
enum class MeshElement
{
    VERTEX,//= 1 << 0,
    COLOR,//= 1 << 1,
    UV,//= 1<<2,
    NORMAL,//= 1<<3
    TANGENT
};
enum class RenderType
{
    OPAQUE, TRANSPARENT
};
enum class BindingTypes
{
    UNIFORMBUFFER_VERTEX = 1u,
    UNIFORMBUFFER_FRAGMENT = 2u,
    IMAGE = 3u,
    IMAGE_SAMPLER = 4u
};
enum class ShaderType
{
    VERTEX_STAGE, FRAGMENT_STAGE
};
class TextureBaseInfo
{
public:
    glm::uvec2 size = glm::zero<glm::uvec2>();
    unsigned int channels = 0;
    unsigned int mipLevels = 0;
    bool cubemap = false;
    bool srgb = false;
    bool depth = false;
    uint32_t msaa = 1;
    bool isAttachment = false;
};
typedef void* (*GenRawDataTexGfxFuncPtr)(unsigned char*, TextureBaseInfo&);
typedef void* (*GenTexGfxFuncPtr)(TextureBaseInfo&);
struct ShaderBindingObject
{
    std::string name;
    uint32_t binding_location;
    uint32_t block_size;
    std::string type;
    BindingTypes binding_type;
};
struct ShaderUniformBufferMember
{
    std::string name;
    std::string type;
    int offset;
};
struct ShaderUniformBuffer
{
    std::string name;
    int block_size;
    std::vector<ShaderUniformBufferMember> members;
};
struct VertexInfo
{
    MeshElement ele;
    int size;
    int location;
};
struct ShaderVertexInput
{
    std::vector<VertexInfo> inputs;
    int get_size()
    {
        int size = 0;
        for (size_t i = 0; i < inputs.size(); i++)
        {
            size += inputs[i].size;
        }
        return size;
    }
};
typedef void* (*GenShaderGfxFuncType)(std::vector<char>&, std::vector<char>&, ShaderVertexInput&, std::vector<ShaderBindingObject>&);
struct VkStartInfo
{
    std::function<void(VkInstance, VkSurfaceKHR*)> create_surface_func;
    std::function<const char** (uint32_t*)> get_ext_func;
    std::function<void(uint32_t*, uint32_t*)>get_size_func;
    std::function<void()>* clean_func;
    std::function<void()>* idle_func;
    std::function<void(GenRawDataTexGfxFuncPtr, DestroyGfxAssetFuncPtr)> set_raw_data_gen_tex_gfx_func;
    std::function<void(GenTexGfxFuncPtr, DestroyGfxAssetFuncPtr)> set_gen_tex_gfx_func;
    std::function<void(GenShaderGfxFuncType, DestroyGfxAssetFuncPtr)> set_gen_shader_gfx_func;
    std::function<void(GenMeshGfxFuncPtr, DestroyGfxAssetFuncPtr)> set_gen_mesh_gfx_func;
    std::function<void()> setup_imgui_window_func;
    DrawFunc* draw_func;
};
struct BuiltinPropUbo
{
    std::string static GetName()
    {
        return "builtin_props";
    }
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 lightdir;
    glm::vec4 camera_posi;
};
struct CameraData
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 cameraPosi;
};
struct RenderingData
{
    glm::vec3 lightDir;

};
struct ObjectData
{
    glm::mat4 model;
};
struct RenderObjectData
{
    CameraData cd;
    RenderingData rd;
    ObjectData od;
};
struct IntRect
{
    IntRect(int x,int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    int x, y, w, h;
};
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
enum class ATTACHMENT_TYPE
{
    COLOR = 1,
    DEPTH = 2
};
struct LoadStroreOp
{
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
};
struct PassCreationInfo
{
    uint32_t flags;
    VkFormat colorFormat;
    uint32_t msaa;
    LoadStroreOp colorOp = { VK_ATTACHMENT_LOAD_OP_CLEAR , VK_ATTACHMENT_STORE_OP_STORE };
    LoadStroreOp depthOp = { VK_ATTACHMENT_LOAD_OP_CLEAR , VK_ATTACHMENT_STORE_OP_DONT_CARE };;
};
struct VkImageMemBarrier
{
    VkPipelineStageFlags src_stage_mask{ VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

    VkPipelineStageFlags dst_stage_mask{ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };

    VkAccessFlags src_access_mask{ 0 };

    VkAccessFlags dst_access_mask{ 0 };

    VkImageLayout old_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

    VkImageLayout new_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

    uint32_t old_queue_family{ VK_QUEUE_FAMILY_IGNORED };

    uint32_t new_queue_family{ VK_QUEUE_FAMILY_IGNORED };
};

_VOXEL_END
