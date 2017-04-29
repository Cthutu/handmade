-- Handmade hero build script

rootdir = path.join(path.getdirectory(_SCRIPT), "..")

filter { "platforms:Win64" }
	system "Windows"
	architecture "x64"


-- Solution
solution "Handmade"
	language "C++"
	configurations { "Debug", "Release" }
	platforms { "Win64" }
	location "../_build"
    debugdir "../data"

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

    warnings "extra"
    disablewarnings {
        "4201", -- nameless struct
        "4100", -- unused parameter
        "4189", -- referenced but not used
    }

    linkoptions "/opt:ref"
    editandcontinue "off"

    --filter "vs*"
    rtti "off"
    exceptionhandling "off"

    configuration "Debug"
        defines { "_DEBUG" }
        flags { "FatalWarnings", "Maps" }
        symbols "on"

    configuration "Release"
        defines { "NDEBUG" }
        optimize "full"

    -- Projects
    project "handmade"
        language "C++"
        targetdir "../_bin/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}"
        objdir "../_obj/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}"
        kind "WindowedApp"
        files {
            "../code/**.h",
            "../code/**.cc",
            "../code/**.inl"
        }
        links {
        }
        includedirs {
            "../code",
        }
        -- postbuildcommands {
        --  "copy \"" .. path.translate(path.join(rootdir, "Data", "*.*")) .. '" "' ..
        --      path.translate(path.join(rootdir, "_Bin", "%{cfg.platform}", "%{cfg.buildcfg}", "%{prj.name}")) .. '"'
        -- }

        configuration "Win*"
            defines {
                "WIN32",
                "HANDMADE_WIN32=1",
                "HANDMADE_SLOW=1",
                "HANDMADE_INTERNAL=1",
            }
            flags { 
                "WinMain", 
                "StaticRuntime",
                "NoMinimalRebuild",
                "NoIncrementalLink",
            }


