<h1 align="center">RL Render 👾</h1>
<p align="center">A simplistic graphics renderer written for simplistic games</p>

## Features
- **Multiple Backends**<br>
The renderer chooses either ___OpenGL 3.3___, ___OpenGL ES 3.0___ or ___DirectX 10___ at runtime depending on which of these three are supported by the system. This is fully dynamic.
- **Custom Shader Language**<br>
[RLSL](https://github.com/rasu01/rlsl) is a shading language specifically written for RL Render. Since there are multiple backends a custom shader language makes sure a single shader can be used for any backend.
- **SDF Fonts**<br>
It is used together with texture atlases generated from [Chlumsky's atlas gen](https://github.com/Chlumsky/msdf-atlas-gen). The engine loads in the .csv file with the glyph data, and then the texture atlas image. It uses [stb image](https://github.com/nothings/stb/blob/master/stb_image.h) to load the image. Only the ___MTSDF___ variant is supported.

## Libraries used
Defined below are the libraries used in the renderer. It mostly depends on other single header libraries to try to minimize the complexity at compile time.
- [cgltf](https://github.com/jkuhlmann/cgltf)
- [glad](https://gen.glad.sh/)
- [glfw](https://github.com/glfw/glfw)
- [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)
- [stb_image_resize2.h](https://github.com/nothings/stb/blob/master/stb_image_resize2.h)
- [rlsl](https://github.com/rasu01/rlsl)

## Dependencies
For a computer to run the renderer an OpenGL 3.3, OpenGL ES 3.0 or DirectX 10 compatible graphics card is required.

On linux you need to install some dependencies to build. On debian this command will download all the required packages:<br>
``sudo apt install cmake gcc pkg-config libgl1-mesa-dev libwayland-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev``

## How to build
The library can mainly be built in two ways. Either with cmake or make.

## How to generate a font
Using [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen) the folliwing is a sample command
```
msdf-atlas-gen -font font.ttf -type mtsdf -format png -pots -pxrange 2 -imageout font.png -csv font.csv -charset charset.txt
```
The important thing is that the output is a csv file and that the output uses unicode codepoints(which it does if -charset is declared). [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) is used to load the image so any image format that is supported by it could be used. The pxrange should be noted, since it is used when loading the font(and used when rendering).

The `charset.txt` must include the unicode points that should be in the final font atlas/csv.
To just get all of them, `charset.txt` could include the follow:
```
[0x0, 0xFFFF]
```