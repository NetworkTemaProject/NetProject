#include <cstdlib>
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#define _CRT_SECURE_NO_WARNINGS

using namespace std;

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fopen_s(&fptr, file, "rb");				 // Open file for reading
	if (!fptr)								// Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END);				 // Seek to the end of the file
	length = ftell(fptr);					// Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1);		// Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET);				// Go back to the beginning of the file
	fread(buf, length, 1, fptr);			 // Read the contents of the file in to the buffer
	fclose(fptr);							// Close the file
	buf[length] = 0;						 // Null terminator
	return buf;								 // Return the buffer
}


BOOL checkCompileErrors(GLuint uin, const char* errorword)
{
	GLint result;
	glGetShaderiv(uin, GL_COMPILE_STATUS, &result);

	if (result != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(uin, 1024, &log_length, message);
		return false;
	}

	return true;
}