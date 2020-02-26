#ifndef SRC_VCODEC_H_
#define SRC_VCODEC_H_
#include <string>
/*
 *simple video parse for h264
 */
class Vcodec {
 public:
  Vcodec();
  enum FrameType { FRAME_UNKNOWN = 0, FRAME_I = 1, FRAME_P = 2, FRAME_B = 3 };
  FrameType GetFrameType(const char* buf, const int len);

 private:
  bool Init(const char* buf, const int len);
  int FindDelimiter(const char* buf, const int len, const int beg);
  bool CheckNALU(const char* buf, const int len, int& pos);

 private:
  bool inited_;
  const char* delimiter_;
  int delimiterlen_;
  const char delimiter_v1[4] = {0x00, 0x00, 0x00, 0x01};
  const char delimiter_v2[3] = {0x00, 0x00, 0x01};
};

#endif  // SRC_VCODEC_H_