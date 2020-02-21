#include "smartpack.h"
#include <stdlib.h>

namespace horizon {

SmartPack::SmartPack() : parser_(MsgParse::Inst()), writecondition_(0) {
  data_ = reinterpret_cast<char*>(malloc(kSmartDataLen));
}

SmartPack::~SmartPack() {
  free(data_);
}

SmartPack& SmartPack::Inst() {
  static SmartPack inst;
  return inst;
}

bool SmartPack::PackFaceDetect() {
  printf("lrh lib pack face detect.\n");
  if (!data_) return false;
  if (!CheckDetectLen()) return false;
  if (!CanWrite()) return false;
  printf("lrh detect begin write.\n");
  uint32_t len = 0;
  *data_ = TYPE_DETECT;
  len += 1;
  FillFaceFrameHead(
      reinterpret_cast<longse::FACE_FRAME_DATA_HEAD*>(data_ + len));
  len += sizeof(longse::FACE_FRAME_DATA_HEAD);
  for (int id = 0; id < parser_.facepoint_.size(); ++id) {
    FillFacePoint(reinterpret_cast<longse::FACE_POINT_INFO*>(data_ + len),
                  parser_.facepoint_[id]);
    len += sizeof(longse::FACE_POINT_INFO);
  }
  memcpy(data_ + len, (char*)&(parser_.panormapic_),
         sizeof(parser_.panormapic_));
  len += sizeof(parser_.panormapic_);
  memcpy(data_ + len, parser_.backgrounddata_.c_str(),
         parser_.panormapic_.imageAttr.size);
  len += parser_.panormapic_.imageAttr.size;
  for (int id = 0; id < parser_.facehead_.size(); ++id) {
    memcpy(data_ + len, (char*)&(parser_.facehead_[id]),
           sizeof(longse::FACE_HEAD));
    len += sizeof(longse::FACE_HEAD);
    if (0 < parser_.facehead_[id].imageAttr.size)
      memcpy(data_ + len, parser_.cropvec_[id].c_str(),
             parser_.facehead_[id].imageAttr.size);
    len += parser_.facehead_[id].imageAttr.size;
  }
  WriteFinish();
  printf("lrh detect end write.\n");
  return true;
}

void SmartPack::FillFacePoint(longse::FACE_POINT_INFO* dst,
                              longse::FACE_POINT_INFO& src) {
  dst->faceID = src.faceID;
  dst->Point1.x = src.Point1.x;
  dst->Point1.y = src.Point1.y;
  dst->Point2.x = src.Point2.x;
  dst->Point2.y = src.Point2.y;
}

void SmartPack::FillFaceFrameHead(longse::FACE_FRAME_DATA_HEAD* head) {
  head->dwSoftwareVer = 1;
  head->dwFeatureDataVer = 1;
  head->frameTime = parser_.timestamp_;
  head->dwImageLen = sizeof(longse::PANORAMA_PICTURE_HEAD) +
                     parser_.panormapic_.imageAttr.size;
  head->dwFacePositionCnt = parser_.facepoint_.size();
  head->dwFaceCnt = parser_.facehead_.size();
  for (int id = 0; id < head->dwFaceCnt; ++id) {
    head->dwFaceDataLen[id] =
        sizeof(longse::FACE_HEAD) + parser_.facehead_[id].imageAttr.size;
  }
}

void SmartPack::FillFaceCmpHead(longse::FACE_COMPARE_PICTURE_HEAD* head) {
  head->usVersion = 1;
  head->usType = longse::FACE_COMPARE_SOURCE_FROM_IPC;
  head->usChlNo = 0;
  head->usSimilarity = (int)(parser_.rate_ * 100);
  head->ullTime = parser_.timestamp_;
  head->dwStreamFaceLen = sizeof(parser_.cropdetail_) +
                          parser_.cropdetail_.dwPictureLen +
                          parser_.cropdetail_.dwFeatureLen;
  head->dwGroupFaceLen = sizeof(parser_.dbdetail_) +
                         parser_.dbdetail_.dwPictureLen +
                         parser_.dbdetail_.dwFeatureLen;
  head->dwImageLen = 0;
  head->byMode = 0;
}

bool SmartPack::CheckDetectLen() {
  if (0 == parser_.panormapic_.imageAttr.size &&
      0 == parser_.facepoint_.size() && 0 == parser_.facehead_.size())
    return false;
  uint32_t totallen = 1 + sizeof(longse::FACE_FRAME_DATA_HEAD);
  totallen += sizeof(longse::FACE_POINT_INFO) * parser_.facepoint_.size();
  totallen += sizeof(longse::PANORAMA_PICTURE_HEAD) +
              parser_.panormapic_.imageAttr.size;
  for (int id = 0; id < parser_.facehead_.size(); ++id)
    totallen +=
        sizeof(longse::FACE_HEAD) + parser_.facehead_[id].imageAttr.size;
  if (totallen > kSmartDataLen) return false;
  return true;
}

bool SmartPack::CheckCmpLen() {
  if (0 == parser_.cropdetail_.dwPictureLen + parser_.dbdetail_.dwPictureLen)
    return false;
  uint32_t totallen = 1 + sizeof(longse::FACE_COMPARE_PICTURE_HEAD);
  totallen += sizeof(longse::FACE_DETAIL_INFO_ST);
  totallen +=
      parser_.cropdetail_.dwPictureLen + parser_.cropdetail_.dwFeatureLen;
  totallen += sizeof(longse::FACE_DETAIL_INFO_ST);
  totallen += parser_.dbdetail_.dwPictureLen + parser_.dbdetail_.dwFeatureLen;
  if (totallen > kSmartDataLen) return false;
  return true;
}

bool SmartPack::PackFaceCmp() {
  printf("lrh lib pack cmp detect.\n");
  if (!data_) return false;
  if (!CheckCmpLen()) return false;
  if (!CanWrite()) return false;
  printf("lrh cmp begin write.\n");
  uint32_t len = 0;
  *data_ = TYPE_CMP;
  len += 1;
  FillFaceCmpHead(
      reinterpret_cast<longse::FACE_COMPARE_PICTURE_HEAD*>(data_ + len));
  len += sizeof(longse::FACE_COMPARE_PICTURE_HEAD);
  memcpy(data_ + len, (char*)&(parser_.cropdetail_),
         sizeof(longse::FACE_DETAIL_INFO_ST));
  len += sizeof(longse::FACE_DETAIL_INFO_ST);
  // crop with no feature info
  memcpy(data_ + len, parser_.cropdata_.c_str(),
         parser_.cropdetail_.dwPictureLen);
  len += parser_.cropdetail_.dwPictureLen;
  memcpy(data_ + len, (char*)&(parser_.dbdetail_),
         sizeof(longse::FACE_DETAIL_INFO_ST));
  len += sizeof(longse::FACE_DETAIL_INFO_ST);
  memcpy(data_ + len, parser_.featinfo_.c_str(),
         parser_.dbdetail_.dwFeatureLen);
  len += parser_.dbdetail_.dwFeatureLen;
  memcpy(data_ + len, parser_.featuredata_.c_str(),
         parser_.dbdetail_.dwPictureLen);
  WriteFinish();
  return true;
}

bool SmartPack::PackLongseSmart() {
  if (parser_.isfacedetect_) return PackFaceDetect();
  if (parser_.isfacecmp_) return PackFaceCmp();
  return false;
}

SMART_TYPE SmartPack::GetSmartType() {
  if (0x1 == *data_) return TYPE_DETECT;
  if (0x2 == *data_) return TYPE_CMP;
  return TYPE_OTHER;
}

bool SmartPack::GetSmartData(const char* &data) {
  if (!CanRead())
    return false;
  data = data_ + 1;
  return true;
}

bool SmartPack::CanWrite() {
  // try 3 times
  int cnt = 3;
  while (cnt-- > 0) {
    uint8_t expect = 0;
    uint8_t desire = 0;
    if (writecondition_.compare_exchange_strong(expect, desire)) return true;
  }
  return false;
}

bool SmartPack::CanRead() {
  // try 3 times
  int cnt = 3;
  while (cnt-- > 0) {
    uint8_t expect = 1;
    uint8_t desire = 1;
    if (writecondition_.compare_exchange_strong(expect, desire)) return true;
  }
  return false;
}

bool SmartPack::WriteFinish() {
  uint8_t expect = 0;
  uint8_t desire = 1;
  writecondition_.compare_exchange_strong(expect, desire);
  return true;
}

bool SmartPack::ReadFinish() {
  uint8_t expect = 1;
  uint8_t desire = 0;
  writecondition_.compare_exchange_strong(expect, desire);
  return true;
}

}  // namespace horizon