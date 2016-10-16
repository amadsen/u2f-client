#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>

#include <nan.h>

/*
Useful for collecting numeric properties like Vendor ID, etc.
*/
extern v8::Local<v8::Uint32> getUInt32Property(IOHIDDeviceRef dev, CFStringRef key);

/*
Useful for collecting string properties like Manufacturer, etc.
*/
extern v8::Local<v8::String> getStringProperty(IOHIDDeviceRef dev, CFStringRef key);
