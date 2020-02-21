#ifndef SRC_SMARTDATATRANS_HPP
#define SRC_SMARTDATATRANS_HPP

namespace horizon {

/*callback func for face detect data*/
typedef void (*FaceDetectFunc)(const char* data);
/*callback func for face feature compare data*/
typedef void (*FaceCmpFunc)(const char* data);

class SmartTrans {
 public:
  /*
   * Init Smart Data Translate
   * @ret：if init success, return true;
   * else return false, and use GetErrorInfo to get the error msg.
   */
  virtual bool Init() = 0;
  /*
   * Registe face detect callback func
   */
  virtual void RegisteFaceDetectHandle(FaceDetectFunc detect) = 0;
  /*
   * Registe face compare callback func
   */
  virtual void RegisteFaceCmpHandle(FaceCmpFunc cmp) = 0;
  /*
   * give up smart sdk use
   */
  virtual void Exit() = 0;
  /*
   * get the error msg while error happen.
   */
  virtual const char* GetErrorInfo() = 0;
   /*
   * get smart instance.
   */
  static SmartTrans& GetSmartTransInst();
};

}  // namespace horizon

#endif  // SRC_SMARTDATATRANS_HPP