in vec3 vWorldDirection;

uniform vec3 uCameraWorldPos;
uniform float uGridSpacing;

uniform vec3 uGridColor = vec3(0.4, 0.4, 0.4);
uniform float uPlusSize = 4.0;

out vec4 fragmentColor;

void main() {
  vec3 rayDir = normalize(vWorldDirection);
  
  if(abs(rayDir.y) < 1e-6) { discard; } 

  float t = -uCameraWorldPos.y / rayDir.y;
  if(t < 0.0) { discard; }

  vec3 worldIntersection = uCameraWorldPos + rayDir * t;
  
  vec2 gridSpace = fract(worldIntersection.xz / uGridSpacing);
  vec2 cellPos = gridSpace - 0.5;

  bool horiz = abs(cellPos.x) < (uPlusSize / uGridSpacing) && 
               abs(cellPos.y) < (uPlusSize / uGridSpacing * 0.05);

  bool vert = abs(cellPos.x) < (uPlusSize / uGridSpacing * 0.05) && 
              abs(cellPos.y) < (uPlusSize / uGridSpacing);

  if (horiz || vert) {
    float fogFactor = exp(-0.001 * t);
    fragmentColor = vec4(uGridColor * fogFactor, fogFactor);
  } else {
    discard;
  }
}
