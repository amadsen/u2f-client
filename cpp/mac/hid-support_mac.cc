#include "hid-support_mac.h"

/*
Useful for collecting numeric properties like Vendor ID, etc.
*/
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

/*
Useful for collecting string properties like Manufacturer, etc.
*/
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
}

/*
Private helper function to convert uint64_t to a string
 */
std::string uint64ToString( uint64_t value ) {
    std::ostringstream os;
    os << value;
    return os.str();
}

/*
Get a unique service/registry id (path) to the device so we can open it later.
*/
extern v8::Local<v8::String> getRegistryEntryId(IOHIDDeviceRef dev) {
	io_service_t ioDevice = IOHIDDeviceGetService(dev);
	if(ioDevice && ioDevice != MACH_PORT_NULL){
		uint64_t entry_id;
	  IOReturn result = IORegistryEntryGetRegistryEntryID(ioDevice, &entry_id);
	  if (result == kIOReturnSuccess) {
	  	return Nan::New<v8::String>(uint64ToString(entry_id)).ToLocalChecked();
	  }
	}

	return Nan::New<v8::String>("").ToLocalChecked();
};
