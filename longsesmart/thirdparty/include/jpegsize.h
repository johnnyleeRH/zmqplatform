#ifndef THIRDPARTY_INCLUDE_JPEGSIZE_H
#define THIRDPARTY_INCLUDE_JPEGSIZE_H
#include <stdio.h>
#include <string>

int scanhead (FILE * infile, int * image_width, int * image_height);
bool scanhead_s (const std::string& jpgstr, unsigned int& image_width, unsigned int& image_height);

#endif