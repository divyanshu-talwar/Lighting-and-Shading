#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_




#define glCheckError() glCheckError_(__FILE__, __LINE__)
GLenum glCheckError_(const char *file, int line);



#endif
