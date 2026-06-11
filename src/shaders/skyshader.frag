in vec3 vWorldDirection;

uniform vec3 uCameraWorldPos;
uniform vec3 uSunDirection = normalize(vec3(0.3, 0.5, -0.6));

out vec4 fragmentColor;

void main() {
  vec3 rayDir = normalize(vWorldDirection);

  // 1. Altitude calculation and atmosphere thickness
  // Scale it so that near-space altitude (e.g. 80,000 feet) feels thin and dark.
  float altitude = max(0.0, uCameraWorldPos.y);
  float altFactor = clamp(altitude / 80000.0, 0.0, 1.0);

  // 2. Base Sky Colors
  // Deep space-like color at zenith when high up, beautiful deep blue at low altitude.
  vec3 zenithColor = mix(vec3(0.12, 0.30, 0.65), vec3(0.005, 0.015, 0.04), altFactor);
  // Horizon color thins out at high altitude and fades to dark-ish grey-blue.
  vec3 horizonColor = mix(vec3(0.55, 0.75, 0.95), vec3(0.08, 0.15, 0.30), altFactor);
  
  // Ground/haze color
  vec3 groundColor = mix(vec3(0.2, 0.2, 0.22), vec3(0.05, 0.05, 0.06), altFactor);

  // 3. Sky Gradient calculation
  vec3 baseColor;
  if (rayDir.y >= 0.0) {
    // Sky: interpolate from horizon to zenith with a non-linear power curve.
    // At high altitudes, the horizon band should be sharper and thinner.
    float exponent = mix(2.5, 6.0, altFactor);
    float h = pow(1.0 - rayDir.y, exponent);
    baseColor = mix(zenithColor, horizonColor, h);
  } else {
    // Ground: interpolate from horizon to ground
    float h = pow(1.0 + rayDir.y, 2.0);
    baseColor = mix(groundColor, horizonColor, h);
  }

  // 4. Sun rendering
  vec3 sunDir = normalize(uSunDirection);
  float dotSun = dot(rayDir, sunDir);

  // Sun glow/corona
  float corona = pow(max(0.0, dotSun), 32.0) * mix(0.4, 0.05, altFactor);
  vec3 sunGlowColor = vec3(1.0, 0.95, 0.8) * corona;

  // Sharp sun disc with smoothstep edge anti-aliasing
  float sunDiscSize = 0.9995;
  float sunDisc = smoothstep(sunDiscSize, sunDiscSize + 0.0001, dotSun);
  vec3 sunDiscColor = vec3(1.0, 1.0, 0.95) * sunDisc * 5.0;

  // Combine sky background and sun elements
  vec3 finalColor = baseColor + sunGlowColor + sunDiscColor;

  // 5. Tone mapping and gamma correction to avoid harsh clipping
  finalColor = finalColor / (finalColor + vec3(1.0));
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  fragmentColor = vec4(finalColor, 1.0);
}
