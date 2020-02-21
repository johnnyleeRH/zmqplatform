#ifndef SRC_EVENTTRIGGER_H_
#define SRC_EVENTTRIGGER_H_

#include <pthread.h>
#include "smartpack.h"
#include "smartdatatrans.h"

namespace horizon {

class EventTrigger {
public:
  bool Start();
  void Stop();
  void RegisteDetectHandle(FaceDetectFunc detect);
  void RegisteCmpHandle(FaceCmpFunc cmp);
  EventTrigger();
private:
  static void* TriggerThread(void *param);
private:
  SmartPack& pack_;
  pthread_t td_;
  bool start_;
  FaceDetectFunc detect_;
  FaceCmpFunc cmp_;
};

}

#endif  // SRC_EVENTTRIGGER_H_