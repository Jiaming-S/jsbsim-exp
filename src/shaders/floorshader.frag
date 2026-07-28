in vec3 vWorldDirection;

uniform vec3 uCameraWorldPos;
uniform float uGridSpacing;
uniform float uSnapInterval;

out vec4 fragmentColor;

// Simple 2D hash
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// 2D Value Noise
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0.0,0.0)), hash(i + vec2(1.0,0.0)), u.x),
               mix(hash(i + vec2(0.0,1.0)), hash(i + vec2(1.0,1.0)), u.x), u.y);
}

// 3-octave FBM for sand dunes
float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    mat2 rot = mat2(0.8, 0.6, -0.6, 0.8);
    for (int i = 0; i < 3; ++i) {
        v += a * noise(p);
        p = rot * p * 2.0 + vec2(10.0);
        a *= 0.5;
    }
    return v;
}

// Anti-aliased grid line helper
float gridVal(vec2 p, float lineWidth) {
    vec2 dd = fwidth(p);
    vec2 grid = abs(fract(p - 0.5) - 0.5) / dd;
    vec2 line = smoothstep(lineWidth + 0.5, lineWidth - 0.5, grid);
    return max(line.x, line.y);
}

void main() {
    vec3 rayDir = normalize(vWorldDirection);

    // Ray-plane intersection with y = 0
    if (abs(rayDir.y) < 1e-6) { discard; }
    
    float t = -uCameraWorldPos.y / rayDir.y;
    if (t < 0.0) { discard; }

    vec3 worldPos = uCameraWorldPos + rayDir * t;

    // Calculate horizontal distance from camera to the intersection point
    float d = length(worldPos.xz - uCameraWorldPos.xz);

    // Desert Sand Colors
    vec3 sandDark = vec3(0.78, 0.64, 0.46);
    vec3 sandLight = vec3(0.90, 0.80, 0.65);

    // Base sand color with large scale dunes (FBM)
    // Increased frequency from 0.0003 to 0.002 (one cycle every 5 grid squares)
    float duneNoise = fbm(worldPos.xz * 0.002);
    vec3 baseColor = mix(sandDark, sandLight, duneNoise);

    // Wind-blown sand ripples (medium frequency)
    float ripple = sin(worldPos.x * 0.06 + sin(worldPos.z * 0.03) * 2.0);
    ripple = ripple * 0.5 + 0.5;
    
    // Fade ripples in the distance to prevent aliasing
    float rippleFade = clamp(1.0 - d / 6000.0, 0.0, 1.0);
    baseColor = mix(baseColor, baseColor * 0.88, ripple * rippleFade);

    // Fine grain for close-up detail
    float grain = noise(worldPos.xz * 1.5) * 0.04;
    float grainFade = clamp(1.0 - d / 1500.0, 0.0, 1.0);
    baseColor += vec3(grain * grainFade);

    // Grids: minor and major
    // Minor grid every uGridSpacing units (e.g. 100m)
    // Major grid every 10 * uGridSpacing units (e.g. 1000m)
    vec2 uvMinor = worldPos.xz / uGridSpacing;
    vec2 uvMajor = worldPos.xz / (uGridSpacing * 10.0);

    float minorGrid = gridVal(uvMinor, 1.0);
    float majorGrid = gridVal(uvMajor, 1.5);

    // Fade factors based on horizontal distance
    float minorGridFade = clamp(1.0 - d / 12000.0, 0.0, 1.0);
    float majorGridFade = clamp(1.0 - d / 40000.0, 0.0, 1.0);
    float groundAlpha = clamp(1.0 - d / 80000.0, 0.0, 1.0);

    // Grid Colors
    vec3 minorLineColor = vec3(0.66, 0.52, 0.35); // darker brown for grid lines
    vec3 majorLineColor = vec3(0.52, 0.38, 0.22); // even darker for major grid lines

    // Apply grid lines with fading
    vec3 finalColor = mix(baseColor, minorLineColor, minorGrid * 0.4 * minorGridFade);
    finalColor = mix(finalColor, majorLineColor, majorGrid * 0.6 * majorGridFade);

    // Discard if completely faded out
    if (groundAlpha < 0.001) { discard; }

    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    fragmentColor = vec4(finalColor, groundAlpha);
}

