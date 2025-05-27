#version 330 core

in vec2 TexCoords;            // from your vertex shader
uniform sampler2D u_texture;  // bound to GL_TEXTURE0

out vec4 FragColor;

void main() {
    FragColor = texture(u_texture, TexCoords);
}
