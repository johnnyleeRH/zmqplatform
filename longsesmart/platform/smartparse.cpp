#include "smartparse.h"
#include "base64.h"
#include "ioAV.pb.h"
// #include "jpegsize.h"
#include <iostream>
#include "vcodec.h"
#include <sys/time.h>
#include <time.h>

#define MAX_FACE_NUM 4

void PrintSystemTm() {
  char buf[32] = {0};
	struct timeval tv;
	struct tm      tm;
	size_t         len = 28;
	memset(&tv, 0, sizeof(tv));
	memset(&tm, 0, sizeof(tm));
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);
	strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm);
	len = strlen(buf);
	sprintf(buf + len, ".%-6.3d", (int)(tv.tv_usec/1000)); 
	printf("%s\n", buf);
}

void PrintDetail(const char* buf, const int len) {
  for (int i = 0; i < len; ) {
    printf("0x%02x ", *(buf + i));
    if ((++i) % 16 == 0)
      printf("\n");
  }
  printf("\n");
}

SmartParse& SmartParse::Inst() {
  static SmartParse inst;
  return inst;
}

SmartParse::SmartParse() { vcodec_.reset(new Vcodec); }

bool SmartParse::Parse(const char* buf, const int len) {
  if (!CheckMsgHeader(buf, len)) return false;
  ioAV::Perception perception;
  if (!perception.ParseFromArray(buf + kmqheaderlen, len - kmqheaderlen))
    return false;
  timestamp_ = perception.timestamp();
  bool ret = false;
  switch (msgtype_) {
    case face::HobotSmartData:
      ret = ParseSmart(perception.body().c_str(), perception.body().size());
      break;
    case face::HobotFeatureIndex:
      ret = ParseFeature(perception.body().c_str(), perception.body().size());
      break;
    case face::HobotSnapshot:
      ret = ParseSnapshot(perception.body().c_str(), perception.body().size());
      break;
    case face::HobotBackGround:
      ret =
          ParseBackGround(perception.body().c_str(), perception.body().size());
      break;
    case face::HobotVideoFrameH264:
      ParseH264Frame(perception.body().c_str(), perception.body().size());
      break;
    default:
      break;
  }
  return ret;
}
/**
 * IPC h264 frame, I frame 1 persec, followed by 24 p frame, no b frame
*/
void SmartParse::ParseH264Frame(const char* buf, const int len) {
  switch (vcodec_->GetFrameType(buf, len)) {
    case Vcodec::FRAME_UNKNOWN:
      // PrintDetail(buf, len);
      break;
    case Vcodec::FRAME_I:
      // PrintSystemTm();
      break;
    case Vcodec::FRAME_P:
      break;
    default:
      break;
  }
}

bool SmartParse::ParseSmart(const char* buf, const int len) {
  return true;
  face::Result result;
  if (!result.ParseFromArray(buf, len)) return false;
  timestamp_ = result.timestamp();
  // printf("lrh smart timestamp %llu.\n", timestamp_);
  // printf("lrh smart result timestamp %llu.\n", result.timestamp());
  // printf("lrh target size %d.\n", result.targets_size());
  for (int targetno = 0;
       targetno < result.targets_size() && targetno < MAX_FACE_NUM;
       targetno++) {
    const face::Target& target = result.targets(targetno);
    // give up head info, only face info
    if (1 == target.type()) continue;
#if 0
    longse::FACE_POINT_INFO point;
    point.faceID = target.id();
    // printf("lrh faceID %d.\n", point.faceID);
    point.Point1.x = target.box().left();
    point.Point1.y = target.box().top();
    point.Point2.x = target.box().right();
    point.Point2.y = target.box().bottom();
    facepoint_.push_back(point);
#endif
    bool hasheadattr = false;
    // longse::FACE_HEAD head;
    for (int attrno = 0; attrno < target.attrs_size() && attrno < 0x10;
         attrno++) {
      if (1 <= target.attrs(attrno).type() &&
          5 >= target.attrs(attrno).type()) {
        if (!hasheadattr) {
          hasheadattr = true;
          // memset((char*)(&head), 0, sizeof(longse::FACE_HEAD));
          // head.faceID = target.id();
        }
      }
      // InsertHead(target.attrs(attrno).type(), target.attrs(attrno), head);
    }
    // if (hasheadattr) facehead_.push_back(head);
  }
  return true;
}

bool SmartParse::ParseFeature(const char* buf, const int len) {
  face::Capture capture;
  if (!capture.ParseFromArray(buf, len)) return false;
  for (int id = 0; id < capture.persons_size(); id++) {
    face::Person person = capture.persons(id);
    // printf("lrh person size %d feature size %d face size %d.\n",
    // capture.persons_size(), person.features_size(), person.faces_size());
    for (int feaid = 0; feaid < person.features_size() && feaid < 1; feaid++) {
      const face::Feature& feature = person.features(feaid);
      GetFeatureImg(feature.matched_img());
#if 0
      dbdetail_.dwPictureLen = featuredata_.size();
      scanhead_s(featuredata_, dbdetail_.dwPictureWidth,
                 dbdetail_.dwPictureHeight);
      dbdetail_.dwFeatureLen = feature.feat_info().size();
      strncpy(dbdetail_.stLibFeatureInfo.szName, feature.name().c_str(), 32);
#endif
      featinfo_ = feature.feat_info();
      rate_ = feature.matched_rate();
    }
    for (int faceid = 0; faceid < person.faces_size() && faceid < 1; faceid++) {
      const face::Face& personface = person.faces(faceid);
#if 0
      cropdetail_.dwPictureWidth =
          personface.pic_box().right() - personface.pic_box().left();
      cropdetail_.dwPictureHeight =
          personface.pic_box().bottom() - personface.pic_box().top();
      cropdetail_.dwPictureLen = personface.snapshot().size();
#endif
      cropdata_ = personface.snapshot();
      timestamp_ = personface.timestamp();
      printf("lrh feature timestamp %llu.\n", timestamp_);
      // printf("lrh feature timestamp is %llu\n", personface.timestamp());
      // printf("lrh feature snap len is %d, track size %d.\n",
      // cropdata_.size(), person.track_id_size());
    }
#if 0
    for (int trkid = 0; trkid < person.track_id_size(); ++trkid) {
      dbdetail_.stLibFeatureInfo.ullFaceID = person.track_id(trkid);
    }
#endif
  }
  return true;
}

