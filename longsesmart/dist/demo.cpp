#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include "facecmp.h"
#include "facedetect.h"
#include "smartdatatrans.h"

#define WIDTH_LIMIT_PRINT(width) std::cout << std::setw(width)

void SavePic(std::string type, const char* data, int len) {
  return;
  static unsigned int cnt = 0;
  if (len <= 0) return;
  std::string path = "/dev/" + type;
  char buf[30] = {0};
  snprintf(buf, 30, "_%u.jpg", ++cnt);
  path += buf;
  FILE* fOut = fopen(path.c_str(), "wb");
  if (!fOut) {
    std::cout << "open file " << path << "failed, error " << strerror(errno)
              << std::endl;
    return;
  }
  fwrite(data, sizeof(char), len, fOut);
  fclose(fOut);
}

void FaceDetectCallback(const char* data) {
  return;
  std::cout << "##################FACE DETECT##################" << std::endl;
  const longse::FACE_FRAME_DATA_HEAD* head =
      reinterpret_cast<const longse::FACE_FRAME_DATA_HEAD*>(data);
  WIDTH_LIMIT_PRINT(20) << "soft_ver:" << head->dwSoftwareVer << std::endl;
  WIDTH_LIMIT_PRINT(20) << "feature_ver:" << head->dwFeatureDataVer
                        << std::endl;
  WIDTH_LIMIT_PRINT(20) << "frame_time:" << head->frameTime << std::endl;
  WIDTH_LIMIT_PRINT(20) << "panorama_len:" << head->dwImageLen << std::endl;
  WIDTH_LIMIT_PRINT(20) << "position_cnt:" << head->dwFacePositionCnt
                        << std::endl;
  WIDTH_LIMIT_PRINT(20) << "face_cnt:" << head->dwFaceCnt << std::endl;
  const longse::FACE_POINT_INFO* point =
      reinterpret_cast<const longse::FACE_POINT_INFO*>(
          data + sizeof(longse::FACE_FRAME_DATA_HEAD));
  for (int id = 0; id < head->dwFacePositionCnt; ++id) {
    WIDTH_LIMIT_PRINT(20) << "point:" << id;
    std::cout << " faceid:" << point->faceID;
    std::cout << " point1_x:" << point->Point1.x;
    std::cout << " point1_y:" << point->Point1.y;
    std::cout << " point2_x:" << point->Point2.x;
    std::cout << " point2_y:" << point->Point2.y;
    std::cout << std::endl;
    point++;
  }
  const longse::PANORAMA_PICTURE_HEAD* panorama =
      reinterpret_cast<const longse::PANORAMA_PICTURE_HEAD*>(point);
  WIDTH_LIMIT_PRINT(20) << "panorama_size:" << panorama->imageAttr.size
                        << std::endl;
  WIDTH_LIMIT_PRINT(20) << "panorama_width:" << panorama->imageAttr.width
                        << std::endl;
  WIDTH_LIMIT_PRINT(20) << "panorama_height:" << panorama->imageAttr.height
                        << std::endl;
  if (panorama->imageAttr.size > 0)
    SavePic("background",
            reinterpret_cast<const char*>(panorama) +
                sizeof(longse::PANORAMA_PICTURE_HEAD),
            panorama->imageAttr.size);
  const longse::FACE_HEAD* facehead =
      reinterpret_cast<const longse::FACE_HEAD*>(
          reinterpret_cast<const char*>(panorama) +
          sizeof(longse::PANORAMA_PICTURE_HEAD) + panorama->imageAttr.size);
  for (int id = 0; id < head->dwFaceCnt; ++id) {
    std::cout << "##################Face " << facehead->faceID
              << "##################" << std::endl;
    // has face crop
    if (facehead->imageAttr.size > 0) {
      std::cout << "image: size " << facehead->imageAttr.size;
      std::cout << " width " << facehead->imageAttr.width << " height "
                << facehead->imageAttr.height;
      std::cout << std::endl;
      std::cout << "faceattr: age " << facehead->faceAtt.age;
      if (longse::FACE_GENDER_MALE == facehead->faceAtt.gender)
        std::cout << " gender male" << std::endl;
      else if (longse::FACE_GENDER_FEMALE == facehead->faceAtt.gender)
        std::cout << " gemder female" << std::endl;
      SavePic(
          "crop",
          reinterpret_cast<const char*>(facehead) + sizeof(longse::FACE_HEAD),
          facehead->imageAttr.size);
    } else {
      WIDTH_LIMIT_PRINT(20) << "pose:";
      std::cout << "yaw " << facehead->facePose.yaw;
      std::cout << " pitch " << facehead->facePose.pitch;
      std::cout << " roll " << facehead->facePose.roll << std::endl;
    }
    facehead = reinterpret_cast<const longse::FACE_HEAD*>(
        reinterpret_cast<const char*>(facehead) + sizeof(longse::FACE_HEAD) +
        facehead->imageAttr.size);
  }
}

