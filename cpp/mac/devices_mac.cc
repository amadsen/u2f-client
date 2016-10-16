#include "../devices.h"

#include "hid-support_mac.h"

const UInt32 fidoUsagePage = 0xF1D0;
const UInt32 fidoUsage     = 0x0001;

NAN_METHOD(supportsU2f) {
  IOHIDManagerRef tIOHIDManagerRef = NULL;
  CFSetRef deviceCFSetRef = NULL;
  CFMutableDictionaryRef deviceMatchingDictionary = NULL;
  IOHIDDeviceRef *tIOHIDDeviceRefs = nil;
  CFIndex deviceCount = 0;

  do {
		tIOHIDManagerRef = IOHIDManagerCreate(kCFAllocatorDefault,
		                                      kIOHIDOptionsTypeNone);
		if (!tIOHIDManagerRef) {
			break;
		}

    // create a dictionary to add usage page/usages to
    deviceMatchingDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
    if(!deviceMatchingDictionary){
        break;
    }

    // Set Vendor ID
    uint32_t vendorId = Nan::To<uint32_t>(info[0]).FromJust();
    CFNumberRef vendorCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &vendorId );
    if ( !vendorCFNumberRef ) {
        break;
    }
    CFDictionarySetValue( deviceMatchingDictionary,
                         CFSTR( kIOHIDVendorIDKey ), vendorCFNumberRef );
    CFRelease( vendorCFNumberRef );

    // Set Product ID
    uint32_t productId = Nan::To<uint32_t>(info[1]).FromJust();
    CFNumberRef productCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &productId );
    if ( !productCFNumberRef ) {
        break;
    }
    CFDictionarySetValue( deviceMatchingDictionary,
                         CFSTR( kIOHIDProductIDKey ), productCFNumberRef );
    CFRelease( productCFNumberRef );

    // Set FIDO Usage Page
    CFNumberRef pageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &fidoUsagePage );
    if ( !pageCFNumberRef ) {
        break;
    }
    CFDictionarySetValue( deviceMatchingDictionary,
                         CFSTR( kIOHIDDeviceUsagePageKey ), pageCFNumberRef );
    CFRelease( pageCFNumberRef );

    // Set FIDO primary usage
    CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &fidoUsage );
    if ( !usageCFNumberRef ) {
        break;
    }
    CFDictionarySetValue( deviceMatchingDictionary,
                         CFSTR( kIOHIDDeviceUsageKey ), usageCFNumberRef );
    CFRelease( usageCFNumberRef );

		IOHIDManagerSetDeviceMatching(tIOHIDManagerRef,
                                      deviceMatchingDictionary);

		IOReturn tIOReturn = IOHIDManagerOpen(tIOHIDManagerRef,
		                                      kIOHIDOptionsTypeNone);
		if (noErr != tIOReturn) {
			break;
		}

		deviceCFSetRef = IOHIDManagerCopyDevices(tIOHIDManagerRef);
		if (!deviceCFSetRef) {
			break;
		}

		deviceCount = CFSetGetCount(deviceCFSetRef);

	} while (false);

	if (tIOHIDDeviceRefs) {
		free(tIOHIDDeviceRefs);
	}
	if (deviceCFSetRef) {
		CFRelease(deviceCFSetRef);
		deviceCFSetRef = NULL;
	}
  if (deviceMatchingDictionary) {
    CFRelease(deviceMatchingDictionary);
  }
	if (tIOHIDManagerRef) {
		CFRelease(tIOHIDManagerRef);
	}

  // return a boolean indicating whether we found a device with the vendorId and
  // productId that also supports the U2F usagePage and usage. If deviceCount is
  // greater than 0, we found it.
  info.GetReturnValue().Set((deviceCount > 0));

}

