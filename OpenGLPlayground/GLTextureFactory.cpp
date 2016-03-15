#include "GLTextureFactory.h"



GLTextureFactory::GLTextureFactory()
{
}


GLTextureFactory::~GLTextureFactory()
{
}

GLuint GLTextureFactory::LoadBMP(const char * filepath)
{
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(filepath, "rb");
	if (file == NULL) {
		fprintf(stderr, "GLTextureFactory::LoadBMP(): failed to open the file.\n");
		throw new GLTextureException(GLTextureException::LOAD_INVALID);
	}

	if (fread(header, 1, 54, file) != 54) 
	{
		fprintf(stderr, "GLTextureFactory::LoadBMP(): not a correct bmp file.\n");
		throw new GLTextureException(GLTextureException::LOAD_INVALID);
	}

	if (header[0] != 'B' || header[1] != 'M') 
	{
		fprintf(stderr, "GLTextureFactory::LoadBMP(): not a correct bmp file.\n");
		throw new GLTextureException(GLTextureException::LOAD_INVALID);
	}

	if (*(int*)&(header[0x1E]) != 0) 
	{ 
		fprintf(stderr, "GLTextureFactory::LoadBMP(): not a correct bmp file.\n");
		throw new GLTextureException(GLTextureException::LOAD_INVALID);
	}

	if (*(int*)&(header[0x1C]) != 24) 
	{
		fprintf(stderr, "GLTextureFactory::LoadBMP(): not a correct bmp file.\n");
		throw new GLTextureException(GLTextureException::LOAD_INVALID);
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0) dataPos = 54; // The BMP header is done that way

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	delete[] data;

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}
