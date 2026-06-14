in vec3 vWorldDirection;

uniform vec3 uCameraWorldPos;
uniform vec3 uSunDirection = normalize(vec3(0.3, 0.5, -0.6));

out vec4 fragmentColor;

// Pseudo-random function for stars
float hash(vec3 p) {
    p  = fract(p * 0.1031);
    p += dot(p, p.zyx + 31.32);
    return fract((p.x + p.y) * p.z);
}

void main() {
  vec3 rayDir = normalize(vWorldDirection);

  // 1. Altitude calculation and atmosphere thickness
  float altitude = max(0.0, uCameraWorldPos.y);
  float altFactor = clamp(altitude / 100000.0, 0.0, 1.0); // 100k ft for near space

  // 2. Base Sky Colors
  // More dramatic colors: deeper zenith, more vibrant horizon
  vec3 zenithColor = mix(vec3(0.05, 0.15, 0.45), vec3(0.002, 0.005, 0.01), altFactor);
  vec3 horizonColor = mix(vec3(0.4, 0.7, 1.0), vec3(0.05, 0.1, 0.2), altFactor);
  vec3 groundColor = mix(vec3(0.1, 0.1, 0.12), vec3(0.02, 0.02, 0.03), altFactor);

  // 3. Sky Gradient calculation
  vec3 baseColor;
  if (rayDir.y >= 0.0) {
    float exponent = mix(3.0, 8.0, altFactor);
    float h = pow(1.0 - rayDir.y, exponent);
    baseColor = mix(zenithColor, horizonColor, h);
    
    // Add a bit of "space" purple/blue tint at very high altitudes
    baseColor += mix(vec3(0.0), vec3(0.01, 0.0, 0.02), pow(altFactor, 2.0));
  } else {
    float h = pow(1.0 + rayDir.y, 1.5);
    baseColor = mix(groundColor, horizonColor, h);
  }

  // 4. Stars (visible at high altitudes or looking away from sun)
  vec3 starColor = vec3(0.0);
  if (altFactor > 0.2 && rayDir.y > -0.1) {
    float starDensity = mix(0.0, 0.0005, pow(altFactor, 2.0));
    vec3 starPos = rayDir * 1000.0;
    float n = hash(floor(starPos * 2.0));
    if (n > 1.0 - starDensity) {
      float brightness = pow(hash(floor(starPos * 2.0) + 0.5), 10.0);
      starColor = vec3(brightness) * (1.0 - altFactor * 0.5); // Faintly colored stars
    }
  }

  // 5. Sun rendering
  vec3 sunDir = normalize(uSunDirection);
  float dotSun = dot(rayDir, sunDir);

  // Sun glow/corona - more dramatic
  float corona = pow(max(0.0, dotSun), 12.0) * mix(0.5, 0.1, altFactor);
  corona += pow(max(0.0, dotSun), 128.0) * 2.0; // Tighter core glow
  vec3 sunGlowColor = vec3(1.0, 0.9, 0.7) * corona;

  // Sharp sun disc
  float sunDiscSize = 0.9998;
  float sunDisc = smoothstep(sunDiscSize, sunDiscSize + 0.0001, dotSun);
  vec3 sunDiscColor = vec3(1.0, 1.0, 0.98) * sunDisc * 10.0;

  // 6. Combine all elements
  vec3 finalColor = baseColor + starColor + sunGlowColor + sunDiscColor;

  // 7. Tone mapping and gamma correction
  // Exposure adjustment
  finalColor *= 1.2;
  finalColor = finalColor / (finalColor + vec3(1.0));
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  fragmentColor = vec4(finalColor, 1.0);
}
