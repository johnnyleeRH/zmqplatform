#include "smartdatatrans.h"
#include "smarttransimpl.h"

namespace horizon {

SmartTrans& SmartTrans::GetSmartTransInst() {
  static SmartTransImpl impl;
  return impl;
}

}