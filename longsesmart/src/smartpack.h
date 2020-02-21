#ifndef SRC_SMARTPACK_HPP
#define SRC_SMARTPACK_HPP

#include <atomic>
#include <cstdint>
#include "msgparse.h"

namespace horizon {

const uint32_t kSmartDataLen = 1 * 1024 * 1024; // 1MB

enum SMART_TYPE {
  TYPE_DETECT = 0x1,
  TYPE_CMP = 0x2,
  TYPE_OTHER
};

class SmartPack {
public:
  static SmartPack& Inst();
  bool PackFaceDetect();
  bool PackFaceCmp();
  bool PackLongseSmart();
  SMART_TYPE GetSmartType();
  bool GetSmartData(const char* &data);
public:
  // about private data can be opr or not
  bool CanWrite();
  bool WriteFinish();
  bool CanRead();
  bool ReadFinish();
private:
  SmartPack();
  ~SmartPack();
  bool CheckDetectLen();
  bool CheckCmpLen();
  void FillFaceFrameHead(longse::FACE_FRAME_DATA_HEAD* head);
  void FillFacePoint(longse::FACE_POINT_INFO* dst, longse::FACE_POINT_INFO& src);
  void FillFaceCmpHead(longse::FACE_COMPARE_PICTURE_HEAD* head);
private:
  MsgParse& parser_;
  char* data_;
  // 0(can be write) -> 1(finish write, and begin read)
  //  -> 0(finish read, next round)
  std::atomic_uchar writecondition_;
};

}

#endif  // SRC_SMARTPACK_HPP