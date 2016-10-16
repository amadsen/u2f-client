#include "hid-support_mac.h"

v8::Local<v8::Uint32> getUInt32Property(IOHIDDeviceRef dev, CFStringRef key) {
	uint32_t value;

  assert( IOHIDDeviceGetTypeID() == CFGetTypeID(dev) );

	CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty(dev, key);
	if (tCFTypeRef && CFNumberGetTypeID() == CFGetTypeID(tCFTypeRef)) {
		// if this is a number, get it's value
		if( CFNumberGetValue( (CFNumberRef) tCFTypeRef, kCFNumberSInt32Type, &value ) ) {
      return Nan::New<v8::Uint32>(value);
    }
	}

	return Nan::New<v8::Uint32>(-1);
}

v8::Local<v8::String> getStringProperty(IOHIDDeviceRef dev, CFStringRef key) {
  assert( IOHIDDeviceGetTypeID() == CFGetTypeID(dev) );

  char        cstring[256] = ""; // name of manufacturer
  CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty( dev, key );
  if( tCFTypeRef && CFStringGetTypeID() == CFGetTypeID(tCFTypeRef) ) {
    CFStringRef tCFStringRef = (CFStringRef) tCFTypeRef;
  	if (tCFStringRef) {
  		(void) CFStringGetCString(tCFStringRef, cstring, sizeof(cstring), kCFStringEncodingUTF8);
  	}
  }

  // TODO: figure out if we need to free this string after converting
  // it to a v8::String
  return Nan::New<v8::String>(cstring).ToLocalChecked();

  // CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty( dev, key );
  // if( !( tCFTypeRef && CFStringGetTypeID() == CFGetTypeID(tCFTypeRef) ) ) {
  //   return NULL;
  // }
  //
  // CFStringRef str = (CFStringRef) tCFTypeRef;
  // CFIndex length = CFStringGetLength(str);
  // CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
  //
  // char * cstring = (char *)malloc(maxSize);
  // if (CFStringGetCString(str, cstring, maxSize, kCFStringEncodingUTF8)) {
  //   return cstring;
  // }
  //
  // return NULL;
}
