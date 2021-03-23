-- A solution contains projects, and defines the available configurations
solution "DotsLandscapeSolution"
location "build"
targetdir "build/bin"
configurations { "Debug", "Release" }
      configuration "Debug"
         defines { "DEBUG","MYENGINE_PLATFORM_WINDOWS" }
         flags { "Symbols" }

      configuration "Release"
         defines { "NDEBUG","MYENGINE_PLATFORM_WINDOWS" }
         flags { "Optimize" }

   -- A project defines one build target
   project "DotsLandscape"
      kind "ConsoleApp"
      language "C++"
      files {
       "src/*.cpp" ,"src/*.cc","src/*.h",
       "src/*",
       "deps/SDL/include/*",
       "deps/SDL/src/*",
       "deps/SDL/src/atomic/*",
       "deps/SDL/src/audio/*",
       "deps/SDL/src/core/*",
       "deps/SDL/src/cpuinfo/*",
       "deps/SDL/src/dynapi/*",
       "deps/SDL/src/events/*",
       "deps/SDL/src/file/*",
       "deps/SDL/src/filesystem/*",
       "deps/SDL/src/haptic/*",
       "deps/SDL/src/joystick/*",
       "deps/SDL/src/libm/*",
       "deps/SDL/src/loadso/*",
       "deps/SDL/src/main/*",
       "deps/SDL/src/power/*",
       "deps/SDL/src/render/*",
       "deps/SDL/src/stdlib/*",
       "deps/SDL/src/test/*",
       "deps/SDL/src/thread/*",
       "deps/SDL/src/timer/*",
       "deps/SDL/src/video/*"
     }
      libdirs { "libs/SDL/" }
      links {"SDL2","SDL2main"}
    includedirs {"deps/SDL/include","deps/SDL/src","deps/SDL/src/*","deps"}
