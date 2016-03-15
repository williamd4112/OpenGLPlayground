#include "GLMeshObject.h"
#include "GLData.hpp"

#include <vector>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

GLMeshObject::GLMeshObject()
{
}


GLMeshObject::~GLMeshObject()
{
}

bool GLMeshObject::Load(const char * filepath)
{
	bool Ret = false;
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(filepath, 
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (pScene) {
		vertexGroups.resize(pScene->mNumMeshes);
		materials.resize(pScene->mNumMaterials);

		// Initialize the meshes in the scene one by one
		for (unsigned int i = 0; i < vertexGroups.size(); i++) {
			const aiMesh* paiMesh = pScene->mMeshes[i];
			vertexGroups[i].materialIndex = paiMesh->mMaterialIndex;

			std::vector<Vertex> Vertices;
			std::vector<unsigned int> Indices;

			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
			
			// Pos, Normal, Uv
			for (unsigned int j = 0; j < paiMesh->mNumVertices; j++) {
				const aiVector3D* pPos = &(paiMesh->mVertices[j]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;

				Vertices.push_back(Vertex(glm::vec3(pPos->x, pPos->y, pPos->z),
					glm::vec3(pNormal->x, pNormal->y, pNormal->z),
					glm::vec2(pTexCoord->x, pTexCoord->y)));
			}

			// Index
			for (unsigned int j = 0; j < paiMesh->mNumFaces; j++) {
				const aiFace& Face = paiMesh->mFaces[j];
				assert(Face.mNumIndices == 3);
				Indices.push_back(Face.mIndices[0]);
				Indices.push_back(Face.mIndices[1]);
				Indices.push_back(Face.mIndices[2]);
			}

			vertexGroups[i].Load(Vertices, Indices);
		}

		Ret = LoadMaterial(pScene, filepath);
	}
	else {
		printf("Error parsing '%s': '%s'\n", filepath, Importer.GetErrorString());
	}

	return Ret;
}

void GLMeshObject::Render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0; i < vertexGroups.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexGroups[i].vertexBufferObj);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(glm::vec3)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexGroups[i].indexBufferObj);

		const unsigned int MaterialIndex = vertexGroups[i].materialIndex;

		if (MaterialIndex < materials.size() && materials[MaterialIndex]) {
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (const GLvoid*)12);
			materials[MaterialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, vertexGroups[i].indexNum, GL_UNSIGNED_INT, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

bool GLMeshObject::LoadMaterial(const aiScene * pScene, const char * filepath)
{
	// Extract the directory part from the file name
	std::string Filename(filepath);
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		materials[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string FullPath = Dir + "/" + Path.data;
				materials[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

				if (!materials[i]->Load()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete materials[i];
					materials[i] = NULL;
					Ret = false;
				}
				else {
					printf("Loaded texture '%s'\n", FullPath.c_str());
				}
			}
		}

		// Load a white texture in case the model does not include its own texture
		if (!materials[i]) {
			materials[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");

			Ret = materials[i]->Load();
		}
	}

	return Ret;
}

void VertexGroup::Load(const std::vector<Vertex>& vertexs, const std::vector<unsigned int>& indexs)
{
	indexNum = indexs.size();
	glGenBuffers(1, &vertexBufferObj);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexs.size(), &vertexs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexBufferObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexNum, &indexs[0], GL_STATIC_DRAW);
}
