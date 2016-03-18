#include "GLMesh.h"
#include "FileUtil.h"

GLMesh::GLMesh()
	: GlutRenderable()
{
}


GLMesh::~GLMesh()
{
}

void GLMesh::Load(std::string filepath)
{
	const aiScene *scene = importer.ReadFile(filepath,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_FindDegenerates);
	bool result = false;
	if (scene)
	{
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);
		LoadScene(scene);
		LoadMaterial(scene, filepath);
		//glBindVertexArray(0);
	}
	else
	{
		std::cerr << "GLMesh import error " << filepath << std::endl;
	}
}

void GLMesh::RenderShader()
{
}

void GLMesh::RenderFixedPipeline()
{
	glPushMatrix();
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	for (int i = 0; i < entries.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, entries[i].vbo);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (const GLvoid*)12);
		glNormalPointer(GL_FLOAT, sizeof(Vertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries[i].ibo);

		const unsigned int MaterialIndex = entries[i].materialIndex;
		if (MaterialIndex < materials.size()) {
			materials[entries[i].materialIndex].Bind(GL_TEXTURE_2D);
		}
		glDrawElements(GL_TRIANGLES, entries[i].numIndices, GL_UNSIGNED_INT, 0);
		
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();
}

void GLMesh::LoadScene(const aiScene * scene)
{
	entries.resize(scene->mNumMeshes);
	//materials.resize(scene->mNumMaterials);

	for (int i = 0; i < entries.size(); i++)
	{
		std::vector<Vertex> vertexes;
		std::vector<unsigned int> indicies;

		const aiMesh *mesh = scene->mMeshes[i];
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		entries[i].numVertexes = mesh->mNumVertices;
		entries[i].numIndices = mesh->mNumFaces * 3;
		entries[i].materialIndex = mesh->mMaterialIndex;

		for (int j = 0; j < mesh->mNumVertices; j++)
		{
			const aiVector3D *pos = &mesh->mVertices[j];
			const aiVector3D* texcoord = mesh->HasTextureCoords(0) ?
				&(mesh->mTextureCoords[0][j]) : &Zero3D;
			const aiVector3D *normal = &mesh->mNormals[j];
			vertexes.push_back(
				Vertex(glm::vec3(pos->x, pos->y, pos->z),
					glm::vec2(texcoord->x, texcoord->y),
					glm::vec3(normal->x, normal->y, normal->z)));
		}

		for (int j = 0; j < mesh->mNumFaces; j++)
		{
			assert(mesh->mFaces[j].mNumIndices == 3);
			const aiFace &face = mesh->mFaces[j];
			indicies.push_back(face.mIndices[0]);
			indicies.push_back(face.mIndices[1]);
			indicies.push_back(face.mIndices[2]);
		}

		glGenBuffers(1, &entries[i].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, entries[i].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), &vertexes[0], GL_STATIC_DRAW);

		glGenBuffers(1, &entries[i].ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries[i].ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), &indicies[0], GL_STATIC_DRAW);
	}
}

void GLMesh::LoadMaterial(const aiScene * scene, std::string filepath)
{
	std::string dir = FileUtil::GetDirectoryPath(filepath);
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial *material = scene->mMaterials[i];
		materials.push_back(GLMaterial(material, dir));
	}
}
