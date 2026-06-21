cmake_minimum_required(VERSION 3.15)
project(Project2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# 核心機制：強迫 Windows MSVC 使用 /MT (多執行緒靜態連結)
# 這是消滅 C++ 執行時期 DLL 的關鍵
# ==========================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/utf-8)
endif()


set(OpenGL_GL_PREFERENCE GLVND)


add_definitions(-DGLEW_STATIC)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends
)


set(SOURCES
    "src/main.cpp"
    "src/App/App.cpp"
    "src/Camera/Camera.cpp"
    "src/Scene/MainScene.cpp"
    "src/Scene/Calculation.cpp"
    "src/Scene/GetSet.cpp"
    "src/Scene/GetSetEffects.cpp"
    "src/Scene/InitMovement.cpp"
    "src/Scene/IO.cpp"
    "src/Scene/Load.cpp"
    "src/Scene/UpdateModel.cpp"
    "src/Scene/UpdateAction.cpp"
    "src/Scene/UpdateAttribute.cpp"
    "src/Scene/Render.cpp"
    "src/Scene/RWJson.cpp"
    "src/Utilty/OBJLoader.cpp"
    "src/Window/ControlWindow.cpp"
    "src/Shader/Shader.cpp"
    "src/Skybox/Skybox.cpp"
    "src/Lightning/Lightning.cpp"
)

set(IMGUI_SOURCES
    "include/imgui/imgui.cpp"
    "include/imgui/imgui_draw.cpp"
    "include/imgui/imgui_demo.cpp"
    "include/imgui/imgui_tables.cpp"
    "include/imgui/imgui_widgets.cpp"
    "include/imgui/backends/imgui_impl_glfw.cpp"
    "include/imgui/backends/imgui_impl_opengl3.cpp"
)

add_executable(Project2 ${SOURCES} ${IMGUI_SOURCES})


if(WIN32)
    target_link_libraries(Project2 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew32s.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw3_mt.lib
        opengl32  
        gdi32    
        user32    
        shell32   
    )
else()
    # Linux (CachyOS) 環境：維持原本的動態庫搜尋邏輯
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    
    # 務實提醒：你的原始碼用的是 ImGui GLFW backend，但你原本腳本卻找了 GLUT。
    # 這裡幫你拿掉無用的 GLUT 搜尋，維持乾淨。
    
    target_link_libraries(Project2 PRIVATE
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        glfw m dl X11
    )
endif()