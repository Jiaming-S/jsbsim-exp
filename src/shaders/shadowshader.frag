in vec2 vTextureCoords;

uniform vec4 uColor;
uniform float uOpacity;

out vec4 fragmentColor;

void main() {
    // Texture coordinates range from 0.0 to 1.0. Center is (0.5, 0.5)
    vec2 center = vec2(0.5, 0.5);
    float dist = length(vTextureCoords - center);
    
    // Dynamic soft boundary: shadow gets blurrier as it fades (uOpacity decreases)
    float innerRadius = 0.40 * uOpacity;
    float mask = smoothstep(0.50, innerRadius, dist);
    
    fragmentColor = vec4(uColor.rgb, uColor.a * mask * uOpacity);
}
