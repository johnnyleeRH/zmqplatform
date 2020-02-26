#include <iostream>
#include <cstring>
#include <cstdio>
char dst[] = {
    0xf, 0xf, 0xc, 0xf, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x3
};

int delimiter1 = 0x01000000;
int delimiter2 = 0x00010000;
int FindDelimiter(const char* buf, const int len, const int beg) {
  char* pos = (char*)memchr(buf + beg, delimiter1, len - beg);
  if (pos != NULL)
    return pos - buf;
  return -1;
}

int main()
{
    int pos = FindDelimiter(dst, 11, 0);
    printf("%d\n", pos);
    
}