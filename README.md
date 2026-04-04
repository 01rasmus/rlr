# RL Renderer
A graphics renderer written in C.

# Philosophy
The points this library tries to abide by are the following:
- Performance
- Portability
- Older and lower spec hardware
- Single header libraries only (This hopefully makes it easily Integratable)
- Simplicity (This is why only the gltf model format is supported)

# How to build
The library can mainly be built in two ways. Either with cmake or make.
It can also be built without any build system though. All the source files inside the ___lib___ and ___src___ folder can be built along side another project and it will compile fine as long as the system dependencies like X11 for Linux are linked.

# Libraries used
This library only depends on other single header libraries to try to minimize the complexity at compile time. They are as follows:
- [cgltf](https://github.com/jkuhlmann/cgltf)
- [glad](https://glad.dav1d.de/)
- [RGFW](https://github.com/ColleagueRiley/RGFW)
- [stb_ds.h](https://github.com/nothings/stb/blob/master/stb_ds.h)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)

# Features
- **SDF Fonts**<br>
It is used together with texture atlases generated from [Chlumsky's atlas gen](https://github.com/Chlumsky/msdf-atlas-gen). The engine loads in the .csv file with the glyph data, and then the texture atlas image. It uses [stb image](https://github.com/nothings/stb/blob/master/stb_image.h) to load the image. Currently only the ___MTSDF___ variant is implemented.