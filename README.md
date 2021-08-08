# Lighting-and-Shading
Illuminating the scene with a point light source and implementing & comparing different lighting computation methods in OpenGL.

In this project we:
* Illuminate the scene by adding a point light source.
* Insert objects (cube/sphere).
* Compute per-vertex/per-fragment normals to the objects, to be used for lighting computations.
* Implement and compare the following shading techniques:
	* Gouraud shading for diffuse lighting.
	* Phong lighting -- by computing per-vertex normals and all three lighting components: ambient, diffuse, and specular (<em><span>&alpha;</span></em> = 64).
	* Phong shading -- by computing per-fragment normals and all three lighting components: ambient, diffuse, and specular (<em><span>&alpha;</span></em> = 64).

## Results
1. Illuminate the scene by adding a point light source and objects (without shading):
		Cube             |  Sphere
	  :-------------------------:|:-------------------------:
	  ![cube_image](results/cube.png)  |  ![sphere_image](results/sphere.png)

2. Gouraud shading (diffuse lighting):
<img src="results/gouraud.png" width="50%">

3. Phong lighting (per-vertex):
<img src="results/phong_vertex.png" width="60%">

3. Phong Shading (per-fragment):
<img src="results/phong_fragment.png" width="60%">

## Execution
* Generate the object file:
	* `mkdir build && cd build`
	* `cmake ..\`
	* `make`
	* `cd ../`
* Execute the object file:
	* `./bin/lighting_and_shading`
