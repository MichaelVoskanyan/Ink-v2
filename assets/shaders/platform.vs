#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_viewMat;
uniform mat4 u_projMat;

uniform vec3 u_position;
uniform vec3 u_scale;
uniform vec4 u_rotation;

// Build scale matrix
mat4 scaleMatrix(vec3 s) {
    return mat4(
        vec4(s.x, 0.0, 0.0, 0.0),
        vec4(0.0, s.y, 0.0, 0.0),
        vec4(0.0, 0.0, s.z, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

// Quaternion to rotation matrix
mat4 quatToMat4(vec4 q) {
    float x = q.x, y = q.y, z = q.z, w = q.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;
    return mat4(
        1.0 - (yy + zz), xy + wz,       xz - wy,       0.0,
        xy - wz,        1.0 - (xx + zz), yz + wx,      0.0,
        xz + wy,        yz - wx,        1.0 - (xx + yy), 0.0,
        0.0,            0.0,            0.0,           1.0
    );
}

// Build translation matrix
mat4 translateMatrix(vec3 t) {
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(t.x, t.y, t.z, 1.0)
    );
}

void main() {
    // Construct model matrix: scale -> (rotation) -> translate
    mat4 S = scaleMatrix(u_scale);
    // Uncomment if using rotation
    // mat4 R = quatToMat4(u_rotation);
    mat4 T = translateMatrix(u_position);
    // mat4 model = T * R * S;
    mat4 model = T * S;

    // Compute world position and normal
    vec4 worldPos = model * vec4(a_position, 1.0);
    FragPos  = worldPos.xyz;
    Normal   = mat3(transpose(inverse(model))) * a_normal;

    // Pass UV to fragment
    TexCoords = a_uv;

    // Final clip-space position
    gl_Position = u_projMat * u_viewMat * worldPos;
}