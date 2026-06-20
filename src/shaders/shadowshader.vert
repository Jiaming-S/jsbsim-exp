layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoords;

uniform mat4 uTransformationProjectionMatrix;

out vec2 vTextureCoords;

void main() {
    vTextureCoords = textureCoords;
    gl_Position = uTransformationProjectionMatrix * position;
}
