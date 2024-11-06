#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>
#include <rc_spline.h>

struct Orientation {
	// Front
	glm::vec3 Front;
	// Up
	glm::vec3 Up;
	// Right
	glm::vec3 Right;
	// origin
	glm::vec3 origin;
};


class Track
{
public:

	// VAO
	unsigned int VAO;

	// Control Points Loading Class for loading from File
	rc_Spline g_Track;

	// Vector of control points
	std::vector<glm::vec3> controlPoints;

	// Track data
	std::vector<Vertex> vertices;
	// indices for EBO
	std::vector<unsigned int> indices;

	// hmax for camera
	float hmax = 0.0f;


	// constructor, just use same VBO as before, 
	Track(const char* trackPath)
	{
		
		// load Track data
		load_track(trackPath);

		create_track();

		setup_track();
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID)
	{
		// Set the shader properties
		shader.use();
		glm::mat4 track_model = glm::mat4();
		shader.setMat4("model", track_model);


		// Set material properties
		shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
		shader.setFloat("material.shininess", 64.0f);


		// active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0);
		// and finally bind the textures
		glBindTexture(GL_TEXTURE_2D, textureID);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	// give a positive float s, find the point by interpolation
	// determine pA, pB, pC, pD based on the integer of s
	// determine u based on the decimal of s
	// E.g. s=1.5 is the at the halfway point between the 1st and 2nd control point,
	//		the 4 control points are:[0,1,2,3], with u=0.5
	glm::vec3 get_point(float s)
	{
		int pB = (int)s;
		int pA = pB - 1;
		int pC = pB + 1;
		int pD = pB + 2;
		float u = s - pB;
		return interpolate(controlPoints[pA], controlPoints[pB], controlPoints[pC], controlPoints[pD], 0.5f, u);
	}


	void delete_buffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

private:
	

	/*  Render data  */
	unsigned int VBO, EBO;

	void load_track(const char* trackPath)
	{
		// Set folder path for our projects (easier than repeatedly defining it)
		g_Track.folder = "../Project_2/Media/";

		// Load the control points
		g_Track.loadSplineFrom(trackPath);

	}

	// Implement the Catmull-Rom Spline here
	//	Given 4 points, a tau and the u value 
	//	u in range of [0,1]  
	//	Since you can just use linear algebra from glm, just make the vectors and matrices and multiply them.  
	//	This should not be a very complicated function
	glm::vec3 interpolate(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 pointD, float tau, float u)
	{
		float raw_points[] = {
			pointA.x, pointB.x, pointC.x, pointD.x,
			pointA.y, pointB.y, pointC.y, pointD.y,
			pointA.z, pointB.z, pointC.z, pointD.z
		};
		glm::mat3x4 points = glm::make_mat3x4(raw_points);

		float cat_rom_array[] = {
			0.0f,  0.0f - tau, 2.0f * tau, 0.0f - tau,
			1.0f, 0.0f, tau - 3.0f, 2.0f - tau,
			0.0f, tau, 3.0f - 2.0f * tau, tau - 2.0f,
			0.0f, 0.0f, 0.0f - tau, tau
		};
		glm::mat4x4 cat_rom = glm::make_mat4x4(cat_rom_array);

		glm::vec4 u_values = glm::vec4(1.0f, u, u*u, u*u*u);

		glm::vec3 result = u_values * cat_rom * points;
		return result;
	} 

	// Here is the class where you will make the vertices or positions of the necessary objects of the track (calling subfunctions)
	//  For example, to make a basic roller coster:
	//    First, make the vertices for each rail here (and indices for the EBO if you do it that way).  
	//        You need the XYZ world coordinates, the Normal Coordinates, and the texture coordinates.
	//        The normal coordinates are necessary for the lighting to work.  
	//    Second, make vector of transformations for the planks across the rails
	void create_track()
	{
		glm::vec3 pos = glm::vec3(0, 0, 0);
		//	Populating controlPoints
		for (pointVectorIter ptsiter = g_Track.points().begin(); ptsiter != g_Track.points().end(); ptsiter++) {
			glm::vec3 pt(*ptsiter);
			pos += pt;
			controlPoints.push_back(pos*2.0f);
		}
		// Create the vertices and indices (optional) for the rails
		//    One trick in creating these is to move along the spline and 
		//    shift left and right (from the forward direction of the spline) 
		//     to find the 3D coordinates of the rails.
		Orientation origOrient;
		origOrient.Up = glm::vec3(0, 1, 0);
		origOrient.Front = glm::vec3(1, 0, 0);
		origOrient.Right = glm::vec3(0, 0, 1);
		origOrient.origin = controlPoints[0];
		Orientation prevOrient;
		prevOrient = origOrient;
		for (int i = 1; i < controlPoints.size(); i++) {
			glm::vec3 pos = controlPoints[i];
			glm::vec3 forward = glm::normalize(pos - prevOrient.origin);
			
			Orientation newOrient;
			newOrient.Front = forward;
			newOrient.Right = glm::normalize(glm::cross(forward, prevOrient.Up));
			newOrient.Up = glm::normalize(glm::cross(newOrient.Right, forward));
			newOrient.origin = pos;
			for (int track = 0; track < 2; track++) {
				makeRailPart(prevOrient, newOrient, glm::vec2((-1)^track, 0));
			}
			prevOrient = newOrient;
		}

		for (int track = 0; track < 2; track++) {
			makeRailPart(prevOrient, origOrient, glm::vec2((-1) ^ track, 0));
		}

		for (int i = 0; i < vertices.size(); i++) {
			indices.push_back(i);
		}


		// Create the plank transformations or just creating the planks vertices
		//   Remember, you have to make planks be on the rails and in the same rotational direction 
		//       (look at the pictures from the project description to give you ideas).  

		

		


		// Here is just visualizing of using the control points to set the box transformatins with boxes. 
		//       You can take this code out for your rollercoster, this is just showing you how to access the control points
		//glm::vec3 currentpos = glm::vec3(-2.0f, 0.0f, -2.0f);
		/* iterate throught  the points	g_Track.points() returns the vector containing all the control points */
		//for (pointVectorIter ptsiter = g_Track.points().begin(); ptsiter != g_Track.points().end(); ptsiter++)
		//{
		//	/* get the next point from the iterator */
		//	glm::vec3 pt(*ptsiter);

		//	// Print the Box
		//	std::cout <<  pt.x << "  " << pt.y << "  " << pt.z << std::endl;

		//	/* now just the uninteresting code that is no use at all for this project */
		//	currentpos += pt;
		//	//  Mutliplying by two and translating (in initialization) just to move the boxes further apart.  
		//	controlPoints.push_back(currentpos*2.0f);
		//}
	}


	// Given 3 Points, create a triangle and push it into vertices (and EBO if you are using one)
		// Optional boolean to flip the normal if you need to
	void make_triangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec2 texCoordA, glm::vec2 texCoordB, glm::vec2 texCoordC, bool flipNormal)
	{
		//Make walls of the rail
		//Call normals

		Vertex v1;
		v1.Position = pointA;
		v1.TexCoords = texCoordA;

		Vertex v2;
		v2.Position = pointB;
		v2.TexCoords = texCoordB;

		Vertex v3;
		v3.Position = pointC;
		v3.TexCoords = texCoordC;

		if (flipNormal) {
			set_normals(v3, v2, v1);
		}
		else {
			set_normals(v1, v2, v3);
		}

		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
	}

