#ifndef SRC_MSGPARSE_HPP
#define SRC_MSGPARSE_HPP

#include <string>
#include <vector>
#include "face.pb.h"
#include "facecmp.h"
#include "facedetect.h"

namespace horizon {

const std::string ktypesmart = "0x31";
const std::string ktypefeature = "0x32";
const std::string ktypecrop = "0x33";
const std::string ktypebackground = "0x34";
const int kmqheaderlen = 24;

class MsgParse {
public:
  static MsgParse& Inst();
  bool Parse(const char* buf, const int len);
public:
  face::HobotMediaType msgtype_;
  std::vector<longse::FACE_POINT_INFO> facepoint_;
  std::vector<longse::FACE_HEAD> facehead_;
  std::vector<std::string> cropvec_;
  longse::PANORAMA_PICTURE_HEAD panormapic_;
  longse::FACE_DETAIL_INFO_ST cropdetail_;
  longse::FACE_DETAIL_INFO_ST dbdetail_;
  std::string featuredata_;
  std::string cropdata_;
  std::string backgrounddata_;
  std::string featinfo_;
  bool isfacedetect_;
  bool isfacecmp_;
  uint64_t timestamp_;
  float rate_;
private:
  void ResetFaceDetect();
  void ResetFaceCmp();
  void InsertHead(const int type, const face::Attribute& attr, longse::FACE_HEAD& head);
  void GetFeatureImg(const std::string& matchedimg);
  bool CheckMsgHeader(const char* buf, const int len);
  bool ParseSmart(const char* buf, const int len);
  bool ParseFeature(const char* buf, const int len);
  bool ParseSnapshot(const char* buf, const int len);
  bool ParseBackGround(const char* buf, const int len);
};

}

#endif //  SRC_MSGPARSE_HPP