# Lighting-and-Shading
In programmable OpenGL, lighting computations are done entirely in the shaders. In this project we:

* Illuminate the scene by adding a point light source.
* Insert objects (cube/sphere).
* Compute per-vertex/per-fragment normals to the objects, to be used for lighting computations.
* Implement and compare the following shading techniques:
	* Gouraud shading for diffuse lighting.
	* Phong lighting -- by computing per-vertex normals and all three lighting components: ambient, diffuse, and specular (<em><span>&#8706;</span></em> = 64).
	* Phong shading -- by computing per-fragment normals and all three lighting components: ambient, diffuse, and specular (<em><span>&#8706;</span></em> = 64).

## Results
1. Illuminate the scene by adding a point light source and objects (without shading):
		Cube             |  Sphere
	  :-------------------------:|:-------------------------:
	  ![cube_image](results/cube.png)  |  ![sphere_image](results/sphere.png)

2. Gouraud shading (diffuse lighting):
![gouraud](results/gouraud.png)

3. Phong lighting (per-vertex):
![phong_vertex](results/phong_vertex.png)

3. Phong Shading (per-fragment):
![phong_fragment](results/phong_fragment.png)

## Execution
* Generate the object file:
	* `mkdir build && cd build`
	* `cmake ..\`
	* `make`
	* `cd ../`
* Execute the object file:
	* `./bin/lighting_and_shading`