#version 330

// uniform vec3 frag_color;

uniform vec4 colour;

out vec4 finalColor;

void main()
{
    finalColor = vec4(colour);
    // gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
