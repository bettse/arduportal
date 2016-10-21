/*
Copyright (c) 2014-2015 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "RawHID.h"

static const uint8_t _hidReportDescriptorRawHID[] PROGMEM = {
  0x06, 0x00, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x19, 0x01, 0x29, 0x40, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x20, 0x81, 0x00, 0x19, 0x01, 0x29, 0x40, 0x91, 0x00, 0xC0
};

RawHID_::RawHID_(void) : PluggableUSBModule(2, 1, epType), protocol(HID_REPORT_PROTOCOL), idle(1), dataLength(0), dataAvailable(0), featureReport(NULL), featureLength(0) {
  epType[0] = EP_TYPE_INTERRUPT_IN;
  epType[1] = EP_TYPE_INTERRUPT_OUT;
  PluggableUSB().plug(this);
}

int RawHID_::getInterface(uint8_t *interfaceCount) {
  // Maybe as optional device FastRawHID with different USAGE PAGE
  *interfaceCount += 1; // uses 1
  HIDDescriptor hidInterface = {
      D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
      D_HIDREPORT(sizeof(_hidReportDescriptorRawHID)),
      D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, RAWHID_SIZE, 0x01)};
  return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int RawHID_::getDescriptor(USBSetup &setup) {
  // Check if this is a HID Class Descriptor request
  if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) {
    return 0;
  }
  if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) {
    return 0;
  }

  // In a HID Class Descriptor wIndex cointains the interface number
  if (setup.wIndex != pluggedInterface) {
    return 0;
  }

  // Reset the protocol on reenumeration. Normally the host should not assume the state of the protocol
  // due to the USB specs, but Windows and Linux just assumes its in report mode.
  protocol = HID_REPORT_PROTOCOL;

  return USB_SendControl(TRANSFER_PGM, _hidReportDescriptorRawHID, sizeof(_hidReportDescriptorRawHID));
}

bool RawHID_::setup(USBSetup &setup) {
  if (pluggedInterface != setup.wIndex) {
    return false;
  }

  uint8_t request = setup.bRequest;
  uint8_t requestType = setup.bmRequestType;

  if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE) {
    if (request == HID_GET_REPORT) {
      // TODO: HID_GetReport();
      return true;
    }
    if (request == HID_GET_PROTOCOL) {
      // TODO: Send8(protocol);
      return true;
    }
  }

  if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
    if (request == HID_SET_PROTOCOL) {
      protocol = setup.wValueL;
      return true;
    }
    if (request == HID_SET_IDLE) {
      idle = setup.wValueL;
      return true;
    }
    if (request == HID_SET_REPORT) {
      // Check if data has the correct length afterwards
      int length = setup.wLength;

      // Feature (set feature report)
      if (setup.wValueH == HID_REPORT_TYPE_FEATURE) {
        // No need to check for negative featureLength values,
        // except the host tries to send more then 32k bytes.
        // We dont have that much ram anyways.
        if (length == featureLength) {
          USB_RecvControl(featureReport, featureLength);

          // Block until data is read (make length negative)
          disableFeatureReport();
          return true;
        }
      }

      // Output (set out report)
      else if (setup.wValueH == HID_REPORT_TYPE_OUTPUT) {
        if (!dataAvailable && length <= dataLength) {
          // Write data to fit to the end (not the beginning) of the array
          USB_RecvControl(data + dataLength - length, length);
          dataAvailable = length;
          return true;
        }
      }

      // Input (set HID report)
      else if(setup.wValueH == HID_REPORT_TYPE_INPUT) {
        if(length == sizeof(_keyReport)){
          USB_RecvControl(&_keyReport, length);
          return true;
        }
      }
    }
  }

  return false;
}

RawHID_ RawHID;
