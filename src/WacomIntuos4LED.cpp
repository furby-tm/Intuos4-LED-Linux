
/*
 * WacomIntuos4LED.cpp
 *
 *  Created on: 11.07.2010
 *      Author: Christoph Karg
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <Magick++.h>
#include "WacomIntuos4LED.h"

using namespace std;
using namespace Magick;

#define WAC_VENDOR_ID 0x056a
#define WAC_INTUOS4_M_PRODUCT_ID 0x00b9
#define WAC_INTUOS4_L_PRODUCT_ID 0x00ba
#define WAC_USB_TIMEOUT 100
#define WAC_USB_RETRIES 10
#define ICON_WIDTH 64
#define ICON_HEIGHT 16

/*
 * Links to the icon library data
 */
extern const unsigned char icon_alt[1024];
extern const unsigned char icon_arrow_up[1024];
extern const unsigned char icon_blank[1024];
extern const unsigned char icon_button_1[1024];
extern const unsigned char icon_button_2[1024];
extern const unsigned char icon_button_3[1024];
extern const unsigned char icon_button_4[1024];
extern const unsigned char icon_button_5[1024];
extern const unsigned char icon_button_6[1024];
extern const unsigned char icon_button_7[1024];
extern const unsigned char icon_button_8[1024];
extern const unsigned char icon_ctrl[1024];
extern const unsigned char icon_debian_logo[1024];
extern const unsigned char icon_number_1[1024];
extern const unsigned char icon_number_2[1024];
extern const unsigned char icon_number_3[1024];
extern const unsigned char icon_number_4[1024];
extern const unsigned char icon_number_5[1024];
extern const unsigned char icon_number_6[1024];
extern const unsigned char icon_number_7[1024];
extern const unsigned char icon_number_8[1024];
extern const unsigned char icon_redo[1024];
extern const unsigned char icon_save[1024];
extern const unsigned char icon_shift[1024];
extern const unsigned char icon_space[1024];
extern const unsigned char icon_tux[1024];
extern const unsigned char icon_undo[1024];
extern const unsigned char icon_wacom_logo[1024];
extern const unsigned char icon_zoom_in[1024];
extern const unsigned char icon_zoom_out[1024];


WacomIntuos4LED::WacomIntuos4LED() {
	initIconLibrary();
}

void WacomIntuos4LED::initIconLibrary() {
	context=NULL;
	dev_handle=NULL;

	icon_table["Alt"] = icon_alt;
	icon_table["ArrowUp"] = icon_arrow_up;
	icon_table["Blank"] = icon_blank;
	icon_table["ButtonOne"] = icon_button_1;
	icon_table["ButtonTwo"] = icon_button_2;
	icon_table["ButtonThree"] = icon_button_3;
	icon_table["ButtonFour"] = icon_button_4;
	icon_table["ButtonFive"] = icon_button_5;
	icon_table["ButtonSix"] = icon_button_6;
	icon_table["ButtonSeven"] = icon_button_7;
	icon_table["ButtonEight"] = icon_button_8;
	icon_table["Ctrl"] = icon_ctrl;
	icon_table["DebianLogo"] = icon_debian_logo;
	icon_table["NumberOne"] = icon_number_1;
	icon_table["NumberTwo"] = icon_number_2;
	icon_table["NumberThree"] = icon_number_3;
	icon_table["NumberFour"] = icon_number_4;
	icon_table["NumberFive"] = icon_number_5;
	icon_table["NumberSix"] = icon_number_6;
	icon_table["NumberSeven"] = icon_number_7;
	icon_table["NumberEight"] = icon_number_8;
	icon_table["Redo"] = icon_redo;
	icon_table["Save"] = icon_save;
	icon_table["Shift"] = icon_shift;
	icon_table["Space"] = icon_space;
	icon_table["Tux"] = icon_tux;
	icon_table["Undo"] = icon_undo;
	icon_table["WacomLogo"] = icon_wacom_logo;
	icon_table["ZoomIn"] = icon_zoom_in;
	icon_table["ZoomOut"] = icon_zoom_out;
}

WacomIntuos4LED::~WacomIntuos4LED() {
	if (context != NULL) {
		libusb_exit(context);
	}
}

unsigned char WacomIntuos4LED::transformShade(double shade) const {
	unsigned char c=(unsigned char)(shade*16.0);
	if (c>=0x10) c=0x0f;
	return c;
}

int WacomIntuos4LED::toIndex(int x, int y) const {
	return (x+ICON_WIDTH*y);
}

void WacomIntuos4LED::flipIconVertically(unsigned char *icon) const {
	unsigned char c;

	for (int y = 0; y < ICON_HEIGHT; y++) {
		for (int x = 0; x < ICON_WIDTH/2; x++) {
			c = icon[toIndex(x,y)];
			icon[toIndex(x,y)] = icon[toIndex(ICON_WIDTH -1 - x ,y)];
			icon[toIndex(ICON_WIDTH -1 - x ,y)] = c;
		}
	}
}

