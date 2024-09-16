add_executable(Fluid_Sim main.cpp)
target_link_libraries(Fluid_Sim PRIVATE ImGui-SFML::ImGui-SFML)
set(CMAKE_CXX_FLAGS "-Ofast")

## we need the ability to build for android devices
#if (ANDROID)
#    add_library(BPHOComputationalChallenge SHARED main.cpp)
#    target_link_libraries(BPHOComputationalChallenge PRIVATE ImGui-SFML::ImGui-SFML)
#endif ()

if(WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET BPHOComputationalChallenge POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
                $<TARGET_FILE:sfml-graphics>
                $<TARGET_FILE:sfml-window>
                $<TARGET_FILE:sfml-system>
                $<TARGET_FILE_DIR:BPHOComputationalChallenge>)
    endif()
endif()

#add_custom_command(TARGET BPHOComputationalChallenge POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/resources
#        $<TARGET_FILE_DIR:BPHOComputationalChallenge>/resources)
