#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_viewMat;
uniform mat4 u_projMat;

uniform vec3 u_position;
uniform vec3 u_scale;
uniform vec4 u_rotation;

mat4 model;

mat4 scaleMatrix(vec3 s) {
    return mat4(vec4(s.x, 0, 0, 0), vec4(0, s.y, 0, 0), vec4(0, 0, s.z, 0), vec4(0, 0, 0, 1));
}

// Quaternion to rotation matrix (assumes q = (x, y, z, w))
mat4 quatToMat4(vec4 q) {
    float x = q.x, y = q.y, z = q.z, w = q.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;
    return mat4(1.0 - (yy + zz), xy + wz, xz - wy, 0.0, xy - wz, 1.0 - (xx + zz), yz + wx, 0.0,
                xz + wy, yz - wx, 1.0 - (xx + yy), 0.0, 0.0, 0.0, 0.0, 1.0);
}

mat4 translateMatrix(vec3 t) {
    return mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, t.x, t.y, t.z, 1.0);
}

void main() {
    mat4 S = scaleMatrix(u_scale);
    // mat4 R = quatToMat4(uRotation);
    mat4 T = translateMatrix(u_position);
    model = T * S;
    // model = T * R * S;
    FragPos = vec3(model * vec4(a_position, 1.0));
    Normal = mat3(transpose(inverse(model))) * a_normal;
    // gl_Position = u_projMat * u_viewMat * model * vec4(aPosition, 1.0);
    gl_Position = model * vec4(a_position, 1.0);
}
