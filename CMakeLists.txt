cmake_minimum_required(VERSION 3.15)

# Define our project with its name and version
project(MusicProductionApp VERSION 1.0.0)

# Use C++17 for modern features we'll need for audio processing
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake where to find JUCE
add_subdirectory(JUCE)

# Create our main application
juce_add_gui_app(MusicProductionApp
    PRODUCT_NAME "Music Production App"
    VERSION "1.0.0"
    COMPANY_NAME "TrinitySolutions"
    BUNDLE_ID "com.trinitysolutions.musicproductionapp"

    NEEDS_CURL FALSE
    NEEDS_WEB_BROWSER FALSE
    MICROPHONE_PERMISSION_ENABLED TRUE
    NEEDS_STORE_KIT FALSE
)

juce_generate_juce_header(MusicProductionApp)


# Add all our source files
target_sources(MusicProductionApp
    PRIVATE
        src/main.cpp
        src/audio/AudioEngine.cpp
        src/audio/MidiHandler.cpp
        src/audio/ChannelAudioProcessor.cpp
        src/gui/MainComponent.cpp
        src/gui/MixerPanel.cpp
        src/gui/SimpleLevelMeter.cpp
        src/gui/ChannelStrip.cpp
       
)

target_include_directories(MusicProductionApp
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/MusicProductionApp_artefacts/JuceLibraryCode
        ${CMAKE_SOURCE_DIR}/src/audio
)

# Link JUCE modules we need
target_link_libraries(MusicProductionApp
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
)