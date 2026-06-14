in vec3 vWorldDirection;

uniform vec3 uCameraWorldPos;
uniform float uGridSpacing;
uniform float uSnapInterval;

uniform vec3 uGridColor = vec3(0.4, 0.4, 0.4);
uniform float uPlusSize = 4.0;

out vec4 fragmentColor;

vec4 getGridColor(vec3 worldIntersection, float t) {
  vec2 gridSpace = fract(worldIntersection.xz / uGridSpacing);
  vec2 cellPos = gridSpace - 0.5;

  bool horiz = abs(cellPos.x) < (uPlusSize / uGridSpacing) && 
               abs(cellPos.y) < (uPlusSize / uGridSpacing * 0.05);

  bool vert = abs(cellPos.x) < (uPlusSize / uGridSpacing * 0.05) && 
              abs(cellPos.y) < (uPlusSize / uGridSpacing);

  if (horiz || vert) {
    float fogFactor = exp(-0.001 * t);
    return vec4(uGridColor * fogFactor, fogFactor);
  }
  return vec4(0.0);
}

void main() {
  vec3 rayDir = normalize(vWorldDirection);

  if(abs(rayDir.y) < 1e-6) { discard; } 

  float snapOffset = uSnapInterval * 0.1;
  float relY = uCameraWorldPos.y - snapOffset;
  float f = fract(relY / uSnapInterval);

  float y1 = floor(relY / uSnapInterval) * uSnapInterval;
  float y2 = y1 + uSnapInterval;

  y1 = max(0.0, y1);
  y2 = max(0.0, y2);

  vec4 color1 = vec4(0.0);
  float t1 = (y1 - uCameraWorldPos.y) / rayDir.y;
  if(t1 > 0.0) {
    color1 = getGridColor(uCameraWorldPos + rayDir * t1, t1);
  }

  vec4 color2 = vec4(0.0);
  float t2 = (y2 - uCameraWorldPos.y) / rayDir.y;
  if(t2 > 0.0) {
    color2 = getGridColor(uCameraWorldPos + rayDir * t2, t2);
  }

  vec4 finalColor = color1 * (1.0 - f) + color2 * f;
  if (finalColor.a < 0.001) { discard; }

  fragmentColor = finalColor;
}

