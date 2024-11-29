#include "triangle.h"

//constructor given  center, radius, and material
triangle::triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float tx0, float tx1, float tx2, float ty0, float ty1, float ty2, int m, scene* s) : rtObject(s)
{
	point0 = p0;
	point1 = p1;
	point2 = p2;

	texX0 = tx0;
	texX1 = tx1;
	texX2 = tx2;
	texY0 = ty0;
	texY1 = ty1;
	texY2 = ty2;
	matIndex = m;
	myScene = s;
}

float triangle::testIntersection(glm::vec3 eye, glm::vec3 dir)
{
	//see the book/slides for a description of how to use Cramer's rule to solve
	//for the intersection(s) of a line and a plane, implement it here and
	//return the minimum distance (if barycentric coordinates indicate it hit
	//the triangle) otherwise 9999999
	float a = point0.x - point1.x;
	float b = point0.y - point1.y;
	float c = point0.z - point1.z;
	float d = point0.x - point2.x;
	float e = point0.y - point2.y;
	float f = point0.z - point2.z;
	float g = dir.x;
	float h = dir.y;
	float i = dir.z;
	float j = point0.x - eye.x;
	float k = point0.y - eye.y;
	float l = point0.z - eye.z;

	float M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);

	float beta = (j * (e * i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;
	float gamma = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;
	float t = -1 * ((f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M);
	
	if (beta >= 0 && gamma >= 0 && beta + gamma <= 1) {
		//std::cout << "Beta: " << beta << ", Gamma: " << gamma << ", T: " << t << std::endl;
		return t;
	}

	return 9999999;
}

glm::vec3 triangle::getNormal(glm::vec3 eye, glm::vec3 dir)
{
	//construct the barycentric coordinates for the plane
	glm::vec3 bary1 = point1 - point0;
	glm::vec3 bary2 = point2 - point0;

	//cross them to get the normal to the plane
	//note that the normal points in the direction given by right-hand rule
	//(this can be important for refraction to know whether you are entering or leaving a material)
	glm::vec3 normal = glm::normalize(glm::cross(bary1, bary2));
	return normal;
}

glm::vec2 triangle::getTextureCoords(glm::vec3 eye, glm::vec3 dir)
{
	//find alpha and beta (parametric distance along barycentric coordinates)
	//use these in combination with the known texture surface location of the vertices
	//to find the texture surface location of the point you are seeing
	glm::vec3 betaUnitLength = point1 - point0;
	glm::vec3 gammaUnitLength = point2 - point0;
	glm::vec3 eyeToOrigin = eye - point0;

	glm::vec3 coords;
	return coords;
}