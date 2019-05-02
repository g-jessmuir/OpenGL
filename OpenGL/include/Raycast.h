#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Object.h>
#include <vector>
#include <algorithm>
#include <map>

struct RaycastHit {
	float distance;
	std::string key;
};

float checkIntersect(glm::vec3 origin, glm::vec3 direction, Object object, float maxDistance);

bool Raycast(glm::vec3 origin, glm::vec3 direction, std::map<std::string, Object> &objects, float maxDistance, RaycastHit &raycastHit) {
	raycastHit.distance = maxDistance;
	raycastHit.key = "";
	
	std::vector<std::string> intersectedKeys;
	std::vector<float> intersectDistances;
	for (std::map<std::string, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		if (it->first == "cursor")
			continue;
		float intersectDistance = checkIntersect(origin, direction, it->second, maxDistance);
		if (intersectDistance) {
			intersectedKeys.push_back(it->first);
			intersectDistances.push_back(intersectDistance);
		}
	}
	if (intersectedKeys.size() == 0)
		return false;

	float nearestDistance = maxDistance;
	int index = -1;
	for (int i(0); i < intersectDistances.size(); i++) {
		if (intersectDistances[i] < nearestDistance) {
			nearestDistance = intersectDistances[i];
			index = i;
		}
	}
	if (index == -1)
		return false;

	raycastHit.key = intersectedKeys[index];
	raycastHit.distance = nearestDistance;
	return true;
}

float checkIntersect(glm::vec3 origin, glm::vec3 direction, Object object, float maxDistance) {
	glm::vec3 maxB = object.position + glm::vec3(0.5);
	glm::vec3 minB = object.position - glm::vec3(0.5);
	
	float tMin = (minB.x - origin.x) / direction.x;
	float tMax = (maxB.x - origin.x) / direction.x;
	if (tMin > tMax) std::swap(tMin, tMax);

	float tMinY = (minB.y - origin.y) / direction.y;
	float tMaxY = (maxB.y - origin.y) / direction.y;

	if (tMinY > tMaxY) std::swap(tMinY, tMaxY);
	if ((tMin > tMaxY || tMinY > tMax))
		return false;

	tMin = tMinY > tMin ? tMinY : tMin;
	tMax = tMaxY < tMax ? tMaxY : tMax;

	float tMinZ = (minB.z - origin.z) / direction.z;
	float tMaxZ = (maxB.z - origin.z) / direction.z;

	if (tMinZ > tMaxZ) std::swap(tMinZ, tMaxZ);

	if (tMin > tMaxZ || tMinZ > tMax)
		return false;

	tMin = tMinZ > tMin ? tMinZ : tMin;
	tMax = tMaxZ < tMax ? tMaxZ : tMax;

	return tMin;	
}