void FaceCmpCallback(const char* data) {
  return;
  std::cout << "##################FACE CMP##################" << std::endl;
  const longse::FACE_COMPARE_PICTURE_HEAD* head =
      reinterpret_cast<const longse::FACE_COMPARE_PICTURE_HEAD*>(data);
  WIDTH_LIMIT_PRINT(20) << "cmp_version:" << head->usVersion << std::endl;
  WIDTH_LIMIT_PRINT(20) << "cmp_similarity:" << head->usSimilarity << std::endl;
  WIDTH_LIMIT_PRINT(20) << "cmp_time:" << head->ullTime << std::endl;
  WIDTH_LIMIT_PRINT(20) << "cmp_streamfacelen:" << head->dwStreamFaceLen
                        << std::endl;
  WIDTH_LIMIT_PRINT(20) << "cmp_groupfacelen:" << head->dwGroupFaceLen
                        << std::endl;
  std::cout << "##################CROP DETAIL##################" << std::endl;
  const longse::FACE_DETAIL_INFO_ST* detail =
      reinterpret_cast<const longse::FACE_DETAIL_INFO_ST*>(
          data + sizeof(longse::FACE_COMPARE_PICTURE_HEAD));
  WIDTH_LIMIT_PRINT(20) << "picwidth:" << detail->dwPictureWidth << std::endl;
  WIDTH_LIMIT_PRINT(20) << "picheight:" << detail->dwPictureHeight << std::endl;
  WIDTH_LIMIT_PRINT(20) << "picsize:" << detail->dwPictureLen << std::endl;
  // crop with no feature
  SavePic("feature_crop",
          reinterpret_cast<const char*>(detail) +
              sizeof(longse::FACE_DETAIL_INFO_ST) + detail->dwFeatureLen,
          detail->dwPictureLen);
  std::cout << "##################DB DETAIL##################" << std::endl;
  detail = reinterpret_cast<const longse::FACE_DETAIL_INFO_ST*>(
      reinterpret_cast<const char*>(detail) + head->dwStreamFaceLen);
  WIDTH_LIMIT_PRINT(20) << "picwidth:" << detail->dwPictureWidth << std::endl;
  WIDTH_LIMIT_PRINT(20) << "picheight:" << detail->dwPictureHeight << std::endl;
  WIDTH_LIMIT_PRINT(20) << "picsize:" << detail->dwPictureLen << std::endl;
  WIDTH_LIMIT_PRINT(20) << "featurelen:" << detail->dwFeatureLen << std::endl;
  if (detail->dwPictureLen > 0)
    SavePic("db_pic",
            reinterpret_cast<const char*>(detail) +
                sizeof(longse::FACE_DETAIL_INFO_ST) + detail->dwFeatureLen,
            detail->dwPictureLen);
}

int main() {
  horizon::SmartTrans& inst = horizon::SmartTrans::GetSmartTransInst();
  if (!inst.Init()) {
    std::cout << inst.GetErrorInfo() << std::endl;
    return -1;
  }
  inst.RegisteFaceCmpHandle(FaceCmpCallback);
  inst.RegisteFaceDetectHandle(FaceDetectCallback);
  while (true) {
    sleep(1);
  }
  return 0;
}