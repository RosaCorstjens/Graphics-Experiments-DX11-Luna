# Graphics Experiments - DirectX 11, Luna 
A collection of small real-time rendering experiments built with DirectX 11 (Luna framework). The goal of this repo is to have a sandbox to explore how modern rendering works under the hood: meshes, textures, shaders, lighting, and the GPU pipeline. Each project is self-contained and showcases a specific graphics concept or workflow.

---

## Contents

### [PyramidGenerator](./PyramidGenerator)
A series of experiments exploring how to render and manipulate a pyramid mesh with increasing complexity. Demonstrates procedural mesh generation, parameter manipulation and geometry shader usage. 
- **Basic**: manually builds and renders a parameterized pyramid. 
- **Interactive**: uses inputs to change parameters used to render the pyramid (height, radius, amount of sides).
- **GeometryShader**: same setup but generated and expanded using a geometry shader. 

<p float="left">
  <img src="PyramidGenerator/Screenshots/pyramid-top.png" width="300" />
  <img src="PyramidGenerator/Screenshots/pyramid-bottom.png" width="300" />
</p>

### [DynamicTexturedPhone](./DynamicTexturedPhone)
Experiments in texturing a phone-like mesh, progressing from static to dynamic render-to-texture workflows. Explores multi-texturing, UV mapping, and render-to-texture techniques on a small mesh.
- **TexturedPhone**: generates and renders a mesh that looks like a phone, textured with basic uv mapping using pictures of all sides of a real phone.   
- **ScreenUV**: applies a second texture with multi-texturing and custom UVs, which is mapped to the phone screen as if the phone shows a picture.  
- **CameraRenderTexture**: renders the scene to a texture displayed on the phone screen, using render-to-texture techniques.

<p float="left">
  <img src="DynamicTexturedPhone/Screenshots/textured-phone.png" width="300" />
  <img src="DynamicTexturedPhone/Screenshots/textured-phone-screen.png" width="300" />
  <img src="DynamicTexturedPhone/Screenshots/textured-phone-render-texture-screen.png" width="300" />
</p>

### [Fresnel](./Fresnel)
Custom implementation of a fresnel shader, calculating the pixel color based on the normal of the pixel and the camera and parameters to control colors and intensities.

<p float="left">
  <img src="Fresnel/Screenshots/black-and-white.png" width="300" />
  <img src="Fresnel/Screenshots/colorful.png" width="300" />
</p>

### [LightingExperiments](./LightingExperiments)
A collection of small interactive lighting experiments, exploring basic functionality of point and spot lights. 
- **PointLightWand**: simulates a wand emitting a controllable point light, inspired by "Lumos". 
- **SpotLightLaser**: a spotlight attached to the camera shines on the environment much like a laser.
- **PointLightUnderwater**: simulates a point light on the bottom of a body of water. Uses a lightmap to simulate water distortion effects.    

<p float="left">
  <img src="LightingExperiments/Screenshots/point-light-wand.png" width="300" />
  <img src="LightingExperiments/Screenshots/spot-light-laser.png" width="300" />
  <img src="LightingExperiments/Screenshots/point-light-underwater.png" width="300" />
</p>

### [ReflectiveChrome](./ReflectiveChrome)
Custom implementation of a real-time reflective surface using dynamic cube mapping, with dynamic reflectiveness based on user inputs. 

<p float="left">
  <img src="ReflectiveChrome/Screenshots/reflective-chrome-mat.png" width="300" />
  <img src="ReflectiveChrome/Screenshots/reflective-chrome-shiny.png" width="300" />
</p>

### [TranslateShadertoyToDX11](./TranslateShadertoyToDX11)
An excersise in translating ShaderToy shaders to DirectX 11. Shader [isovalues 2](https://www.shadertoy.com/view/MdfcRS) was translated from GLSL to HLSL, taking the development environment into account.  

<p float="left">
  <img src="TranslateShadertoyToDX11/Screenshots/colorful-noise.png" width="300" />
</p>

---

## How to Run

These are Win32 C++ DirectX 11 projects built with the Luna framework.  
To run them locally, you need:
- Windows 10+  
- Visual Studio with “Desktop development with C++” workload  
- Luna framework installed (headers + libraries)  
- DirectX 11 SDK 
- Project assets (textures, meshes, shaders)

Open the project folder in Visual Studio, configure include/lib paths for Luna and DirectX if needed, then build and run.

---

## Notes

- Screenshots are provided for quick previews. For full interaction, run the demos
- This repository is used as a sandbox environment to expirement with real-time graphics programming, to showcase my work and to reference examples in conversation or lectures. 

