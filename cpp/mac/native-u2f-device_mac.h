class NativeU2fDevice : public Nan::ObjectWrap {
    IOHIDDeviceRef dev;

    static NAN_METHOD(read);
    static NAN_METHOD(write);
    static NAN_METHOD(close);
}
