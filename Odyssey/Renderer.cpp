#include <stdexcept>
#include "Renderer.h"

#include <algorithm>

//Todo: Remove this later
#include "TrashTheCache.h"
#include "SceneManager.h"
#include "Texture2D.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "../3rdParty/imgui-1.89.4/backends/imgui_impl_opengl2.h"
#include "../ImPlot/implot.h"



int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}

void dae::Renderer::Init(SDL_Window* window)
{
	m_window = window;
	m_renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL2_Init();

	ImPlot::CreateContext();


}

void dae::Renderer::Render() const
{
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	SceneManager::GetInstance().Render();

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();

	static bool didTrash{ false }, didCalculate{false};
	ImGui::Begin("Exercise 1");
	if (ImGui::Button("Trash the cache", ImVec2{ 140, 20 })) didTrash = true;

	if (didTrash)
	{
		static float xValues[11], yValues[11];

		if (!didCalculate)
		{
		int stepCount{ 1 };
			std::vector<float> measurements{};
			IntegersTiming(measurements);
			for (int i = 0; i < 11; ++i)
			{
				xValues[i] = float(stepCount);
				yValues[i] = measurements[i];
				stepCount *= 2;
			}
			didCalculate = true;
		}

		if (ImPlot::BeginPlot("Timings"))
		{
			ImPlot::SetupAxes("Step Count", "ms");
			ImPlot::PlotBars("test", yValues, 11);

			ImPlot::SetNextMarkerStyle(ImPlotMarker_Diamond);
			ImPlot::EndPlot();
		}
	}
	ImGui::End();

	static bool didTrash2{ false }, didTrash3{ false }, didCalculate2{ false }, didCalcualte3{ false };
	static float xValues2[11], yValues2[11];
	static float xValues3[11], yValues3[11];
	std::vector<float> measurementsGameObj{};
	std::vector<float> measurementsGameObjAlt{};

	ImGui::Begin("Exercise 2");

	if (ImGui::Button("GameObject", { 140, 20 })) didTrash2 = true;
	if (didTrash2)
	{
		if (!didCalculate2)
		{
			GameObjectTiming(false, measurementsGameObj);
			for (int currentNum{}; currentNum < 11; currentNum++)
			{
				xValues2[currentNum] = measurementsGameObj[currentNum];
				yValues2[currentNum] = measurementsGameObj[currentNum];
			}
			didCalculate2 = true;
		}

		if (ImPlot::BeginPlot("Timings GameObject"))
		{
			ImPlot::SetupAxes("Step Count", "ms");
			ImPlot::PlotBars("GameObject", yValues2, 11);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::EndPlot();
		}
	}

	if (ImGui::Button("GameObjectAlt", { 140, 20 })) didTrash3 = true;
	if (didTrash3)
	{
		if (!didCalcualte3)
		{
			GameObjectTiming(true, measurementsGameObjAlt);
			for (int currentNum{}; currentNum < 11; currentNum++)
			{

				xValues3[currentNum] = measurementsGameObjAlt[currentNum];
				yValues3[currentNum] = measurementsGameObjAlt[currentNum];
			}
			didCalcualte3 = true;
		}

		if (ImPlot::BeginPlot("Timings GameObjectAlt"))
		{
			ImPlot::SetupAxes("Step Count", "ms");
			ImPlot::PlotBars("GameObject", yValues2, 11);
			ImPlot::PlotBars("GameObjAlt", yValues3, 11, 0.4);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::EndPlot();
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	
	SDL_RenderPresent(m_renderer);
}

void dae::Renderer::Destroy()
{
	ImPlot::DestroyContext();

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width);
	dst.h = static_cast<int>(height);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

inline SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_renderer; }
