# Install script for directory: /Users/mothusit/Documents/C++/MusicProductionApp/JUCE

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/mothusit/Documents/C++/MusicProductionApp/build/JUCE/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/mothusit/Documents/C++/MusicProductionApp/build/JUCE/extras/Build/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-8.0.4" TYPE FILE FILES
    "/Users/mothusit/Documents/C++/MusicProductionApp/build/JUCE/JUCEConfigVersion.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/build/JUCE/JUCEConfig.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/JUCECheckAtomic.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/JUCEHelperTargets.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/JUCEModuleSupport.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/JUCEUtils.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/JuceLV2Defines.h.in"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/LaunchScreen.storyboard"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/PIPComponent.cpp.in"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/PIPConsole.cpp.in"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/checkBundleSigning.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/copyDir.cmake"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    "/Users/mothusit/Documents/C++/MusicProductionApp/JUCE/extras/Build/CMake/juce_LinuxSubprocessHelper.cpp"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/mothusit/Documents/C++/MusicProductionApp/build/JUCE/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
