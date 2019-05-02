#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <shaders.h>
#include <vector>
#include <map>
enum DRAW_MODE { DM_NORMAL, DM_NO_LINES, DM_ONLY_LINES };

class Object {
private:
	std::vector<float> vertices;
	unsigned int VAO, VBO;
public:
	DRAW_MODE drawMode = DM_NORMAL;
	glm::vec4 color;
	glm::vec4 lineColor;
	glm::vec3 position;
	glm::vec3 rotation = glm::vec3(0, 1, 0);
	float rotationAngle = 0;
	glm::vec3 scale = glm::vec3(1);
	Object(std::vector<float> nVertices = std::vector<float>(), glm::vec3 nPosition = glm::vec3(0), 
		glm::vec4 nColor = glm::vec4(1), glm::vec4 nLineColor = glm::vec4(0), DRAW_MODE nDrawMode = DM_NO_LINES) {
		vertices = nVertices;
		position = nPosition;
		color = nColor;
		lineColor = nLineColor;
		drawMode = nDrawMode;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	void Draw(Shader shader) {
		glm::mat4 transform = glm::mat4(1.f);
		transform = glm::translate(transform, position);
		transform = glm::scale(transform, scale);
		transform = glm::rotate(transform, rotationAngle, rotation);
		glBindVertexArray(VAO);
		shader.setMat3("normalMat", glm::mat3(glm::inverse(glm::transpose(transform))));
		shader.setMat4("model", transform);
		if (drawMode == DM_NORMAL || drawMode == DM_NO_LINES) {
			shader.setVec4("objectColor", color);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		if (drawMode == DM_NORMAL || drawMode == DM_ONLY_LINES) {
			shader.setVec4("objectColor", lineColor);
			glDrawArrays(GL_LINE_STRIP, 0, 36);
		}
	}
	void Destroy() {
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
};