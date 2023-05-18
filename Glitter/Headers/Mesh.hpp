#pragma once
#include "Vertex.hpp"
#include <vector>

class Mesh {
	public:
		unsigned int m_VertexBufferObject;
		unsigned int m_VertexArrayObject;
		
		std::vector<Vertex> m_vertices;

		Mesh();
		void Render();
};