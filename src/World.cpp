#include <World.h>
#include <VkHelper.h>
#include<functional>
#include<RendererComp.h>
#include<Camera.h>
#include<Resources.h>
#include <MeshRendererComp.h>
#include <imgui.h>
_VOXEL_BEGIN
World* World::inst = nullptr;
void World::start()
{
	w->run();
}
void World::init()
{
	startClock = std::chrono::system_clock::now();
	w = new Window();
	w->m_drawFrameFunc = [&] {on_window_update_event_call(); };// std::bind(&(on_window_update_event_call), this);
	w->m_waitDeviceIdleFunc = [&] { if (idle_func != nullptr) { idle_func(); }};
	w->m_cleanFunc = std::bind(&World::on_destroy, this);
	//gui_func = std::bind(&World::gui, this);
	gfx_init();
}
bool show_demo_window = true;
bool show_another_window;
void World::gui()
{
	//gui_func();
	//ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//{

	//}
	//ImGui::SameLine();
	//ImGui::End();
}
void World::gfx_init()
{
	using namespace std::placeholders;
	auto create_surface_func = std::bind(&(Window::create_surface), w, _1, _2);
	auto get_size_func = std::bind(&(Window::get_size), w, _1, _2);
	auto get_ext_func = std::bind(&(Window::get_ext), w, _1);
	VkStartInfo start_info;
	start_info.create_surface_func = create_surface_func;
	start_info.get_ext_func = get_ext_func;
	start_info.get_size_func = get_size_func;
	start_info.clean_func = &this->clean_gfx_func;
	start_info.idle_func = &this->idle_func;
	start_info.set_raw_data_gen_tex_gfx_func = Texture::set_rawdata_texture_gfx_func;
	start_info.set_gen_tex_gfx_func = GfxTexture::set_gfx_func;
	start_info.set_gen_shader_gfx_func = Shader::set_gfx_func;
	start_info.set_gen_mesh_gfx_func = Mesh::set_gfx_func;
	start_info.setup_imgui_window_func = std::bind(&Window::setup_imgui, w);
	start_info.draw_func = &draw_func;
	VkHelper::start_vk(start_info);
	Resources::init_default_resource();
}
void World::tick(float delta)
{
	for (size_t i = 0; i < objs.size(); i++)
	{
		objs[i]->update(delta);
	}
	for (size_t i = 0; i < to_del.size(); i++)
	{
		auto rst = std::find(objs.begin(), objs.end(), to_del[i]);
		if (rst != objs.end())
		{
			objs.erase(rst);
		}
	}
	to_del.clear();
	CameraData cd;
	cd.view = Camera::main_camera->get_view_mat();
	cd.proj = Camera::main_camera->get_proj_mat();
	cd.cameraPosi = glm::vec4(Camera::main_camera->go->get_comp<VoxelRendering::Transform>()->get_pos(), 1);
	RenderingData rd;
	rd.lightDir = -glm::vec3(.0, -.57, -.57);
	std::vector<GfxRenderDataset*> renderers;
	int dataset_cache_idx = 0;
	for (size_t i = 0; i < objs.size(); i++)
	{
		auto r = objs[i]->get_comp<MeshRendererComp>();
		if (r != nullptr)
		{
			auto trans = objs[i]->get_comp<Transform>();
			ObjectData od;
			od.model = trans->get_matrix();
			RenderObjectData rod;
			rod.cd = cd;
			rod.od = od;
			rod.rd = rd;
			GfxRenderDataset* data = renderDataset_cache + dataset_cache_idx;
			if (data == nullptr)
			{
				data = new GfxRenderDataset(r->mesh, r->material, rod);
			}
			else
			{
				data->reset(r->mesh, r->material, rod);
			}
			renderers.push_back(data);
			dataset_cache_idx++;
		}
	}
	draw_func(renderers, post_processing, gui_func);
}
void World::on_destroy()
{
	delete[] renderDataset_cache;
	objs.clear();
	gui_func = nullptr;
	GfxTexture::clear_temp_cache();
	Resources::unload_everything();
	this->clean_gfx_func();
}
void World::on_window_update_event_call()
{
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - startClock;
	totolTime = elapsed_seconds.count();
	float costTimeCurrentFrame = totolTime - lastFrameTotalTime;
	auto timediff = 1.0 / targetFrameRate - costTimeCurrentFrame;
	if (timediff < 0)
	{
		frame_count++;
		tick(costTimeCurrentFrame);
		lastFrameTotalTime = elapsed_seconds.count();
		//std::this_thread::sleep_for(std::chrono::milliseconds((long)(timediff * 1000)));
	}
}
World* World::get_world()
{
	return inst;
}
World::World()
{
	totolTime = 0;
	lastFrameTotalTime = 0;
}
uint32_t World::get_frame_count()
{
	return frame_count;
}
void World::wait_to_fin()
{
	//mainThread.join();
}
World* World::create()
{
	World* newWorld = new World();
	newWorld->init();
	inst = newWorld;
	return newWorld;
}
_VOXEL_END