#version 330

layout (location = 0)in vec3 vVertex;
layout (location = 1)in vec3 vColor;
layout (location = 2)in vec3 vNormal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

uniform vec3 lightSourceColor;
uniform vec3 lightSourcePos;
uniform vec3 cameraPos;

out vec3 fColor;

// fixed point light properties
vec3 position_eye, normal_eye;

float specular_exponent = 64.0; // specular 'power'

void main () {
	position_eye = vec3 (vView * vModel * vec4 (vVertex, 1.0));
	// normal_eye = vec3 (vView * vModel * vec4 (vNormal, 0.0));
	normal_eye = vec3 (vView * (transpose(inverse(vModel))) * vec4 (vNormal, 0.0));

	gl_Position = vProjection * vec4 (position_eye, 1.0);
	// ambient intensity
	vec3 Ia = 0.2*lightSourceColor;

	vec3 n_eye = normalize( normal_eye );

	// diffuse intensity
	vec3 light_position_eye = vec3 (vView * vec4 (lightSourcePos, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize (distance_to_light_eye);
	float dot_prod = dot (direction_to_light_eye, n_eye);
	dot_prod = max (dot_prod, 0.0);
	vec3 Id = 0.7 * lightSourceColor * dot_prod; // final diffuse intensity
	
	// specular intensity
	vec3 surface_to_viewer_eye = normalize (-position_eye);
	vec3 half_way_eye = normalize (surface_to_viewer_eye + direction_to_light_eye);
	float dot_prod_specular = max (dot (half_way_eye, n_eye), 0.0);
	float specular_factor = pow (dot_prod_specular, specular_exponent);
	
	vec3 Is = 1.0 * lightSourceColor * specular_factor; // final specular intensity
	
	// final colour
	fColor = (Is + Id + Ia)* vColor;
	// fColor = (Ia + Id) * vColor;
	// fColor = vColor;
}
