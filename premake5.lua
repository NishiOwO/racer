flags("MultiProcessorCompile")

workspace("racer")
        configurations({
                "Debug",
                "Release"
        })
        platforms({
                "Native",
                "Win32",
                "Win64"
        })
        defaultplatform("Native")

filter("platforms:Win32")
        system("windows")
        architecture("x86")
        gccprefix("i686-w64-mingw32-")

filter("platforms:Win64")
        system("windows")
        architecture("x86_64")
        gccprefix("x86_64-w64-mingw32-")

filter({})

include("raknet")

project("racer-server")
	kind("ConsoleApp")
	targetdir("bin/%{cfg.buildcfg}/%{cfg.platform}")
	includedirs({
		"include",
		"raknet/include"
	})

	defines("RACER_SERVER")

	filter("platforms:Native")
		buildoptions("`ode-config --cflags`")
		linkoptions("`ode-config --libs`")
	filter("platforms:not Native")
		links("ode")
	filter({})

	links({
		"raknet",
		"stdc++"
	})
	filter("system:windows")
		links("ws2_32")
	filter("system:not windows")
		links("pthread")
	filter({})
	files({
		"server/*.c",
		"server/*.cpp"
	})

project("racer-client")
	kind("ConsoleApp")
	targetdir("bin/%{cfg.buildcfg}/%{cfg.platform}")
	includedirs({
		"include",
		"raknet/include"
	})

	defines("RACER_CLIENT")

	includedirs("/usr/X11R7/include")
	libdirs("/usr/X11R7/lib")
	links({
		"raknet",
		"stdc++"
	})
	filter("system:windows")
		links({"ws2_32", "opengl32", "glu32", "gdi32"})
	filter("system:not windows")
		links({"pthread", "GL", "GLU", "m"})
	filter({})
	files({
		"client/*.c",
		"client/*.cpp"
	})
