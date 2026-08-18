#!/bin/bash
OPENGL_RENDERER=OFF
BUILTIN_SHADER_COMPILER=OFF
BUILD_TYPE="Debug"

WEBGPU_RENDERER=ON
WEBGPU_BACKEND=WGPU_STATIC

COMPILE_SHADERS=0
BUILD=1

if [[ -z "$@" ]]; then
    DEFAULT=1
fi

for i in "$@"; do
    case $i in
        -S|--shaders)
            COMPILE_SHADERS=1
        ;;
        -B|--build)
            BUILD=1
        ;;
        --gl)
            OPENGL_RENDERER=ON
        ;;
        --shaderc)
            BUILTIN_SHADER_COMPILER=ON
        ;;
        --release)
            BUILD_TYPE="Release"
        ;;
        -G|--build-game)
            BUILD_GAME=1
        ;;
        -E|--build-editor)
            BUILD_EDITOR=1
        ;;
        --build-all)
            BUILD_GAME=1
            BUILD_EDITOR=1
        ;;
        # -t=*|--target=*)
        # TARGET="${i#*=}"
        # ;;
    esac
done

compile_builtin_shaders() {
    for filename in VorliasEngine/src/Shaders/*; do
        if ! [[ $filename =~ \.(frag|vert)$ ]]; then
            continue
        fi

        basename=$(basename $filename)
        dirname=$(dirname $filename)
        outputName="$dirname/$basename.spv"
        headerName="$dirname/$basename.h"

        glslang -V $filename -o $outputName
        # xxd -n "$basename" -i $outputName > $headerName
    done
}

generate_project() {
    for i in "$@"; do
        case $i in
            --shaderc=*)
            BUILTIN_SHADER_COMPILER="${i#*=}"
            ;;
            --internal)
            ANDROMEDA_INTERNAL=1
            ;;
        esac
    done

    cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DOPENGL_RENDERER=$OPENGL_RENDERER \
        -DWEBGPU_RENDERER=$WEBGPU_RENDERER \
        -DBUILTIN_SHADER_COMPILER=$BUILTIN_SHADER_COMPILER \
        -DLUAU_BUILD_CLI=OFF -DLUAU_BUILD_TESTS=OFF -DANDROMEDA_INTERNAL=$ANDROMEDA_INTERNAL \
        -DWEBGPU_BACKEND=$WEBGPU_BACKEND
}

# At some point will inline the shaders, but we can compile this via the application itself in debug
if [[ $COMPILE_SHADERS == 1 ]]; then
    compile_builtin_shaders
fi

if [[ $DEFAULT ]]; then
    BUILD_GAME=1
fi

if [[ -f "Tools/lute" ]]; then
    ./Tools/lute ./scripts/atoms.luau
fi

if [[ $BUILD_GAME == 1 ]]; then
    generate_project #--shaderc=OFF # temporarily disabled while i figure out the vulkan stuff
    cmake --build build --target VorliasGame --config $BUILD_TYPE

    if [[ $? != 0 ]]; then
        echo -e "\e[31m""Failed Building Game, see above.""\e[0m"
        exit 1
    fi
fi

if [[ $BUILD_EDITOR == 1 ]]; then
    generate_project --shaderc=ON
    cmake --build build --target VorliasEditor --config $BUILD_TYPE
fi