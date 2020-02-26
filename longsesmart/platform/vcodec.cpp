#include "vcodec.h"
#include <cstdio>
#include <cstring>

Vcodec::Vcodec() { inited_ = false; }

bool Vcodec::CheckNALU(const char* buf, const int len, int& pos) {
  pos = FindDelimiter(buf, len, pos);
  /**
   * |0 |1 |2 |3 |4 |5 |6 |7 |
   * |F |NRI  |Type          |
   * I Frame = | SPS | |PPS| |IDR|
   */
  if (-1 == pos) return false;
  pos += delimiterlen_;
  // printf("pos %d data 0x%x\n", pos, buf[pos]);
  if ((buf[pos] & 0x80) != 0x0) return false;
  // if ((buf[pos] & 0x60) == 0x0) return false;
  return true;
}

// only do the simplest check for h264 now
Vcodec::FrameType Vcodec::GetFrameType(const char* buf, const int len) {
  if (!Init(buf, len)) return FRAME_UNKNOWN;
  int pos = 0;
  if (!CheckNALU(buf, len, pos)) return FRAME_UNKNOWN;
  char type = buf[pos] & 0x1F;
  if (type == 1) return FRAME_P;
  // find SPS
  if (type == 7) {
    if (!CheckNALU(buf, len, pos)) return FRAME_UNKNOWN;
    // PPS
    if ((buf[pos] & 0x1F) != 8) return FRAME_UNKNOWN;
    // till find IDR or find delimiter failed
    while (true) {
      if (!CheckNALU(buf, len, pos)) return FRAME_UNKNOWN;
      // IDR
      if ((buf[pos] & 0x1F) == 5) return FRAME_I;
    }
  } else {
    return FRAME_UNKNOWN;
  }
  return FRAME_UNKNOWN;
}

bool Vcodec::Init(const char* buf, const int len) {
  if (inited_) return true;
  if (len >= 4 && 0x1000000 == *((int*)buf)) {
    delimiter_ = delimiter_v1;
    delimiterlen_ = 4;
    inited_ = true;
  }
  if (len >= 3 && buf[0] == 0 && buf[1] == 0 && buf[2] == 1) {
    delimiter_ = delimiter_v2;
    delimiterlen_ = 3;
    inited_ = true;
  }
  return inited_;
}

int Vcodec::FindDelimiter(const char* buf, const int len, const int beg) {
  for (int id = beg; id < len - delimiterlen_ - 1; ++id) {
    if (*((unsigned char*)(buf + id)) > 0x1) continue;
    int i = 0;
    for (; i < delimiterlen_; ++i) {
      if (buf[id + i] != delimiter_[i]) break;
    }
    if (i == delimiterlen_) return id;
  }
  return -1;
}