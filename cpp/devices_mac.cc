#include "devices.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>

NAN_METHOD(devices) {
  IOHIDManagerRef tIOHIDManagerRef = NULL;
	CFSetRef deviceCFSetRef = NULL;
  CFMutableDictionaryRef deviceMatchingDictionary = NULL;
	IOHIDDeviceRef *tIOHIDDeviceRefs = nil;

  UInt32 fidoUsagePage = 0xF1D0;
  UInt32 fidoUsage     = 0x0001;

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

      // create a new object to hold the device information
      v8::Local<v8::Object> deviceInfo = Nan::New<v8::Object>();
      // deviceInfo->Set(Nan::New<String>("vendorId").ToLocalChecked(), Nan::New<Integer>(dev->vendor_id));
      // deviceInfo->Set(Nan::New<String>("productId").ToLocalChecked(), Nan::New<Integer>(dev->product_id));
      // if (dev->path) {
      //   deviceInfo->Set(Nan::New<String>("path").ToLocalChecked(), Nan::New<String>(dev->path).ToLocalChecked());
      // }
      // if (dev->serial_number) {
      //   deviceInfo->Set(Nan::New<String>("serialNumber").ToLocalChecked(), Nan::New<String>(narrow(dev->serial_number).c_str()).ToLocalChecked());
      // }
      // if (dev->manufacturer_string) {
      //   deviceInfo->Set(Nan::New<String>("manufacturer").ToLocalChecked(), Nan::New<String>(narrow(dev->manufacturer_string).c_str()).ToLocalChecked());
      // }
      // if (dev->product_string) {
      //   deviceInfo->Set(Nan::New<String>("product").ToLocalChecked(), Nan::New<String>(narrow(dev->product_string).c_str()).ToLocalChecked());
      // }
      // deviceInfo->Set(Nan::New<String>("release").ToLocalChecked(), Nan::New<Integer>(dev->release_number));
      // deviceInfo->Set(Nan::New<String>("interface").ToLocalChecked(), Nan::New<Integer>(dev->interface_number));

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
	if (tIOHIDManagerRef) {
		CFRelease(tIOHIDManagerRef);
	}

	info.GetReturnValue().Set(retval);
}
