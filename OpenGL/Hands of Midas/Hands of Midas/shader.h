#ifndef SHADER_H
#define SHADER_H 

//#pragma comment(lib,"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.14393.0\\um\\x86\\glew32")

#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif