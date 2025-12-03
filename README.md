#Graphics Experiments - DirectX 11, Luna 
A small collection of real-time rendering experiments built with DirectX 11 (Luna framework).
Originally created while learning low-level graphics programming, now kept as a reference and sandbox for future work.

The goal of this repo is to explore how modern rendering works under the hood: meshes, textures, shaders, lighting, and the GPU pipeline. These are small, self-contained demos rather than full projects.

#Contents
##Primitives
Experiments with basic geometry:
- Manual mesh creation
- Vertex buffers, index buffers
- Drawing cubes, spheres, and custom shapes

##Textures
Playing with texture sampling & material detail:
Texture loading
UV mapping
Filtering modes
Simple procedural textures

##Lighting
Exploring simple lighting models:
Per-vertex vs per-pixel lighting
Ambient, diffuse, specular
Point/Directional lights
Material parameters

##Shading
Small HLSL shader experiments:
Basic shading models
Normal mapping
Color manipulation
Misc GPU-side experiments

#Requirements
- Windows
- Visual Studio
- DirectX 11 SDK
- Luna framework

You can open any folder in Visual Studio and run the demo directly.

#Notes
This repo isn’t meant as production code — it’s a playground to learn and test graphics concepts.
I’m keeping it public so I can reference it and share examples when discussing rendering or teaching.
