#include <iostream>
#include <unistd.h>
#include "smartrecv.h"
using namespace std;

int main() {
  SmartRecv smart;
  smart.Init();
  while (true) {
    sleep(1);
  }
  smart.Exit();
}