#version 330 

in vec3 vert;

uniform mat4 mvpMatrix;
uniform vec3 diffuse_color;
uniform vec3 light_location;

uniform vec4 sphere_center;
uniform bool picking;

smooth out vec3 frag_color;

void main()
{
    gl_Position = mvpMatrix * vec4(vert, 1.0);

    vec4 normal = gl_Position - sphere_center;    

    if(picking) {
        frag_color = diffuse_color;
    } else {
        vec3 light_direction = vert - light_location;
        float cos_theta = clamp(dot(light_direction, normal.xyz)/(length(light_direction)*length(normal.xyz)), 0.0, 1.0);
        frag_color = sqrt(cos_theta) * diffuse_color;
    }   

    // gl_Position = ftransform();
}
