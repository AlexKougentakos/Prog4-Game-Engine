#include <stdexcept>
#include "Renderer.h"

#include <backends/imgui_impl_sdl2.h>

#include "Constants.h"
#include "DebugDrawer.h"
#include "SceneManager.h"
#include "Texture2D.h"
#include "imgui.h"
#include "ImGuiManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "backends/imgui_impl_opengl3.h"

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

void ody::Renderer::Init(SDL_Window* window)
{
	m_window = window;
	SDL_SetWindowSize(m_window, constants::g_ScreenWidth, constants::g_ScreenHeight);
	m_renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
#endif

	ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL3_Init();
}

void ody::Renderer::Render() const
{
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	auto& sceneManager = SceneManager::GetInstance();

	sceneManager.Render();

	SDL_RenderFlush(m_renderer);

#ifdef _DEBUG
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	sceneManager.OnGUI();
	ImGuiManager::GetInstance().Render();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	DebugDrawer::GetInstance().Render(m_renderer);
	DebugDrawer::GetInstance().Clear(); // Clear after rendering so that the shapes don't persist

#endif
	
	SDL_RenderPresent(m_renderer);
}

void ody::Renderer::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void ody::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, float scale) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	dst.w = static_cast<int>(dst.w * scale);
	dst.h = static_cast<int>(dst.h * scale);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void ody::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, float width, float height, float scale) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width * scale);
	dst.h = static_cast<int>(height * scale);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void ody::Renderer::RenderTexture(const Texture2D& texture, float x, float y, float width, float height, float srcX, float srcY, float scale , SDL_RendererFlip flip) const
{
	SDL_Rect src{};
	src.x = static_cast<int>(srcX);
	src.y = static_cast<int>(srcY);
	src.w = static_cast<int>(width);
	src.h = static_cast<int>(height);

	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width * scale);
	dst.h = static_cast<int>(height * scale);

	//SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), &src, &dst);
	SDL_RenderCopyEx(GetSDLRenderer(), texture.GetSDLTexture(), &src, &dst, 0.0, nullptr, flip);
}

// New method with rotation support
void ody::Renderer::RenderTexture(const Texture2D& texture, float x, float y, float width, float height, float rotation, float scale, SDL_RendererFlip flip) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width * scale);
	dst.h = static_cast<int>(height * scale);

	SDL_Point center{dst.w / 2, dst.h / 2};  // Rotate around the center of the texture

	SDL_RenderCopyEx(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, rotation, &center, flip);
}

SDL_Renderer* ody::Renderer::GetSDLRenderer() const { return m_renderer; }
