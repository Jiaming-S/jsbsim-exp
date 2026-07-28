in vec3 vWorldDirection;

uniform vec3 uCameraWorldPos;
uniform vec3 uSunDirection;

out vec4 fragmentColor;

void main() {
  vec3 rayDir = normalize(vWorldDirection);

  // Simple, clean sky gradient
  // Zenith (top): Deep blue
  // Horizon: Very light sky blue
  // Nadir (bottom): Neutral ground color/horizon transition
  vec3 zenithColor = vec3(0.18, 0.38, 0.68);
  vec3 horizonColor = vec3(0.70, 0.82, 0.94);
  vec3 nadirColor = vec3(0.55, 0.62, 0.68);

  vec3 baseColor;
  if (rayDir.y >= 0.0) {
    float h = pow(rayDir.y, 0.7);
    baseColor = mix(horizonColor, zenithColor, h);
  } else {
    float h = pow(-rayDir.y, 0.7);
    baseColor = mix(horizonColor, nadirColor, h);
  }

  // Soft sun glow
  vec3 sunDir = normalize(uSunDirection);
  float dotSun = dot(rayDir, sunDir);
  vec3 sunGlow = vec3(0.0);
  if (dotSun > 0.0) {
    float glow = pow(dotSun, 8.0) * 0.15;
    glow += pow(dotSun, 32.0) * 0.15;
    sunGlow = vec3(1.0, 0.95, 0.85) * glow;
  }

  vec3 finalColor = baseColor + sunGlow;

  // Gamma correction
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  fragmentColor = vec4(finalColor, 1.0);
}
