workspace "Minecraft"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    startproject "Minecraft"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "glfw"
    location "vendor/glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "vendor/glfw/include/GLFW/**.h",
        "vendor/glfw/src/**.c"
    }

    includedirs { "vendor/glfw/include" }

    filter "system:windows"
        systemversion "latest"
        defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        optimize "on"


project "Minecraft"
    location "Minecraft"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Minecraft/src/**.h",
        "Minecraft/src/**.hpp",
        "Minecraft/src/**.cpp",

        "vendor/glad/src/glad.c",

        "vendor/imgui/*.cpp",
        "vendor/imgui/backends/imgui_impl_glfw.cpp",
        "vendor/imgui/backends/imgui_impl_opengl3.cpp"
    }

    includedirs {
        "Minecraft/src",

        "vendor/glfw/include",
        "vendor/glad/include",
        "vendor/glm",
        "vendor/stb",
        "vendor/FastNoiseLite/Cpp",
        "vendor/imgui",
        "vendor/imgui/backends",
        "vendor/spdlog/include"
    }

    links {
        "glfw",
        "opengl32"
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "NOMINMAX" }
        buildoptions { "/utf-8" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"