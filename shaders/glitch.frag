#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    vec2 uv = TexCoords;
    
    // Time-based glitch effect
    float glitchStrength = 0.05;
    float glitchFrequency = 10.0;
    float t = floor(time * glitchFrequency);
    
    // Horizontal glitch
    if (random(vec2(t, 0.0)) > 0.8) {
        uv.x += glitchStrength * (random(vec2(t, uv.y)) - 0.5);
    }
    
    // Vertical glitch
    if (random(vec2(t, 1.0)) > 0.8) {
        uv.y += glitchStrength * (random(vec2(t, uv.x)) - 0.5);
    }
    
    // Color shift
    vec2 redOffset = vec2(random(vec2(t, 2.0)) - 0.5, random(vec2(t, 3.0)) - 0.5) * 0.02;
    vec2 greenOffset = vec2(random(vec2(t, 4.0)) - 0.5, random(vec2(t, 5.0)) - 0.5) * 0.02;
    vec2 blueOffset = vec2(random(vec2(t, 6.0)) - 0.5, random(vec2(t, 7.0)) - 0.5) * 0.02;
    
    vec4 color;
    color.r = texture(screenTexture, uv + redOffset).r;
    color.g = texture(screenTexture, uv + greenOffset).g;
    color.b = texture(screenTexture, uv + blueOffset).b;
    color.a = 1.0;
    
    // Scanline effect
    float scanline = sin(uv.y * 800.0) * 0.06;
    color.rgb += vec3(scanline);
    
    FragColor = color;
}