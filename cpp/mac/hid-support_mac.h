#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDDevice.h>

#include <nan.h>
#include <sstream>

/*
Useful for collecting numeric properties like Vendor ID, etc.
*/
extern v8::Local<v8::Uint32> getUInt32Property(IOHIDDeviceRef dev, CFStringRef key);

/*
Useful for collecting string properties like Manufacturer, etc.
*/
extern v8::Local<v8::String> getStringProperty(IOHIDDeviceRef dev, CFStringRef key);

/*
Get a unique service/registry id (path) to the device so we can open it later.
*/
extern v8::Local<v8::String> getRegistryEntryId(IOHIDDeviceRef dev);
