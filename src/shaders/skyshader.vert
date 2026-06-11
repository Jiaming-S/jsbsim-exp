layout(location = 0) in vec3 position;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

out vec3 vWorldDirection;

void main() {
  gl_Position = vec4(position, 1.0);

  mat4 invProj = inverse(uProjectionMatrix);
  vec4 viewDir = invProj * vec4(position.xy, -1.0, 1.0);
  viewDir /= viewDir.w;

  vWorldDirection = (inverse(uViewMatrix) * vec4(viewDir.xyz, 0.0)).xyz;
}
