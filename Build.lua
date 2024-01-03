-- premake5.lua
workspace "Loc-Chat"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
--    location "build"
   startproject "Loc-ChatApp"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

-- Directories
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
WalnutNetworkingBinDir = "Walnut/Walnut-Modules/Walnut-Networking/vendor/GameNetworkingSockets/bin/%{cfg.system}/%{cfg.buildcfg}/"

include "Walnut/Build-Walnut-External.lua"

group "Loc-ChatApp"
	include "App-Common/Build-App-Common.lua"
	include "App-Client/Build-App-Client.lua"
	include "App-Server/Build-App-Server.lua"
	include "User/Build-User.lua"
	include "Authentication/Build-Authentication.lua"
   include "App-Utils/Build-App-Utils.lua"
   include "Message/Build-Message.lua"
   include "Last-Online/Build-Last-Online.lua"
group ""