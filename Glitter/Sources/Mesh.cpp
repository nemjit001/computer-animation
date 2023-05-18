#pragma once
#include <iostream>
#include <glad/glad.h>
#include "Mesh.hpp"
#include <glm/glm.hpp>

Mesh::Mesh() {
    // Hardcoded triangle vertices:
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Enter the hardcoded triangle position coordinates into the vector of vertices.
    m_vertices = std::vector<Vertex>();
    for (int i = 0; i < 3; i++) {
        Vertex nextVertex;
        nextVertex.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        m_vertices.push_back(nextVertex);
    }

    // The Mesh class and its properties have been set up. Next we need to execute some OpenGL code.

    // Request a buffer from OpenGL - a place on the GPU to store our vertices.
	glGenBuffers(1, &m_VertexBufferObject);

    // Bind our buffer into the GL_ARRAY_BUFFER slot.
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);

    // Figure out the total size of the vertex data in bytes:
    unsigned int verticesTotalBytes = sizeof(m_vertices[0]) * m_vertices.size();
    
    // Send all the vertex data to OpenGL. Our buffer is bound to the GL_ARRAY_BUFFER slot so we can send the data using that information
    glBufferData(GL_ARRAY_BUFFER, verticesTotalBytes, m_vertices.data(), GL_STATIC_DRAW);
    
    // Request a Vertex Array Object from OpenGL - A vertex array stores information about how vertex data should be interpreted.
    glGenVertexArrays(1, &m_VertexArrayObject);
    glBindVertexArray(m_VertexArrayObject);

    unsigned int positionDimensions = 3;
    unsigned int stride = sizeof(m_vertices[0]);

    // Let OpenGL know how our vertex data should be interpreted:
    
    // The positional vertex data is contained in the first three (3D) floats of every vertex:
    glVertexAttribPointer(0, positionDimensions, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
};

void Mesh::Render() {
    // Bind the vertex array object (defines how the vertex data should be interpreted), and vertex buffer (contains the vertex data). Then draw the faces.
    glBindVertexArray(m_VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    glBindVertexArray(0);
}