void WacomIntuos4LED::flipIconHorizontally(unsigned char *icon) const {
	unsigned char c;

	for (int x = 0; x < ICON_WIDTH; x++) {
		for (int y = 0; y < ICON_HEIGHT/2; y++) {
			c = icon[toIndex(x,y)];
			icon[toIndex(x,y)] = icon[toIndex(x,ICON_HEIGHT-y-1)];
			icon[toIndex(x,ICON_HEIGHT-y-1)] = c;
		}
	}

	for (int i=0; i<ICON_HEIGHT*ICON_WIDTH; i++) {
		c = (icon[i]>>4) + (icon[i]<<4);
		icon[i] = c;
	}


}


int WacomIntuos4LED::sendControlMsg(uint8_t bm_request_type, uint8_t b_request,
		uint16_t w_value, uint16_t w_index, unsigned char *data,
		uint16_t w_length, unsigned int timeout, unsigned int retries) {
	int rv = 0;
	int cntr = 0;

	do {
		rv = libusb_control_transfer(dev_handle, bm_request_type, // bmRequestType
				b_request, // bRequest
				w_value, // wValue
				w_index, // wIndex
				data, // data
				w_length, // wLength
				timeout // timeout
				);
		cntr++;
	} while (rv < 0 && cntr < retries);

	return rv;
}

bool WacomIntuos4LED::deviceEnabled() const {
	return (dev_handle != NULL);
}

bool WacomIntuos4LED::deviceDisabled() const {
	return (dev_handle == NULL);
}

bool WacomIntuos4LED::displayPalette(bool right_handed) {
	unsigned char icon[8][1024];
	unsigned char c;

	for (int b=0; b<8; b++) {
		c = b;
		c = (c << 4) + b+1;
		for (int i=0; i<512; i++) {
			icon[b][i]=(2*b << 4) + 2*b;
			icon[b][512+i] = (2*b+1 << 4) + 2*b+1;
		}
		setIcon(b, icon[b], right_handed);
	}

}

bool WacomIntuos4LED::setTransferMode(bool mode) {
	int rv = 0;
	unsigned char buf[2];

	if (deviceDisabled()) {
		return false;
	}

	buf[0] = 0x21;
	buf[1] = mode;

	rv = sendControlMsg(0x21, // bmRequestType
			0x09, // bRequest
			0x0321, // wValue
			0, // wIndex
			buf, // data
			2, // wLength
			WAC_USB_TIMEOUT, // timeout
			WAC_USB_RETRIES);

	return (rv >= 0);
}

bool WacomIntuos4LED::setLibraryIcon(unsigned int button, string name, bool right_handed) {
	if (icon_table.count(name)==0) {
		return false;
	} else {
		return setIcon(button, icon_table[name], right_handed);
	}
}


bool WacomIntuos4LED::setImage(unsigned int button, string file, bool right_handed) {
	unsigned char data[1024];

	if (imageToIcon(file, data)) {
		return setIcon(button, data, right_handed);
	} else {
		return false;
	}
}


bool WacomIntuos4LED::dumpImageAsStaticData(string file, string name) {
	unsigned char data[1024];

	if (imageToIcon(file, data)) {
		dumpIconAsStaticData(data, name);
		return true;
	} else {
		return false;
	}
}


bool WacomIntuos4LED::setIcon(unsigned int button, const unsigned char *icon, bool right_handed) {
	unsigned char buf[259];
	int c = 0;
	int rv = 0;
	unsigned char data[1024];

	if (button>=8) {
		return false;
	}

	if (!setTransferMode(true)) {
		return false;
	}

	memcpy(data, icon, 1024);

	if (!right_handed) {
		button = 7-button;
		flipIconHorizontally(data);
		flipIconVertically(data);
	}

	buf[0] = 0x23;
	buf[1] = button;
	for (int i = 0; i < 4; i++) {
		buf[2] = i;
		for (int j = 0; j < 256; j++) {
			buf[3 + j] = data[256 * i + j];
		}

		rv = sendControlMsg(0x21, // bmRequestType
				0x09, // bRequest
				0x0323, // wValue
				0, // wIndex
				buf, // data
				259, // wLength
				WAC_USB_TIMEOUT,// timeout
				WAC_USB_RETRIES);

		if (rv < 0) {
			return false;
		}
	}

	if (!setTransferMode(false)) {
		return false;
	}

	return true;
}