bool SmartParse::ParseSnapshot(const char* buf, const int len) {
  face::Capture capture;
  if (!capture.ParseFromArray(buf, len)) return false;
  for (int id = 0; id < capture.persons_size(); ++id) {
    const face::Person& person = capture.persons(id);
#if 0
    std::vector<longse::FACE_POINT_INFO> pointvec;
    std::vector<longse::FACE_HEAD> headvec;
#endif
    for (int faceid = 0; faceid < person.faces_size(); ++faceid) {
#if 0
      longse::FACE_HEAD head;
      longse::FACE_POINT_INFO point;
      memset((char*)&head, 0, sizeof(head));
#endif
      const face::Face& personface = person.faces(faceid);
      timestamp_ = personface.timestamp();
// printf("lrh snapshot timestamp %llu.\n", timestamp_);
// printf("lrh snapshot timestamp %llu.\n", personface.timestamp());
#if 0
      point.Point1.x = personface.face_box().left();
      point.Point1.y = personface.face_box().top();
      point.Point2.x = personface.face_box().right();
      point.Point2.y = personface.face_box().bottom();
      head.imageAttr.size = personface.snapshot().size();
      head.imageAttr.width =
          personface.pic_box().right() - personface.pic_box().left();
      head.imageAttr.height =
          personface.pic_box().bottom() - personface.pic_box().top();
#endif
      // cropvec_.push_back(personface.snapshot());
      for (int attrno = 0; attrno < personface.attrs_size() && attrno < 0x10;
           attrno++) {
        if (1 == personface.attrs(attrno).type())
          printf("lrh lib in age is %d.\n", personface.attrs(attrno).int_val());
      }
      // headvec.push_back(head);
      // pointvec.push_back(point);
    }
#if 0
    for (int trkid = 0;
         trkid < person.track_id_size() && trkid < person.faces_size();
         ++trkid) {
      pointvec[trkid].faceID = person.track_id(trkid);
      headvec[trkid].faceID = person.track_id(trkid);
    }
    for (int id = 0; id < headvec.size(); ++id)
      facehead_.push_back(headvec[id]);
    for (int id = 0; id < pointvec.size(); ++id)
      facepoint_.push_back(pointvec[id]);
#endif
  }
  return true;
}

// background with no track and no attr
bool SmartParse::ParseBackGround(const char* buf, const int len) {
  // printf("lrh backgroup recv %lu.\n", timestamp_);
  static int cnt = 10;
  face::Capture capture;
  if (!capture.ParseFromArray(buf, len)) return false;
  for (int id = 0; id < capture.persons_size() & id < 1; ++id) {
    const face::Person& person = capture.persons(id);
    // longse::FACE_POINT_INFO point;
    // longse::FACE_HEAD head;
    for (int faceid = 0; faceid < person.faces_size() & faceid < 1; ++faceid) {
      timestamp_ = person.faces(faceid).timestamp();
      printf("lrh background timestamp %llu.\n", timestamp_);
      // printf("lrh background timestamp %llu.\n",
      // person.faces(faceid).timestamp());
      // panormapic_.imageAttr.size = person.faces(faceid).snapshot().size();
      // scanhead_s(person.faces(faceid).snapshot(),
      // panormapic_.imageAttr.width,
      //            panormapic_.imageAttr.height);
      backgrounddata_ = person.faces(faceid).snapshot();
    }
  }
  return true;
  // return ParseSnapshot(buf, len);
}

void SmartParse::GetFeatureImg(const std::string& matchedimg) {
  // check base64 or raw jpeg
  if (matchedimg.size() <= 2) {
    featuredata_ = matchedimg;
    return;
  }
  // not base64
  if (0xFF == matchedimg[0] && 0xD8 == matchedimg[1]) {
    featuredata_ = matchedimg;
  } else {
    featuredata_ = base64_decode(matchedimg);
  }
}

bool SmartParse::CheckMsgHeader(const char* buf, const int len) {
  if (len <= kmqheaderlen) return false;
  msgtype_ = face::HobotDefaultFrame;
  if (0 == strncmp(buf, ktypesmart.c_str(), 4)) msgtype_ = face::HobotSmartData;
  if (0 == strncmp(buf, ktypefeature.c_str(), 4))
    msgtype_ = face::HobotFeatureIndex;
  if (0 == strncmp(buf, ktypecrop.c_str(), 4)) msgtype_ = face::HobotSnapshot;
  if (0 == strncmp(buf, ktypebackground.c_str(), 4))
    msgtype_ = face::HobotBackGround;
  if (0 == strncmp(buf, ktypeh264.c_str(), 4))
    msgtype_ = face::HobotVideoFrameH264;
  if (face::HobotDefaultFrame == msgtype_) return false;
  return true;
}