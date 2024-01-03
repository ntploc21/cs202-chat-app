project "User"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.cpp", "Source/**.hpp" }

   includedirs
   {
      "../App-Common/Source",

      "../Walnut/vendor/imgui",
      "../Walnut/vendor/glfw/include",
      "../Walnut/vendor/glm",

      "../Walnut/Walnut/Source",
      "../Walnut-Networking/Source",

      "%{IncludeDir.VulkanSDK}",
      "../Walnut/vendor/spdlog/include",
      "../Walnut/vendor/yaml-cpp/include",

      "../Walnut-Networking/vendor/GameNetworkingSockets/include",

      "../date/include/",
   }

   links
   {
       "Walnut",
       "Walnut-Networking",
       
      "yaml-cpp",

      -- "date",
   }

   defines
   {
       "YAML_CPP_STATIC_DEFINE"
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WL_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "WL_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "WL_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "WL_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"