bool WacomIntuos4LED::imageToIcon(const string& file, unsigned char* icon) const {
	InitializeMagick("/usr/bin/");
	Image image(file.c_str());
	ColorGray color;
	unsigned char l, h;
	int i=0;

	image.type(GrayscaleType);
	int rows = image.rows();
	int cols = image.columns();

	if (rows!=32 || cols!=64) {
		return false;
	}

	for (int y=0; y<rows; y+=2) {
		for (int x=0; x<cols; x++) {
			color = image.pixelColor(x,y);
			l = transformShade(color.shade());
			color = image.pixelColor(x,y+1);
			h = transformShade(color.shade());
			icon[i]= (h << 4) + l;
			i++;
		}
	}

	flipIconVertically(icon);

	return true;
}

void WacomIntuos4LED::releaseInterface() {
	libusb_release_interface(dev_handle, 0);
	libusb_attach_kernel_driver (dev_handle, 0);
} 

bool WacomIntuos4LED::clearButton(unsigned int button, bool right_handed) {
	unsigned char icon[1024];

	for (int i=0; i<1024; i++) {
		icon[i]=0x00;
	}

	return setIcon(button, icon, right_handed);
}

bool WacomIntuos4LED::clearAllButtons() {

	for (int b=0; b<8; b++) {
		clearButton(b);
	}

	return true;
}

bool WacomIntuos4LED::init(uint16_t vendor_id, uint16_t product_id) {
	int rv;

	/*
	 * Initialize the USB library.
	 */
	rv = libusb_init(&context);
	if (rv < 0) {
		cerr << "ERROR: USB library init failed." << endl;
		return false;
	}

	dev_handle = libusb_open_device_with_vid_pid(context, vendor_id, product_id);

	if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
		cout << "Active kernel driver found." << endl;
		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
			cout << "Kernel Driver Detached!" << endl;
		}
	}

	rv = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had just 1)
	if (rv < 0) {
		cout << "ERROR: Cannot Claim Interface" << endl;
		return false;
	}
	return true;
}

bool WacomIntuos4LED::autoDetectTablet() {
	// Pointer to the device list
	libusb_device **dev_list = NULL;
	// Number of attached devices;
	int nr_of_devices = 0;
	// Result of auto detection.
	bool found = false;
	// descriptor containing the device information
	libusb_device_descriptor dev_descriptor;
	// return value
	int rv=0;
	// helper to run through the device array;
	ssize_t i=0;
	// helper to store the vendor id of the device
	uint16_t vendor_id;
	// helper to store the product id of the device
	uint16_t product_id;

	/*
	 * Initialize the USB library.
	 */
	rv = libusb_init(&context);
	if (rv < 0) {
		cerr << "ERROR: USB library init failed." << endl;
		return false;
	}

	/*
	 * Retrieving device list information.
	 */
	nr_of_devices = libusb_get_device_list(context, &dev_list);

	/*
	 * Error check!
	 */
	if (nr_of_devices<0) {
		return false;
	}

	/*
	 * Iterate through the device list.
	 */
	while (i < nr_of_devices && !found) {
		/*
		 * Retrieving description of the device
		 */
		rv = libusb_get_device_descriptor(dev_list[i], &dev_descriptor);
		if (rv<0) {
			cerr << "Error during retrieval of device information." << endl;
		} else {
			/*
			 *  Get vendor and product information.
			 */
			vendor_id = dev_descriptor.idVendor;
			product_id = dev_descriptor.idProduct;

			/*
			 * Check for an Intuos 4 tablet.
			 */
			if (vendor_id==WAC_VENDOR_ID) {
                          if ((product_id==WAC_INTUOS4_M_PRODUCT_ID) || 
                              (product_id==WAC_INTUOS4_L_PRODUCT_ID)) {
					found=true;
				}
			}

		}
		i++;
	}

	/*
	 * Release device list memory.
	 */
	libusb_free_device_list(dev_list, 1);

	/*
	 * If an Intuos 4 tablet was found, try to initialize it.
	 */
	if (found) {
		found = init(vendor_id, product_id);
	}

	/*
	 * Return the result of the auto detection.
	 */
	return found;
}

void WacomIntuos4LED::dumpIconNames() const {
	// Iterator to access the icon library
	map<string,const unsigned char*>::const_iterator map_it;

	for (map_it=icon_table.begin(); map_it!=icon_table.end(); map_it++) {
		cout << map_it->first << endl;
	}
}

void WacomIntuos4LED::dumpIconAsStaticData(unsigned char* icon, string name) const {
	int steps=16;

	cout << "unsigned char " << name << "[1024] = {" << endl << "\t";

	for (int i=0; i<1024; i++) {
		cout << "0x" << hex << setw(2) << setfill('0') << (int)icon[i];
		if (i<1023) {
			cout << ", ";
		}
		if ((i+1)%steps==0 && i<1023) {
			cout << endl << "\t";
		}
	}
	cout << endl << "};" << endl;
}