	// Given two orientations, create the rail between them.  Offset can be useful if you want to call this for more than for multiple rails
	void makeRailPart(Orientation ori_prev, Orientation ori_cur, glm::vec2 offset)
	{
		glm::vec3 prev_center = ori_prev.origin + ori_prev.Right * offset.x * 3.0f;
		glm::vec3 prev_1 = prev_center + 0.5f * (-1.0f * ori_prev.Right + ori_prev.Up);
		glm::vec3 prev_2 = prev_center + 0.5f * (ori_prev.Right + ori_prev.Up);
		glm::vec3 prev_3 = prev_center + 0.5f * (-1.0f * ori_prev.Right + -1.0f * ori_prev.Up);
		glm::vec3 prev_4 = prev_center + 0.5f * (ori_prev.Right + -1.0f * ori_prev.Up);

		glm::vec3 curr_center = ori_cur.origin + ori_cur.Right * offset.x * 3.0f;
		glm::vec3 curr_1 = curr_center + 0.5f * (-1.0f * ori_cur.Right + ori_cur.Up);
		glm::vec3 curr_2 = curr_center + 0.5f * (ori_cur.Right + ori_cur.Up);
		glm::vec3 curr_3 = curr_center + 0.5f * (-1.0f * ori_cur.Right + -1.0f * ori_cur.Up);
		glm::vec3 curr_4 = curr_center + 0.5f * (ori_cur.Right + -1.0f * ori_cur.Up);

		make_triangle(prev_1, prev_2, curr_2, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), false);
		make_triangle(prev_1, curr_1, curr_2, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), true);

		make_triangle(prev_2, prev_4, curr_4, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), false);
		make_triangle(prev_2, curr_2, curr_4, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), true);

		make_triangle(prev_3, prev_1, curr_1, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), false);
		make_triangle(prev_3, curr_3, curr_1, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), true);

		make_triangle(prev_4, prev_3, curr_3, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), false);
		make_triangle(prev_4, curr_4, curr_3, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), true);
	}

	// Find the normal for each triangle uisng the cross product and then add it to all three vertices of the triangle.  
	//   The normalization of all the triangles happens in the shader which averages all norms of adjacent triangles.   
	//   Order of the triangles matters here since you want to normal facing out of the object.  
	void set_normals(Vertex &p1, Vertex &p2, Vertex &p3)
	{
		glm::vec3 normal = glm::cross(p2.Position - p1.Position, p3.Position - p1.Position);
		p1.Normal += normal;
		p2.Normal += normal;
		p3.Normal += normal;
	}

	void setup_track()
	{
	   // Like the heightmap project, this will create the buffers and send the information to OpenGL

		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/3/2 array which
		// again translates to 3/3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normal coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);

	}

};
