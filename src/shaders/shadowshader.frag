in vec2 vTextureCoords;

uniform vec4 uColor;
uniform float uOpacity;

out vec4 fragmentColor;

void main() {
    // Texture coordinates range from 0.0 to 1.0. Center is (0.5, 0.5)
    vec2 center = vec2(0.5, 0.5);
    float dist = length(vTextureCoords - center);
    
    // Smooth boundary for a soft circular shadow
    // Inside 0.43, it is fully solid (1.0).
    // Outside 0.50, it is fully transparent (0.0).
    float mask = smoothstep(0.50, 0.43, dist);
    
    fragmentColor = vec4(uColor.rgb, uColor.a * mask * uOpacity);
}
