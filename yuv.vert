#version 330 core

out vec2 vUV;
uniform vec2 uScale;

void main()
{
    // Fullscreen quad
    vec2 pos[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2(-1.0,  1.0),
        vec2( 1.0,  1.0)
    );

    vec2 uv[4] = vec2[](
        vec2(0.0, 0.0),  // left bottom
        vec2(1.0, 0.0),  // right bottom
        vec2(0.0, 1.0),  // left top
        vec2(1.0, 1.0)   // right top
    );

    gl_Position = vec4(pos[gl_VertexID] * uScale, 0.0, 1.0);
    vUV = uv[gl_VertexID];
}
