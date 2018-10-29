#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <iostream>
#include <glm/vec3.hpp>
#include <vector>

struct Pstruct{
  float picture[1920*1080*3];
  float factor;
};

struct SSBO {
  GLuint handle = 0;
};

struct Tex_obj{
  GLuint handle = 0;
};

struct PBO{
  GLuint handle = 0;
};

struct FBO{
  GLuint handle = 0;
};

#endif
