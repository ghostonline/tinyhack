R"(
#version 330

layout(location=0) in vec2 aPos;

uniform mat4 mvp;

out vec2 internal_coord;

void main()
{
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
    internal_coord = aPos;
}
)";
