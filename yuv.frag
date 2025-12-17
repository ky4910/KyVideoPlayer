#version 330 core

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

void main()
{
    float y = texture(texY, vUV).r;
    float u = texture(texU, vUV).r - 0.5;
    float v = texture(texV, vUV).r - 0.5;

    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;

    fragColor = vec4(r, g, b, 1.0);
}
