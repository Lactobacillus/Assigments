#ifndef TEXTURE_H
#define TEXTURE_H 

#include <string>
#include <vector>

GLuint loadBMP_custom(const char * imagepath);
GLuint loadDDS(const char * imagepath);
GLuint loadCubemap(std::vector <std::string> faces);

#endif