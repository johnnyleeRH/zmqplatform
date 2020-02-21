// from http://carnage-melon.tom7.org/stuff/jpegsize.html
/* portions derived from IJG code */

#include <string>
#include <cstdio>

#define readbyte(a,b) do if(((a)=getc((b))) == EOF) return 0; while (0)
#define readword(a,b) do { int cc_=0,dd_=0; \
                          if((cc_=getc((b))) == EOF \
        		              || (dd_=getc((b))) == EOF) return 0; \
                          (a) = (cc_<<8) + (dd_); \
                          } while(0)


int scanhead (FILE * infile, int * image_width, int * image_height) {
  int marker=0;
  int dummy=0;
  if ( getc(infile) != 0xFF || getc(infile) != 0xD8 )
    return 0;

  for (;
      ;) {

    int discarded_bytes=0;
    readbyte(marker,infile);
    while (marker != 0xFF) {
      discarded_bytes++;
      readbyte(marker,infile);
    }
    do readbyte(marker,infile); while (marker == 0xFF);

    if (discarded_bytes != 0) return 0;
   
    switch (marker) {
    case 0xC0:
    case 0xC1:
    case 0xC2:
    case 0xC3:
    case 0xC5:
    case 0xC6:
    case 0xC7:
    case 0xC9:
    case 0xCA:
    case 0xCB:
    case 0xCD:
    case 0xCE:
    case 0xCF: {
      readword(dummy,infile);	/* usual parameter length count */
      readbyte(dummy,infile);
      readword((*image_height),infile);
      readword((*image_width),infile);
      readbyte(dummy,infile);

      return 1;
      break;
      }
    case 0xDA:
    case 0xD9:
      return 0;
    default: {
      int length;
      
      readword(length,infile);

      if (length < 2)
        return 0;
      length -= 2;
      while (length > 0) {
        readbyte(dummy, infile);
        length--;
      }
      }
      break;
    }
  }
}

#define readbyte_s(a, b, c) do { if(b > c.size() - 1) return false; \
                              a = c[b++]; \
                              }while (0)
#define readword_s(a, b, c) do { int cc_=0,dd_=0; \
                              if(b > c.size() - 2) return false; \
                              cc_ = c[b++]; \
                              dd_ = c[b++]; \
                              a = (cc_<<8) + (dd_); \
                          } while(0)

bool scanhead_s (const std::string& jpgstr, unsigned int& image_width, unsigned int& image_height) {
  int marker=0;
  int dummy=0;
  int id = 0;
  if (jpgstr.size() < 2)
    return false;
  if ( jpgstr[id++] != 0xFF || jpgstr[id++] != 0xD8 )
    return false;

  for (;
      ;) {
    int discarded_bytes=0;
    readbyte_s(marker, id, jpgstr);
    while (marker != 0xFF) {
      discarded_bytes++;
      readbyte_s(marker,id, jpgstr);
    }
    do readbyte_s(marker,id, jpgstr); while (marker == 0xFF);

    if (discarded_bytes != 0) return 0;
   
    switch (marker) {
    case 0xC0:
    case 0xC1:
    case 0xC2:
    case 0xC3:
    case 0xC5:
    case 0xC6:
    case 0xC7:
    case 0xC9:
    case 0xCA:
    case 0xCB:
    case 0xCD:
    case 0xCE:
    case 0xCF: {
      readword_s(dummy,id, jpgstr);	/* usual parameter length count */
      readbyte_s(dummy,id, jpgstr);
      readword_s(image_height,id, jpgstr);
      readword_s(image_width,id, jpgstr);
      readbyte_s(dummy,id, jpgstr);

      return 1;
      break;
      }
    case 0xDA:
    case 0xD9:
      return 0;
    default: {
      int length;  
      readword_s(length,id, jpgstr);
      if (length < 2)
        return 0;
      length -= 2;
      while (length > 0) {
        readbyte_s(dummy, id, jpgstr);
        length--;
      }
      }
      break;
    }
  }
}