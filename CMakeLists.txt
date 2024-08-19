cmake_minimum_required(VERSION 3.26)
project(Odyssey)

# Include the FetchContent module
include(FetchContent)

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
)
FetchContent_MakeAvailable(imgui)

# Define ImGui source files
set(IMGUI_SOURCES
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

if (WIN32)
#Ignore compiler warnings for imgui_impl_opengl3.cpp
set_source_files_properties("${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
                           PROPERTIES COMPILE_FLAGS "/wd4701")
endif()

# Fetch GLM
FetchContent_Declare(
  glm
  URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
  DOWNLOAD_NO_PROGRESS ON
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
endif()

# Define Odyssey source files and assign to filters
set(ODYSSEY_AUDIO
  Odyssey/Audio.cpp
  Odyssey/Audio.h
  Odyssey/AudioSystem.cpp
  Odyssey/AudioSystem.h
  Odyssey/WebAudioSystem.cpp
  Odyssey/WebAudioSystem.h
  Odyssey/IAudio.h
)
source_group("Audio" FILES ${ODYSSEY_AUDIO})

source_group("ImGui Lib" FILES ${IMGUI_SOURCES})

set(ODYSSEY_MANAGERS
  Odyssey/ResourceManager.cpp
  Odyssey/ResourceManager.h
  Odyssey/SceneManager.cpp
  Odyssey/SceneManager.h
  Odyssey/StateManager.cpp
  Odyssey/StateManager.h
  Odyssey/ImGuiManager.cpp
  Odyssey/ImGuiManager.h
)
source_group("Managers" FILES ${ODYSSEY_MANAGERS})

set(ODYSSEY_ENGINECOMPONENTS
  Odyssey/CameraComponent.cpp
  Odyssey/CameraComponent.h
  Odyssey/Component.cpp
  Odyssey/Component.h
  Odyssey/RenderComponent.h
  Odyssey/TextComponent.h
  Odyssey/TransformComponent.cpp
  Odyssey/TransformComponent.h
  Odyssey/TextureComponent.h
  Odyssey/UpdateComponent.h
  Odyssey/AnimatedTextureComponent.h
  Odyssey/AnimatedTextureComponent.cpp
)
source_group("Engine Components" FILES ${ODYSSEY_ENGINECOMPONENTS})

set(ODYSSEY_INPUT
  Odyssey/Controller.cpp
  Odyssey/Controller.h
  Odyssey/InputManager2.cpp
  Odyssey/InputManager2.h
  Odyssey/XboxController.cpp
  Odyssey/XboxController.h
)
source_group("Input" FILES ${ODYSSEY_INPUT})

set(ODYSSEY_CORE
  Odyssey/Constants.h
  Odyssey/GameTime.cpp
  Odyssey/GameTime.h
  Odyssey/Odyssey.cpp
  Odyssey/Odyssey.h
  Odyssey/Renderer.cpp
  Odyssey/Renderer.h
  Odyssey/Structs.h
)
source_group("Core" FILES ${ODYSSEY_CORE})

set(ODYSSEY_TOOLS
  Odyssey/Logger.h
  Odyssey/PerformanceTimer.cpp
  Odyssey/PerformanceTimer.h
  Odyssey/Utils.cpp
  Odyssey/Utils.h
)
source_group("Tools" FILES ${ODYSSEY_TOOLS})

set(ODYSSEY_WRAPPERS
  Odyssey/Font.cpp
  Odyssey/Font.h
  Odyssey/Texture2D.cpp
  Odyssey/Texture2D.h
  Odyssey/DebugDrawer.cpp
  Odyssey/DebugDrawer.h
)
source_group("Wrappers" FILES ${ODYSSEY_WRAPPERS})

set(ODYSSEY_PATTERNS
  Odyssey/Command.h
  Odyssey/IPrefab.h
  Odyssey/Observer.h
  Odyssey/ServiceLocator.cpp
  Odyssey/ServiceLocator.h
  Odyssey/Singleton.h
  Odyssey/State.h
  Odyssey/Subject.h
)
source_group("Patterns" FILES ${ODYSSEY_PATTERNS})

set(ODYSSEY_SCENEGRAPH
  Odyssey/GameObject.cpp
  Odyssey/GameObject.h
  Odyssey/GameScene.cpp
  Odyssey/GameScene.h
)
source_group("SceneGraph" FILES ${ODYSSEY_SCENEGRAPH})

# Combine all source files
set(ODYSSEY_SOURCES
  ${ODYSSEY_AUDIO}
  ${ODYSSEY_IMGUILIB}
  ${ODYSSEY_MANAGERS}
  ${ODYSSEY_ENGINECOMPONENTS}
  ${ODYSSEY_INPUT}
  ${ODYSSEY_CORE}
  ${ODYSSEY_TOOLS}
  ${ODYSSEY_WRAPPERS}
  ${ODYSSEY_PATTERNS}
  ${ODYSSEY_SCENEGRAPH}
  ${IMGUI_SOURCES}
)

# Create the Odyssey static library
add_library(OdysseyLib STATIC ${ODYSSEY_SOURCES})

# Include ImGui and GLM directories for the Odyssey library
target_include_directories(OdysseyLib PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${glm_SOURCE_DIR}
)


# Add SDL2 dependencies for the Odyssey library
if(EMSCRIPTEN)
  # Declare the SDL_AudioPlayer for Emscripten builds
  FetchContent_Declare(
    sdl_audioplayer
    GIT_REPOSITORY https://github.com/madureira/SDL_AudioPlayer.git
  )
  FetchContent_GetProperties(sdl_audioplayer)
  if(NOT sdl_audioplayer_POPULATED)
    FetchContent_Populate(sdl_audioplayer)
  endif()

  target_include_directories(OdysseyLib PUBLIC ${sdl_audioplayer_SOURCE_DIR})

elseif(WIN32)
  FetchContent_Declare(
    SDL2
    URL https://www.libsdl.org/release/SDL2-devel-2.28.5-VC.zip 
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads 
  )
  FetchContent_GetProperties(SDL2)
  if(NOT SDL2_POPULATED)
    FetchContent_Populate(SDL2)
    set(SDL2_INCLUDE_DIR ${sdl2_SOURCE_DIR}/include)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
      set(SDL2_LIBRARIES "${sdl2_SOURCE_DIR}/lib/x64/SDL2.lib;${sdl2_SOURCE_DIR}/lib/x64/SDL2main.lib")
      set(SDL2_LIBRARY_DLL "${sdl2_SOURCE_DIR}/lib/x64/SDL2.dll")
    else()
      set(SDL2_LIBRARIES "${sdl2_SOURCE_DIR}/lib/x86/SDL2.lib;${sdl2_SOURCE_DIR}/lib/x86/SDL2main.lib")
      set(SDL2_LIBRARY_DLL "${sdl2_SOURCE_DIR}/lib/x86/SDL2.dll")
    endif()
  endif()

  # add SDL2_image
  FetchContent_Declare(
    SDL2_image
    URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.8.2-VC.zip 
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_GetProperties(SDL2_image)
  if(NOT SDL2_image_POPULATED)
    FetchContent_Populate(SDL2_image)
    set(SDL2_IMAGE_INCLUDE_DIR ${sdl2_image_SOURCE_DIR}/include)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
      set(SDL2_IMAGE_LIBRARIES "${sdl2_image_SOURCE_DIR}/lib/x64/SDL2_image.lib")
      set(SDL2_IMAGE_LIBRARY_DLL "${sdl2_image_SOURCE_DIR}/lib/x64/SDL2_image.dll")
    else()
      set(SDL2_IMAGE_LIBRARIES "${sdl2_image_SOURCE_DIR}/lib/x86/SDL2_image.lib")
      set(SDL2_IMAGE_LIBRARY_DLL "${sdl2_image_SOURCE_DIR}/lib/x86/SDL2_image.dll")
    endif()
  endif()
  
  #add SDL2_Mixer
  FetchContent_Declare(
    SDL2_mixer
    URL https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.8.0-VC.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_GetProperties(SDL2_mixer)
  if(NOT SDL2_mixer_POPULATED)
    FetchContent_Populate(SDL2_mixer)
    set(SDL2_MIXER_INCLUDE_DIR ${sdl2_mixer_SOURCE_DIR}/include)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
      set(SDL2_MIXER_LIBRARIES "${sdl2_mixer_SOURCE_DIR}/lib/x64/SDL2_mixer.lib")
      set(SDL2_MIXER_LIBRARY_DLL "${sdl2_mixer_SOURCE_DIR}/lib/x64/SDL2_mixer.dll")
    else()
      set(SDL2_MIXER_LIBRARIES "${sdl2_mixer_SOURCE_DIR}/lib/x86/SDL2_mixer.lib")
      set(SDL2_MIXER_LIBRARY_DLL "${sdl2_mixer_SOURCE_DIR}/lib/x86/SDL2_mixer.dll")
    endif()
  endif()

  # add SDL2_ttf
  FetchContent_Declare(
    SDL2_ttf
    URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.20.2-VC.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_GetProperties(SDL2_ttf)
  if(NOT sdl2_ttf_POPULATED)
    FetchContent_Populate(SDL2_ttf)
    set(SDL2_TTF_INCLUDE_DIR ${sdl2_ttf_SOURCE_DIR}/include)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
      set(SDL2_TTF_LIBRARIES ${sdl2_ttf_SOURCE_DIR}/lib/x64/SDL2_ttf.lib)
      set(SDL2_TTF_LIBRARY_DLL "${sdl2_ttf_SOURCE_DIR}/lib/x64/SDL2_ttf.dll")
    else()
      set(SDL2_TTF_LIBRARIES ${sdl2_ttf_SOURCE_DIR}/lib/x86/SDL2_ttf.lib)
      set(SDL2_TTF_LIBRARY_DLL "${sdl2_ttf_SOURCE_DIR}/lib/x86/SDL2_ttf.dll")
    endif()
  endif()

  # set all include directories for the Odyssey library
  target_include_directories(OdysseyLib PUBLIC
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
  )
  
  # set libraries to link with for the Odyssey library
  target_link_libraries(OdysseyLib PUBLIC
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
  )

  # add post build commands for the Odyssey library
  add_custom_command(TARGET OdysseyLib POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${SDL2_LIBRARY_DLL}" "$<TARGET_FILE_DIR:OdysseyLib>"
  )
  add_custom_command(TARGET OdysseyLib POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${SDL2_IMAGE_LIBRARY_DLL}" "$<TARGET_FILE_DIR:OdysseyLib>"
  )
  add_custom_command(TARGET OdysseyLib POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${SDL2_TTF_LIBRARY_DLL}" "$<TARGET_FILE_DIR:OdysseyLib>"
  )
  add_custom_command(TARGET OdysseyLib POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "${SDL2_MIXER_LIBRARY_DLL}" "$<TARGET_FILE_DIR:OdysseyLib>"
  )

else()
  FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.5
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(SDL2)
  
  FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  set(SDL2IMAGE_INSTALL OFF)
  FetchContent_MakeAvailable(SDL2_image)

  FetchContent_Declare(
    SDL2_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG release-2.8.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  set(SDL2MIXER_INSTALL OFF)
  FetchContent_MakeAvailable(SDL2_mixer)

  FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.20.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  set(SDL2TTF_INSTALL OFF)
  FetchContent_MakeAvailable(SDL2_ttf)

  target_include_directories(OdysseyLib PUBLIC 
    ${sdl2_SOURCE_DIR}/include
    ${sdl2_image_SOURCE_DIR}/include
    ${sdl2_ttf_SOURCE_DIR}
	${sdl2_mixer_SOURCE_DIR}
  )
  target_link_libraries(OdysseyLib PUBLIC 
    SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer
  )
endif()

# Set C++20 standard for the Odyssey library
set_target_properties(OdysseyLib PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

# Create the Game executable
add_executable(Game
    Testing/src/Application.cpp
    Testing/src/CardRenderPackage.h
    Testing/src/Tichu.cpp
    Testing/src/Tichu.h
    Testing/src/ButtonManagerComponent.cpp
    Testing/src/ButtonManagerComponent.h
    Testing/src/Scenes/TichuScene.cpp
    Testing/src/Scenes/TichuScene.h
    Testing/src/Components/CardComponent.cpp
    Testing/src/Components/CardComponent.h
    Testing/src/Components/PlayerComponent.cpp
    Testing/src/Components/PlayerComponent.h
    Testing/src/Commands/CardSelectCommand.cpp
    Testing/src/Commands/CardSelectCommand.h
    Testing/src/Commands/ButtonPressed.cpp
    Testing/src/Commands/ButtonPressed.h

  ${IMGUI_SOURCES}
)

# Link the Game executable with the Odyssey library, ImGui, and GLM
target_link_libraries(Game PRIVATE OdysseyLib ${IMGUI_LIBRARIES})
target_include_directories(Game PRIVATE
  ${CMAKE_SOURCE_DIR}/Odyssey/
  ${CMAKE_SOURCE_DIR}/Testing/src
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
  ${glm_SOURCE_DIR}
)

# Set C++20 standard for the Game executable
set_target_properties(Game PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

# Compile options for both targets
target_compile_options(OdysseyLib PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_compile_options(Game PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# EMSCRIPTEN-specific settings
if(EMSCRIPTEN)
    set(EMSCRIPTEN_COMPILE_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2 -sNO_DISABLE_EXCEPTION_CATCHING -g")
    
    # Note the correction in the placement of the quotation marks around the path for --preload-file
    set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2 -g -sNO_DISABLE_EXCEPTION_CATCHING -sALLOW_MEMORY_GROWTH --preload-file ${CMAKE_SOURCE_DIR}/Data@/ -sSDL2_IMAGE_FORMATS=['tga','png']")
    
    #Work around https://github.com/emscripten-core/emscripten/issues/11513
    set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -fno-stack-protector")
    
    #Use the Emscripten toolchain
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenToolchain.cmake)
    
    #Additional settings for the Odyssey library
    set_target_properties(OdysseyLib PROPERTIES
        COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
        LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
    )
    
    #Additional settings for the Game executable
    set_target_properties(Game PROPERTIES
        COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
        LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
    )
    
    #Have Emscripten generate an HTML page for the Game executable
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    #Set the startup project for Visual Studio

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)
    #Add post build commands for the Game executable

    get_target_property(GAME_OUTPUT_DIR Game RUNTIME_OUTPUT_DIRECTORY)
    if(NOT GAME_OUTPUT_DIR)
        set(GAME_OUTPUT_DIR ${CMAKE_BINARY_DIR})
    endif()

    message(STATUS "Game target output directory: ${GAME_OUTPUT_DIR}")

    add_custom_command(TARGET Game POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "${GAME_OUTPUT_DIR}/Data"
    )

endif()