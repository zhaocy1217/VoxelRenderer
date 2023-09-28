#pragma once
#include <window.h>
#include <thread>
#include <chrono>
#include <GObject.h>
#include <PostProcessing.h>
_VOXEL_BEGIN
class Window;
class GObject;
class World
{
private:
	static World* inst;
	std::chrono::system_clock::time_point startClock;
	double totolTime;
	double lastFrameTotalTime;
	DrawFunc draw_func;
	std::function<void()> clean_func;
	std::function<void()> clean_gfx_func;
	std::function<void()> idle_func;
	std::thread mainThread;
	std::vector<std::shared_ptr<GObject>> objs;
	std::vector<std::shared_ptr<GObject>> to_del;
	uint32_t frame_count = 0;
	Window* w = nullptr;
	const int RenderDataSetCacheSize = 256;
	GfxRenderDataset* renderDataset_cache = new GfxRenderDataset[RenderDataSetCacheSize];
	void init();
	void gui();
	void gfx_init();
	void tick(float delta);
	void on_destroy();
	void on_window_update_event_call();
public:
	static World* create();
	static World* get_world();
	uint32_t targetFrameRate = 30;
	World();
	void start();
	uint32_t get_frame_count();
	std::function<void()> gui_func;
	PostProcessing* post_processing;
	void push_object(std::shared_ptr<GObject>&& g)
	{
		auto rst = std::find(objs.begin(), objs.end(), g);
		if (rst == objs.end())
		{
			objs.push_back(std::forward<std::shared_ptr<GObject>>(g));
		}
	}

	void remove_object(std::shared_ptr<GObject> g)
	{
		auto rst = std::find(objs.begin(), objs.end(), g);
		if (rst != objs.end())
		{
			to_del.push_back(g);
		}
	}
	void wait_to_fin();
	
	
};
_VOXEL_END