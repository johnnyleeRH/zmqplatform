#ifndef SMARTDATA_FACEDETECT_HPP
#define SMARTDATA_FACEDETECT_HPP

#pragma pack(4)

namespace longse {

#define MAX_FACE_NUM 100

/*gender enum*/
typedef enum FACE_GENDER_ENUM {
  FACE_GENDER_MALE = 0x00,
  FACE_GENDER_FEMALE,
  FACE_GENDER_MAX
} FACE_GENDER_TYPE_E;

/*minority enum*/
typedef enum FACE_MINORITY_ENUM {
  FACE_MINORITY_TAOISM = 0x00,
  FACE_MINORITY_OTHERS,
  FACE_MINORITY_MAX
} FACE_MINORITY_TYPE_E;

/*eye occ enum*/
typedef enum FACE_EYE_OCC_ENUM {
  FACE_EYEOCC_NO_OCC = 0x00,
  FACE_EYEOCC_GLASSES,
  FACE_EYEOCC_SUNGLASSES,
  FACE_EYEOCC_OTHERS,
  FACE_EYEOCC_MAX
} FACE_EYE_OCC_E;

/*mouth occ enum*/
typedef enum FACE_MOUTH_OCC_ENUM {
  FACE_MOUTHOCC_NOOCC = 0x00,
  FACE_MOUTHOCC_MASK,
  FACE_MOUTHOCC_RESPIRATOR,
  FACE_MOUTHOCC_OTHERS,
  FACE_MOUTHOCC_MAX
} FACE_MOUTH_OCC_E;

/*head wear enum*/
typedef enum FACE_HEADWEAR_ENUM {
  FACE_HEADWEAR_HAT = 0x00,
  FACE_HEADWEAR_LITTLE_HAIR,
  FACE_HEADWEAR_SHORT_HAIR,
  FACE_HEADWEAR_LONG_HAIR,
  FACE_HEADWEAR_OTHERS,
  FACE_HEADWEAR_MAX
} FACE_HEADWEAR_E;

/*class enum*/
typedef enum FACE_CONFIDENCE_CLASS_ENUM {
  FACE_CONFIDENCE_UNKNOWN = 0x00,
  FACE_CONFIDENCE_NEGATIVE,
  FACE_CONFIDENCE_POSITIVE,
  FACE_CONFIDENCE_MAX
} FACE_CONFIDENCE_CLASS_E;

/*nose occ enum*/
typedef enum FACE_NOSE_OCC_ENUM {
  FACE_NOSEOCC_NOOCC = 0x00,
  FACE_NOSEOCC_MASK,
  FACE_NOSEOCC_RESPIRATOR,
  FACE_NOSEOCC_OTHERS,
  FACE_NOSEOCC_NUMS
} FACE_NOSE_OCC_E;

/*beard enum*/
typedef enum FACE_BEARD_ENUM {
  FACE_BEARD_NO = 0x00,
  FACE_BEARD_MOUSTACHE,
  FACE_BEARD_WHISKER,
  FACE_BEARD_OTHERS,
  FACE_BEARD_NUMS
} FACE_BEARD_E;

/*3D pose*/
typedef struct FACE_POSE_ST {
  float roll;
  float pitch;
  float yaw;
} FACE_POSE_S;

typedef struct FACE_FRAME_DATA_HEAD_S {
  unsigned int dwSoftwareVer;
  unsigned int dwFeatureDataVer;
  long long frameTime;
  unsigned int dwImageLen;
  unsigned int dwFacePositionCnt;
  unsigned int dwFaceCnt;
  unsigned int dwFaceDataLen[MAX_FACE_NUM];
  unsigned int reserved[4];
} FACE_FRAME_DATA_HEAD;

typedef struct IMAGE_ATTR_S {
  unsigned int size;
  unsigned int ImageType;  // 0: jpg
  unsigned int width;
  unsigned int height;
} IMAGE_ATTR;

typedef struct PANORAMA_PICTURE_HEAD_S {
  IMAGE_ATTR imageAttr;
  unsigned int reserved[4];
} PANORAMA_PICTURE_HEAD;

typedef struct U32_Point_S {
  unsigned int x;
  unsigned int y;
} U32_Point;

/*detect rectangle*/
typedef struct FACE_POINT_INFO_S {
  unsigned int faceID;
  U32_Point Point1;
  U32_Point Point2;
} FACE_POINT_INFO;

typedef struct FACE_ATTRIBUTE_INFO_S {
  unsigned int age;
  FACE_GENDER_TYPE_E gender;
  FACE_MINORITY_TYPE_E minority;
  FACE_EYE_OCC_E eyeocc;
  FACE_MOUTH_OCC_E mouthocc;
  FACE_HEADWEAR_E headwear;
  FACE_NOSE_OCC_E noseocc;
  FACE_BEARD_E beard;
  unsigned int eye_close;
  unsigned int mouth_open;
  unsigned int reserved[22];
} FACE_ATTRIBUTE_INFO;

typedef struct FACE_HEAD_S {
  IMAGE_ATTR imageAttr;
  FACE_POSE_S facePose;
  float blur;
  FACE_ATTRIBUTE_INFO faceAtt;
  unsigned int faceID;
  unsigned int featureSize;
  unsigned int reserved[6];
} FACE_HEAD;

}

#pragma pack()

#endif //  SMARTDATA_FACEDETECT_HPP