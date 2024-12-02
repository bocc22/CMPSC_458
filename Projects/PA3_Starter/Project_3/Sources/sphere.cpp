#include "sphere.h"
#include <numbers>

//constructor given  center, radius, and material
sphere::sphere(glm::vec3 c, float r, int m, scene* s) : rtObject(s)
{
	center = c;
	radius = r;
	matIndex = m;
	myScene = s;
}

float sphere::testIntersection(glm::vec3 eye, glm::vec3 dir)
{
	//see the book for a description of how to use the quadratic rule to solve
	//for the intersection(s) of a line and a sphere, implement it here and
	//return the minimum positive distance or 9999999 if none
	float b = glm::dot(dir, eye - center);

	float discriminant = pow(b, 2) - ((glm::dot(eye - center, eye - center) - pow(radius, 2)));
	if (discriminant > 0) {
		float t1 = (-1.0f * b + sqrt(discriminant));
		float t2 = (-1.0f * b - sqrt(discriminant));
		if (t1 > 1e-3 && t1 < 9999999 && t1 < t2) {
			return t1;
		}
		else if (t2 > 1e-3 && t2 < 9999999){
			return t2;
		}
	}	
	return 9999999;
}

glm::vec3 sphere::getNormal(glm::vec3 eye, glm::vec3 dir)
{
	//once you can test for intersection,
	//simply add (distance * view direction) to the eye location to get surface location,
	//then subtract the center location of the sphere to get the normal out from the sphere
	glm::vec3 normal;
	glm::vec3 pos = testIntersection(eye, dir) * dir + eye;

	//dont forget to normalize
	normal = glm::normalize(pos - center);
	
	return normal;
}

glm::vec2 sphere::getTextureCoords(glm::vec3 eye, glm::vec3 dir)
{
	//find the normal as in getNormal
	glm::vec3 normal = getNormal(eye, dir);
	//use these to find spherical coordinates
	glm::vec3 x(1, 0, 0);
	glm::vec3 z(0, 0, 1);

	//phi is the angle down from z
	//theta is the angle from x curving toward y
	//hint: angle between two vectors is the acos() of the dot product

	//find phi using normal and z
	float phi = acos(normal.z);
	//find the x-y projection of the normal
	glm::vec3 xyProj = glm::normalize(glm::vec3(normal.x, normal.y, 0.0f));
	//find theta using the x-y projection and x
	float theta = acos(xyProj.x);
	//if x-y projection is in quadrant 3 or 4, then theta=2*PI-theta
	if (xyProj.y < 0) {
		theta = 2 * 3.141 - theta;
	}
	//return coordinates scaled to be between 0 and 1
	glm::vec2 coords(theta / (2 * 3.141), phi / 3.141);
	//coords = glm::normalize(coords);
	return coords;
}