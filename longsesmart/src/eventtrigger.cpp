#include "eventtrigger.h"
#include <unistd.h>

namespace horizon {

EventTrigger::EventTrigger()
    : pack_(SmartPack::Inst()), start_(false), detect_(NULL), cmp_(NULL) {}

bool EventTrigger::Start() {
  start_ = true;
  int ret = pthread_create(&td_, NULL, TriggerThread, this);
  if (0 != ret) {
    start_ = false;
    return false;
  }
  start_ = true;
  return true;
}

void EventTrigger::Stop() {
  start_ = false;
  pthread_join(td_, NULL);
}

void EventTrigger::RegisteDetectHandle(FaceDetectFunc detect) {
  detect_ = detect;
}

void EventTrigger::RegisteCmpHandle(FaceCmpFunc cmp) { cmp_ = cmp; }

void* EventTrigger::TriggerThread(void* param) {
  EventTrigger* impl = reinterpret_cast<EventTrigger*>(param);
  while (impl->start_) {
    const char* data;
    bool datahandled = false;
    if (impl->pack_.GetSmartData(data)) {
      printf("lrh lib get smart data.\n");
      if (TYPE_DETECT == impl->pack_.GetSmartType() && impl->detect_) {
        printf("lrh detect callback\n");
        impl->detect_(data);
        datahandled = true;
      }
      if (TYPE_CMP == impl->pack_.GetSmartType() && impl->cmp_) {
        printf("lrh cmp callback\n");
        impl->cmp_(data);
        datahandled = true;
      }
      impl->pack_.ReadFinish();
      printf("lrh lib get smart data end.\n");
    }
    if (!datahandled) usleep(1);
  }
}

}  // namespace horizon