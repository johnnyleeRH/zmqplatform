#include "msgparse.h"
#include "ioAV.pb.h"
#include "base64.h"
#include "jpegsize.h"

namespace horizon {

MsgParse& MsgParse::Inst() {
  static MsgParse inst;
  return inst;
}

bool MsgParse::Parse(const char* buf, const int len) {
  if (!CheckMsgHeader(buf, len))
    return false;
  ioAV::Perception perception;
  if (!perception.ParseFromArray(buf + kmqheaderlen, len - kmqheaderlen))
    return false;
  timestamp_ = perception.timestamp();
  printf("lrh ioav timestamp %llu.\n", timestamp_);
  isfacecmp_ = false;
  isfacedetect_ = false;
  bool ret = false;
  switch (msgtype_) {
    case face::HobotSmartData:
      ret = ParseSmart(perception.body().c_str(), perception.body().size());
      isfacedetect_ = true;
      break;
    case face::HobotFeatureIndex:
      ret = ParseFeature(perception.body().c_str(), perception.body().size());
      isfacecmp_ = true;
      break;
    case face::HobotSnapshot:
      ret = ParseSnapshot(perception.body().c_str(), perception.body().size());
      isfacedetect_ = true;
      break;
    case face::HobotBackGround:
      ret = ParseBackGround(perception.body().c_str(), perception.body().size());
      isfacedetect_ = true;
      break;
    default:
      break;
  }
  return ret;
}

bool MsgParse::ParseSmart(const char* buf, const int len) {
  // printf("lrh smart recv, %lu.\n", timestamp_);
  face::Result result;
  if (!result.ParseFromArray(buf, len))
    return false;
  ResetFaceDetect();
  timestamp_ = result.timestamp();
  printf("lrh smart timestamp %llu.\n", timestamp_);
  // printf("lrh smart result timestamp %llu.\n", result.timestamp());
  // printf("lrh target size %d.\n", result.targets_size());
  for (int targetno = 0;
        targetno < result.targets_size() && targetno < MAX_FACE_NUM;
        targetno++) {
    const face::Target& target = result.targets(targetno);
    // give up head info, only face info
    if (1 == target.type())
      continue;
    longse::FACE_POINT_INFO point;
    point.faceID = target.id();
    // printf("lrh faceID %d.\n", point.faceID);
    point.Point1.x = target.box().left();
    point.Point1.y = target.box().top();
    point.Point2.x = target.box().right();
    point.Point2.y = target.box().bottom();
    facepoint_.push_back(point);
    bool hasheadattr = false;
    longse::FACE_HEAD head;
    for (int attrno = 0;
          attrno < target.attrs_size() && attrno < 0x10;
          attrno++) {
      if (1 <= target.attrs(attrno).type()
        && 5 >= target.attrs(attrno).type()) {
        if (!hasheadattr) {
          hasheadattr = true;
          memset((char*)(&head), 0, sizeof(longse::FACE_HEAD));
          head.faceID = target.id();
        }
      }
      InsertHead(target.attrs(attrno).type(), target.attrs(attrno), head);
    }
    if (hasheadattr)
      facehead_.push_back(head);
  }
  return true;
}

bool MsgParse::ParseFeature(const char* buf, const int len) {
  // printf("lrh feature recv, %lu.\n", timestamp_);
  face::Capture capture;
  if (!capture.ParseFromArray(buf, len))
    return false;
  ResetFaceCmp();
  for (int id = 0; id < capture.persons_size(); id++) {
    face::Person person = capture.persons(id);
    // printf("lrh person size %d feature size %d face size %d.\n",
      // capture.persons_size(), person.features_size(), person.faces_size());
    for (int feaid = 0; feaid < person.features_size() && feaid < 1; feaid++) {
      const face::Feature& feature = person.features(feaid);
      GetFeatureImg(feature.matched_img());
      dbdetail_.dwPictureLen = featuredata_.size();
      scanhead_s(featuredata_, dbdetail_.dwPictureWidth, dbdetail_.dwPictureHeight);
      dbdetail_.dwFeatureLen = feature.feat_info().size();
      featinfo_ = feature.feat_info();
      strncpy(dbdetail_.stLibFeatureInfo.szName, feature.name().c_str(), 32);
      rate_ = feature.matched_rate();
    }
    for (int faceid = 0; faceid < person.faces_size() && faceid < 1; faceid++) {
      const face::Face& personface = person.faces(faceid);
      cropdetail_.dwPictureWidth =
        personface.pic_box().right() - personface.pic_box().left();
      cropdetail_.dwPictureHeight =
        personface.pic_box().bottom() - personface.pic_box().top();
      cropdetail_.dwPictureLen = personface.snapshot().size();
      cropdata_ = personface.snapshot();
      timestamp_ = personface.timestamp();
      printf("lrh feature timestamp %llu.\n", timestamp_);
      // printf("lrh feature timestamp is %llu\n", personface.timestamp());
      // printf("lrh feature snap len is %d, track size %d.\n", cropdata_.size(), person.track_id_size());
    }
    for (int trkid = 0; trkid < person.track_id_size(); ++trkid) {
      dbdetail_.stLibFeatureInfo.ullFaceID = person.track_id(trkid);
    }
  }
  return true;
}

bool MsgParse::ParseSnapshot(const char* buf, const int len) {
  // printf("lrh snapshot recv, %lu.\n", timestamp_);
  face::Capture capture;
  if (!capture.ParseFromArray(buf, len))
    return false;
  ResetFaceDetect();
  for (int id = 0; id < capture.persons_size(); ++id) {
    const face::Person& person = capture.persons(id);
    std::vector<longse::FACE_POINT_INFO> pointvec;
    std::vector<longse::FACE_HEAD> headvec;
    for (int faceid = 0; faceid < person.faces_size(); ++faceid) {
      longse::FACE_HEAD head;
      longse::FACE_POINT_INFO point;
      memset((char*)&head, 0, sizeof(head));
      const face::Face& personface = person.faces(faceid);
      timestamp_ = personface.timestamp();
      printf("lrh snapshot timestamp %llu.\n", timestamp_);
      // printf("lrh snapshot timestamp %llu.\n", personface.timestamp());
      point.Point1.x = personface.face_box().left();
      point.Point1.y = personface.face_box().top();
      point.Point2.x = personface.face_box().right();
      point.Point2.y = personface.face_box().bottom();
      head.imageAttr.size = personface.snapshot().size();
      head.imageAttr.width =
        personface.pic_box().right() - personface.pic_box().left();
      head.imageAttr.height =
        personface.pic_box().bottom() - personface.pic_box().top();
      cropvec_.push_back(personface.snapshot());
      for (int attrno = 0;
          attrno < personface.attrs_size() && attrno < 0x10;
          attrno++) {
        if (1 == personface.attrs(attrno).type())
          printf("lrh lib in age is %d.\n", personface.attrs(attrno).int_val());
        InsertHead(personface.attrs(attrno).type(), personface.attrs(attrno), head);
      }
      headvec.push_back(head);
      pointvec.push_back(point);
    }
    for (int trkid = 0;
      trkid < person.track_id_size() && trkid < person.faces_size(); ++trkid) {
      pointvec[trkid].faceID = person.track_id(trkid);
      headvec[trkid].faceID = person.track_id(trkid);
    }
    for (int id = 0; id < headvec.size(); ++id)
      facehead_.push_back(headvec[id]);
    for (int id = 0; id < pointvec.size(); ++id)
      facepoint_.push_back(pointvec[id]);
  }
  return true;
}

// background with no track and no attr
bool MsgParse::ParseBackGround(const char* buf, const int len) {
  // printf("lrh backgroup recv %lu.\n", timestamp_);
  static int cnt = 10;
  face::Capture capture;
  if (!capture.ParseFromArray(buf, len))
    return false;
  ResetFaceDetect();
  for (int id = 0; id < capture.persons_size() & id < 1; ++id) {
    const face::Person& person = capture.persons(id);
    longse::FACE_POINT_INFO point;
    longse::FACE_HEAD head;
    for (int faceid = 0; faceid < person.faces_size() & faceid < 1; ++faceid) {
      timestamp_ = person.faces(faceid).timestamp();
      printf("lrh background timestamp %llu.\n", timestamp_);
      // printf("lrh background timestamp %llu.\n", person.faces(faceid).timestamp());
      panormapic_.imageAttr.size = person.faces(faceid).snapshot().size();
      scanhead_s(person.faces(faceid).snapshot(), panormapic_.imageAttr.width, panormapic_.imageAttr.height);
      backgrounddata_ = person.faces(faceid).snapshot();
    }
  }
  return true;
  // return ParseSnapshot(buf, len);
}

void MsgParse::ResetFaceDetect() {
  facepoint_.clear();
  facehead_.clear();
  cropvec_.clear();
  memset((char*)&panormapic_, 0, sizeof(longse::PANORAMA_PICTURE_HEAD));
}

void MsgParse::ResetFaceCmp() {
  memset((char*)&cropdetail_, 0, sizeof(longse::FACE_DETAIL_INFO_ST));
  memset((char*)&dbdetail_, 0, sizeof(longse::FACE_DETAIL_INFO_ST));
  rate_ = 0.0;
}

void MsgParse::GetFeatureImg(const std::string& matchedimg) {
  //check base64 or raw jpeg
  if (matchedimg.size() <= 2) {
    featuredata_ = matchedimg;
    return;
  }
  //not base64
  if (0xFF == matchedimg[0] && 0xD8 == matchedimg[1]) {
    featuredata_ = matchedimg;
  } else {
    featuredata_ = base64_decode(matchedimg);
  }
}

bool MsgParse::CheckMsgHeader(const char* buf, const int len) {
  if (len <= kmqheaderlen)
    return false;
  msgtype_ = face::HobotDefaultFrame;
  if (0 == strncmp(buf, ktypesmart.c_str(), 4))
    msgtype_ = face::HobotSmartData;
  if (0 == strncmp(buf, ktypefeature.c_str(), 4))
    msgtype_ = face::HobotFeatureIndex;
  if (0 == strncmp(buf, ktypecrop.c_str(), 4))
    msgtype_ = face::HobotSnapshot;
  if (0 == strncmp(buf, ktypebackground.c_str(), 4))
    msgtype_ = face::HobotBackGround;
  if (face::HobotDefaultFrame == msgtype_)
    return false;
  return true;
}

void MsgParse::InsertHead(const int type, const face::Attribute& attr, longse::FACE_HEAD& head) {
  switch (type) {
    case 1:
      // printf("lrh lib age is %d.\n", attr.int_val());
      head.faceAtt.age = attr.int_val();
      break;
    case 2:
      if (0 == strncmp(attr.str_val().c_str(), "male", 4))
        head.faceAtt.gender = longse::FACE_GENDER_MALE;
      else if (0 == strncmp(attr.str_val().c_str(), "female", 6))
        head.faceAtt.gender = longse::FACE_GENDER_FEMALE;
      break;
    case 3:
      // printf("lrh has yaw info.\n");
      head.facePose.yaw = attr.int_val();
      break;
    case 4:
      head.facePose.pitch = attr.int_val();
      break;
    case 5:
      head.facePose.roll = attr.int_val();
      break;
    default:
      break;
  }
}

}