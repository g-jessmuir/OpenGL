#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
  
uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(iPos, 1.0f);
	Normal = normalMat * iNormal;
	FragPos = vec3(model * vec4(iPos, 1));
} 