NAN_METHOD(devices) {
  IOHIDManagerRef tIOHIDManagerRef = NULL;
	CFSetRef deviceCFSetRef = NULL;
  CFMutableDictionaryRef deviceMatchingDictionary = NULL;
	IOHIDDeviceRef *tIOHIDDeviceRefs = nil;

  // create an array to return
  v8::Local<v8::Array> retval = Nan::New<v8::Array>();

  do {
		tIOHIDManagerRef = IOHIDManagerCreate(kCFAllocatorDefault,
		                                      kIOHIDOptionsTypeNone);
		if (!tIOHIDManagerRef) {
			break;
		}

    // create a dictionary to add usage page/usages to
    deviceMatchingDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
    if(!deviceMatchingDictionary){
        break;
    }

    // Set FIDO Usage Page
    CFNumberRef pageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &fidoUsagePage );
    if ( !pageCFNumberRef ) {
        break;
    }
    CFDictionarySetValue( deviceMatchingDictionary,
                         CFSTR( kIOHIDDeviceUsagePageKey ), pageCFNumberRef );
    CFRelease( pageCFNumberRef );

    // Set FIDO primary usage
    CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &fidoUsage );
    if ( !usageCFNumberRef ) {
        break;
    }
    CFDictionarySetValue( deviceMatchingDictionary,
                         CFSTR( kIOHIDDeviceUsageKey ), usageCFNumberRef );
    CFRelease( usageCFNumberRef );

		IOHIDManagerSetDeviceMatching(tIOHIDManagerRef,
                                      deviceMatchingDictionary);

		IOReturn tIOReturn = IOHIDManagerOpen(tIOHIDManagerRef,
		                                      kIOHIDOptionsTypeNone);
		if (noErr != tIOReturn) {
			break;
		}

		deviceCFSetRef = IOHIDManagerCopyDevices(tIOHIDManagerRef);
		if (!deviceCFSetRef) {
			break;
		}

		CFIndex deviceIndex,
		        deviceCount = CFSetGetCount(deviceCFSetRef);

		tIOHIDDeviceRefs = (IOHIDDeviceRef *) malloc(sizeof(IOHIDDeviceRef) * deviceCount);
		if (!tIOHIDDeviceRefs) {
			break;
		}

		CFSetGetValues(deviceCFSetRef, (const void **)tIOHIDDeviceRefs);
		CFRelease(deviceCFSetRef);
		deviceCFSetRef = NULL;

    int count = 0;
		for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
			if (!tIOHIDDeviceRefs[deviceIndex]) {
				continue;
			}

      IOHIDDeviceRef dev = tIOHIDDeviceRefs[deviceIndex];
      assert( IOHIDDeviceGetTypeID() == CFGetTypeID(dev) );

      // create a new object to hold the device information
      v8::Local<v8::Object> deviceInfo = Nan::New<v8::Object>();
      deviceInfo->Set(
        Nan::New<v8::String>("vendorId").ToLocalChecked(),
        getUInt32Property( dev, CFSTR(kIOHIDVendorIDKey) )
      );
      deviceInfo->Set(
        Nan::New<v8::String>("productId").ToLocalChecked(),
        getUInt32Property( dev, CFSTR(kIOHIDProductIDKey) )
      );
      deviceInfo->Set(
        Nan::New<v8::String>("path").ToLocalChecked(),
        Nan::New<v8::String>("").ToLocalChecked()
      );
      deviceInfo->Set(
        Nan::New<v8::String>("serialNumber").ToLocalChecked(),
        getStringProperty( dev, CFSTR(kIOHIDSerialNumberKey) )
      );
      deviceInfo->Set(
        Nan::New<v8::String>("manufacturer").ToLocalChecked(),
        getStringProperty( dev, CFSTR(kIOHIDManufacturerKey) )
      );
      deviceInfo->Set(
        Nan::New<v8::String>("product").ToLocalChecked(),
        getStringProperty( dev, CFSTR(kIOHIDProductKey) )
      );
      deviceInfo->Set(
        Nan::New<v8::String>("release").ToLocalChecked(),
        getUInt32Property( dev, CFSTR(kIOHIDVersionNumberKey) )
      );
      deviceInfo->Set(
        Nan::New<v8::String>("usagePage").ToLocalChecked(),
        Nan::New<v8::Int32>(fidoUsagePage) // just set it to the one we searched for
      );
      deviceInfo->Set(
        Nan::New<v8::String>("usage").ToLocalChecked(),
        Nan::New<v8::Int32>(fidoUsage) // just set it to the one we searched for
      );
      deviceInfo->Set(
        Nan::New<v8::String>("interface").ToLocalChecked(),
        Nan::New<v8::Int32>(-1) // not meaningful on Mac
      );

      // add deviceInfo object to the array
      retval->Set(count++, deviceInfo);
		}
	} while (false);

	if (tIOHIDDeviceRefs) {
		free(tIOHIDDeviceRefs);
	}
	if (deviceCFSetRef) {
		CFRelease(deviceCFSetRef);
		deviceCFSetRef = NULL;
	}
  if (deviceMatchingDictionary) {
    CFRelease(deviceMatchingDictionary);
  }
	if (tIOHIDManagerRef) {
		CFRelease(tIOHIDManagerRef);
	}

	info.GetReturnValue().Set(retval);
}
