include(FetchContent)


if (CMAKE_USE_HOST_SFML)
    find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)
else ()
    FetchContent_Declare(
            sfml
            GIT_REPOSITORY "https://github.com/SFML/SFML"
            GIT_TAG "2.6.1"
    )
    add_subdirectory(sfml)
endif ()
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG 527b2c45af2f8964f95826bd16ab7c7ed372ae41 # docking branch latest 04/07/24
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG 2.6.x
)
add_subdirectory(imgui-sfml)
