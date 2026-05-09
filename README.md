<h1 align="center">RL Render 👾</h1>
<p align="center">A simplistic graphics renderer written for simplistic games</p>

## Features
- **Multiple Backends**<br>
The renderer chooses either ___OpenGL 3.3___, ___OpenGL ES 3.0___ or ___DirectX 10___ at runtime depending on which of these three are supported by the system. This is fully dynamic.
- **Custom Shader Language**<br>
[RLSL](https://github.com/rasu01/rlsl) is a shading language specifically written for RL Render. Since there are multiple backends a custom shader language makes sure a single shader can be used for any backend. It is very similar to [glsl](https://wikis.khronos.org/opengl/OpenGL_Shading_Language) so it should feel familiar.
- **SDF Fonts**<br>
It is used together with texture atlases generated from [Chlumsky's atlas gen](https://github.com/Chlumsky/msdf-atlas-gen). The engine loads in the .csv file with the glyph data, and then the texture atlas image. It uses [stb image](https://github.com/nothings/stb/blob/master/stb_image.h) to load the image. Currently only the ___MTSDF___ variant is implemented.

## Libraries used
Defined below are the libraries used in the renderer. It mostly depends on other single header libraries to try to minimize the complexity at compile time.
- [cgltf](https://github.com/jkuhlmann/cgltf)
- [glad](https://gen.glad.sh/)
- [glfw](https://github.com/glfw/glfw)
- [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)

## Dependencies
For a computer to run the renderer an OpenGL 3.3, OpenGL ES 3.0 or DirectX 10 compatible graphics card is required.

On linux you need to install some dependencies to build. On debian this command will download all the required packages:<br>
``sudo apt install libx11-dev libgl1-mesa-dev libxrandr-dev build-essential``

## How to build
The library can mainly be built in two ways. Either with cmake or make.