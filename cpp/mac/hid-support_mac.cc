#include "hid-support_mac.h"

/*
Useful for collecting numeric properties like Vendor ID, etc.
*/
v8::Local<v8::Uint32> getUInt32Property(IOHIDDeviceRef dev, CFStringRef key) {
	Nan::EscapableHandleScope scope;
	uint32_t value;

	assert( IOHIDDeviceGetTypeID() == CFGetTypeID(dev) );

	CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty(dev, key);
	if (tCFTypeRef && CFNumberGetTypeID() == CFGetTypeID(tCFTypeRef)) {
		// if this is a number, get it's value
		if( CFNumberGetValue( (CFNumberRef) tCFTypeRef, kCFNumberSInt32Type, &value ) ) {
			return scope.Escape(Nan::New<v8::Uint32>(value));
		}
	}

	return scope.Escape(Nan::New<v8::Uint32>(-1));
}

/*
Useful for collecting string properties like Manufacturer, etc.
*/
v8::Local<v8::String> getStringProperty(IOHIDDeviceRef dev, CFStringRef key) {
	Nan::EscapableHandleScope scope;
	assert( IOHIDDeviceGetTypeID() == CFGetTypeID(dev) );

	char        cstring[256] = ""; // name of manufacturer
	CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty( dev, key );
	if( tCFTypeRef && CFStringGetTypeID() == CFGetTypeID(tCFTypeRef) ) {
		CFStringRef tCFStringRef = (CFStringRef) tCFTypeRef;
		if (tCFStringRef) {
			(void) CFStringGetCString(tCFStringRef, cstring, sizeof(cstring), kCFStringEncodingUTF8);
		}
	}

	return scope.Escape(
		Nan::New<v8::String>(cstring).ToLocalChecked()
	);
}

/*
Private helper function to convert uint64_t to a string
 */
extern std::string uint64ToString( uint64_t value ) {
    std::ostringstream os;
    os << value;
    return os.str();
}

/*
Private helper function to convert a string to a uint64_t
 */
extern uint64_t stringToUint64( v8::String numeric ) {
	uint64_t value;
	Nan::Utf8String utf8Str(numeric);
    std::istringstream numericInputStream(*utf8Str);
	numericInputStream >> value;

    return value;
}

/*
Get a unique service/registry id (path) to the device so we can open it later.
*/
extern v8::Local<v8::String> getRegistryEntryId(IOHIDDeviceRef dev) {
	Nan::EscapableHandleScope scope;

	io_service_t ioDevice = IOHIDDeviceGetService(dev);
	if(ioDevice && ioDevice != MACH_PORT_NULL) {
		uint64_t entry_id;
		IOReturn result = IORegistryEntryGetRegistryEntryID(ioDevice, &entry_id);
		if (result == kIOReturnSuccess) {
			return scope.Escape(
				Nan::New<v8::String>(uint64ToString(entry_id)).ToLocalChecked()
			);
		}
	}

	return scope.Escape(Nan::New<v8::String>("").ToLocalChecked());
};

/*
Open an IOHIDDeviceRef for a given "path" (a.k.a. RegistryEntryId)
*/
extern IOHIDDeviceRef openDeviceAtPath(v8::String path) {
	uint64_t registryId = stringToUint64(path);
	CFDictionaryRef deviceMatchingDictionary(IORegistryEntryIDMatching(device_id));
	io_service_t deviceService;
    IOHIDDeviceRef dev = nil;

	do {
		if(!deviceMatchingDictionary){
			break;
		}

		deviceService = IOServiceGetMatchingService(kIOMasterPortDefault, deviceMatchingDictionary);
		if(!deviceService){
			break;
		}

		dev = IOHIDDeviceCreate(kCFAllocatorDefault, deviceService);
    } while(false);

	if (deviceMatchingDictionary) {
		CFRelease(deviceMatchingDictionary);
	}

	return dev;
}
