#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
#include <stb_image.h>

struct Vertex {
	// position
	glm::vec3 Position;
	// texCoords
	glm::vec2 TexCoords;
};

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Heightmap
{
public:
	//heightmap attributes
	int width, height;

	// VAO for heightmap
	unsigned int VAO;

	// pointer to data - data is an array so can be accessed by data[x]. 
	//       - this is an uint8 array (so values range from 0-255)
	unsigned char *data;

	// heightmap vertices
	std::vector<Vertex> vertices;
	// indices for EBO
	std::vector<unsigned int> indices;


	// constructor
	Heightmap(const char* heightmapPath)
	{
		// load heightmap data
		load_heightmap(heightmapPath);

		// create heightmap verts from the image data - (you have to write this)
		create_heightmap();

		// free image data
		stbi_image_free(data);

		// create_indices - create the indices array (you have to write this)
		//  This is an optional step so and you can ignore this if you want to just create all the triangles rather than
		//     using this to index it.
		create_indices();

		// setup the VBO, VAO, and EBO and send the information to OpenGL (you need to write this)
		setup_heightmap();
	}

	// render the heightmap mesh (you need to write this)
	void Draw(Shader shader, unsigned int textureID)
	{
		// You must:
		// -  active proper texture unit before binding
		// -  bind the texture
		// -  draw mesh (using GL_TRIANGLES is the most reliable way)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(VAO);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0, -10, 0));
		model = glm::scale(model, glm::vec3(100.0f, 5.0f, 100.0f));

		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		shader.setMat4("model", model);

		glDrawElements(GL_TRIANGLES, vertices.size() * sizeof(Vertex), GL_UNSIGNED_INT, 0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:

	unsigned int VBO, EBO;

	// Load the image data
	void load_heightmap(const char* heightmapPath)
	{
		int nrChannels;
		data = stbi_load(heightmapPath, &width, &height, &nrChannels, 0);
		if (!data)
		{
			std::cout << "Failed to load heightmap" << std::endl;
		}

	}


	// Make Vertex:  take x and y position return a new vertex for that position which includes 
	//  the position and the texture coordinates
	//     The data is in a char c-array and can be access via  
	//           float(data[x*height + y]) / 255.0f 
	//      where x and y are varables between 0 and width or height  (just use a black and white image for simplicity)

	
	Vertex make_vertex(int x, int y)

	{
		Vertex v;
		float point = float(data[x * height + y]) / 255.0f;
		//XYZ coords
		v.Position.x =  (- width / 2.0f + x) / width;
		v.Position.y = point;
		v.Position.z = (- height / 2.0f + y) / height;

		//Texture Coords
		v.TexCoords.x = x / (width - 1.0f);
		v.TexCoords.y = y / (height - 1.0f);

		return v;
	}
	

	// convert heightmap to floats, set position and texture vertices using the subfunction make_vertex
	void create_heightmap()
	{
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Vertex point = make_vertex(x, y);
				vertices.push_back(point);
			}
		}
		std::cout << vertices.size() << std::endl;
		std::cout << width << std::endl;
		std::cout << height << std::endl;
	}


	/*
	0 1 2 3
	4 5 6 7
	*/


	// create the indicies array for the EBO (so what indicies correspond with triangles to for rendering)
	//  This is an optional step so and you can ignore this if you want to just create all the triangles rather than
	//     using this to index it.
	void create_indices()
	{
		for (unsigned int i = 0; i < height - 1; i++) {
			for(unsigned int j = 0; j < width - 1; j++) {
				for (unsigned int k = 0; k < 2; k++) {
					indices.push_back((i + k) * width + j);
					indices.push_back((i + k) * width + j + 1);
					indices.push_back((i + 1 - k) * width + j + k);
				}
			}
		}
		std::cout << indices.size() << std::endl;
	}


	// create buffers/arrays for the VAO, VBO, and EBO 
	// Notes
	//  -  sizeof(Vertex) returns the size of the vertex
	//  -  to get the pointer the underlying array, you can use "&vertices[0]"
	void setup_heightmap()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

};
#endif