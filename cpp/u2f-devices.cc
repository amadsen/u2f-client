#include "devices.h"

using v8::FunctionTemplate;

// u2f-devices.cc represents the top level of the module.
// C++ constructs that are exposed to javascript are exported here

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("devices").ToLocalChecked(),
    Nan::GetFunction(Nan::New<FunctionTemplate>(devices)).ToLocalChecked());
}

NODE_MODULE(U2fDevices, InitAll)
