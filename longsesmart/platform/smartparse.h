#ifndef SRC_SMARTPARSE_HPP
#define SRC_SMARTPARSE_HPP

#include <string>
#include <vector>
#include <memory>
#include "face.pb.h"

const std::string ktypesmart = "0x31";
const std::string ktypefeature = "0x32";
const std::string ktypecrop = "0x33";
const std::string ktypebackground = "0x34";
const std::string ktypeh264 = "0x01";
const int kmqheaderlen = 24;

class Vcodec;
class SmartParse {
public:
  static SmartParse& Inst();
  bool Parse(const char* buf, const int len);
public:
  face::HobotMediaType msgtype_;
  std::vector<std::string> cropvec_;
  std::string featuredata_;
  std::string cropdata_;
  std::string backgrounddata_;
  std::string featinfo_;
  uint64_t timestamp_;
  float rate_;
private:
  SmartParse();
  std::shared_ptr<Vcodec> vcodec_;
private:
  void GetFeatureImg(const std::string& matchedimg);
  bool CheckMsgHeader(const char* buf, const int len);
  bool ParseSmart(const char* buf, const int len);
  bool ParseFeature(const char* buf, const int len);
  bool ParseSnapshot(const char* buf, const int len);
  bool ParseBackGround(const char* buf, const int len);
  void ParseH264Frame(const char* buf, const int len);
};

#endif //  SRC_SMARTPARSE_HPP