## rl_render
A graphics renderer written in C.

## How to build
The library can mainly be built in two ways. Either with cmake or make.
It can also be built without any build system at all though. All the source files inside the ___lib___ and ___src___ folder can be built along side another project and it will compile fine as long as the system dependencies like X11 for Linux are linked.

## Libraries used
Defined below are the libraries used in the renderer. It only depends on other single header libraries to try to minimize the complexity at compile time.
- [cgltf](https://github.com/jkuhlmann/cgltf)
- [glad](https://glad.dav1d.de/)
- [RGFW](https://github.com/ColleagueRiley/RGFW)
- [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)

## Features
- **SDF Fonts**<br>
It is used together with texture atlases generated from [Chlumsky's atlas gen](https://github.com/Chlumsky/msdf-atlas-gen). The engine loads in the .csv file with the glyph data, and then the texture atlas image. It uses [stb image](https://github.com/nothings/stb/blob/master/stb_image.h) to load the image. Currently only the ___MTSDF___ variant is implemented.

## Dependencies
For a computer to run the renderer an OpenGL 3.3 compatible graphics card is required.

The following flags are needed to compile the library.
| Operating System | Linker flags                                                                 |
| ---------------- | ---------------------------------------------------------------------------- |
| Linux            | ``-lX11 -lGL -lXrandr -lm``                                                  |
| Windows          | ``-lopengl32 -lgdi32``                                                       |
| macOS            | ``-framework Cocoa -framework CoreVideo -framework OpenGL -framework IOKit`` |

On linux you need to install some dependencies to build. On debian this command will download all the required packages:<br>
``sudo apt install libx11-dev libgl1-mesa-dev libxrandr-dev build-essential``