#include "Mesh.h"

const void Mesh::makeMash() {

	glGenVertexArrays(1, &EmptyVAOID);
	glBindVertexArray(EmptyVAOID);

	// Create SSBO vertex binding = 0
	glCreateBuffers(1, &SSBOvert);
	glNamedBufferStorage(
		SSBOvert,
		sizeof(Vertex) * Mesh::vertices.size(),
		Mesh::vertices.data(),
		GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBOvert);
}

void Mesh::clear() {
	glDeleteVertexArrays(1, &EmptyVAOID);
	glDeleteBuffers(1, &SSBOvert);
	SSBOvert = 0;
	EmptyVAOID = 0;
	vertices.clear();
}


void Mesh::Draw()
{
	glBindVertexArray(EmptyVAOID);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBOvert);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
}

const void WaterMesh::makeMash(std::vector <Texture>& textures) {

	WaterMesh::textures = textures;

	glGenVertexArrays(1, &EmptyVAOID);
	glBindVertexArray(EmptyVAOID);

	// Create SSBO vertex binding = 0
	glCreateBuffers(1, &SSBOvert);
	glNamedBufferStorage(
		SSBOvert,
		sizeof(WaterVertex) * WaterMesh::vertices.size(),
		WaterMesh::vertices.data(),
		GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBOvert);

}

void WaterMesh::clear() {
	glDeleteVertexArrays(1, &EmptyVAOID);
	glDeleteBuffers(1, &SSBOvert);
	SSBOvert = 0;
	EmptyVAOID = 0;
	vertices.clear();
	textures.clear();
}

void WaterMesh::Draw(){
	glBindVertexArray(EmptyVAOID);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBOvert);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 6);
}