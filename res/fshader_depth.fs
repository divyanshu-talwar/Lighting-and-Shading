#version 330 core
out vec4 FragColor;

float near = 0.1; 
float far  = 1000.0; 
  
float LinearizeDepth(float depth) // reverse the projection matrix multiplication for the z - coordinate.
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / 1000.0; 
    // divided by a constant for the depth map to be displayed on the screen.
    FragColor = vec4(vec3(depth), 1.0);
}