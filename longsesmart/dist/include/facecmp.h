#ifndef SMARTDATA_FACECMP_HPP
#define SMARTDATA_FACECMP_HPP

#include "facetype.h"

#pragma pack(4)

namespace longse {

enum FACE_COMPARE_SOURCE {
  FACE_COMPARE_SOURCE_NULL = 0,
  FACE_COMPARE_SOURCE_FROM_NVR,
  FACE_COMPARE_SOURCE_FROM_IPC
};

enum FACE_BLACK_WHITE_TYPE {
  FACE_TYPE_NULL = 0,
  FACE_TYPE_BLACK,
  FACE_TYPE_WHITE
};

#define FACE_COMPARE_VERSION 0x01

typedef struct _face_compare_picture_head {
  cuint16 usVersion;
  cuint16 usType;
  cuint16 usChlNo;
  cuint16 usSimilarity;
  cuint64 ullTime;
  cuint32 dwStreamFaceLen;
  cuint32 dwGroupFaceLen;
  cuint32 dwImageLen;
  cuint8  byMode;
  cuint8  byReserver[227];
} FACE_COMPARE_PICTURE_HEAD;

typedef struct _face_lib_feature_info_st_ {
  unsigned long long ullFaceID;
  char szName[32];
  cuint8 byReserver[984];
} FACE_LIB_FEATURE_INFO_ST;

typedef struct _face_stream_feature_info_st_ {
  cuint8 byReserver[1024];
} FACE_STREAM_FEATURE_INFO_ST;

typedef struct _face_detail_info_st_ {
  cuint32 dwPictureType;  // 0: jpeg
  cuint32 dwPictureWidth;
  cuint32 dwPictureHeight;
  cuint32 dwPictureLen;
  cuint32 dwFeatureLen;
  union {
    FACE_STREAM_FEATURE_INFO_ST stStreamFeatureInfo;
    FACE_LIB_FEATURE_INFO_ST stLibFeatureInfo;
  };
  cuint8 byReserver[32];
} FACE_DETAIL_INFO_ST;

}

#pragma pack()

#endif //  SMARTDATA_FACECMP_HPP