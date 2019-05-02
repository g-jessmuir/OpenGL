#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <shaders.h>
#include <Object.h>
#include <vector>

class Voxel : public Object {
private:
	
public:
	Voxel(glm::vec3 nPosition, glm::vec4 nColor = glm::vec4(1, 0, 1, 1)) {
		Object(cubeVerts, nPosition, nColor);
	}
};