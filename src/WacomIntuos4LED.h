/*
 * WacomIntuos4LED.h
 *
 *  Created on: 11.07.2010
 *      Author: Christoph Karg
 */

#ifndef WACOMINTUOS4LED_H_
#define WACOMINTUOS4LED_H_

#include <string>
#include <map>
#include <libusb-1.0/libusb.h>

using namespace std;

/**
 * @class WacomIntuos4LED
 *
 * This class provides methods to access the LED button display of
 * the Wacom Intuos4 Tablet.
 */
class WacomIntuos4LED {
private:

	/**
	 *  USB context.
	 *
	 *  USB context to use for communication.
	 */
	libusb_context *context;

	/**
	 * Device handle for tablet access.
	 *
	 * This handle is used to access the device. If no device was detected and initialized, this pointer
	 * equals NULL.
	 */
	libusb_device_handle *dev_handle;

	/**
	 * Table with icons.
	 *
	 * This table contains (pointers to) icons to be used with the intuos 4 led displays. For
	 * simplicity, the icons can be accessed by a unique name.
	 */
	map<string,const unsigned char*> icon_table;

	/**
	 * Init the icon library.
	 *
	 * This method sets up the icon library, this is, stores all available icons with their respective
	 * names in the icon_table map.
	 */
	void initIconLibrary();

	/**
	 * Send a control message to the USB device.
	 *
	 * This function is a wrapper to the underlying function of the USB library.
	 *
	 * @param bm_request_type 	the request type field for the setup packet
	 * @param b_request 	the request field for the setup packet
	 * @param w_value 	the value field for the setup packet
	 * @param w_index 	the index field for the setup packet
	 * @param data 	data buffer to be transmitted
	 * @param w_length 	length field for the setup packet. The data buffer should be at
	 * 	least this size.
	 * @param timeout 	timeout (in millseconds) that this function should wait before
	 * 	giving up due to no response being received. For an unlimited timeout, use value 0.
	 * @param retries number of retries in case of a transmission error.
	 *
	 * @return error code (<0) in case of a transmission error, number of transferred bytes,
	 * 	otherwise.
	 */
	int sendControlMsg(uint8_t bm_request_type, uint8_t b_request,
			uint16_t w_value, uint16_t w_index, unsigned char *data,
			uint16_t w_length, unsigned int timeout, unsigned int retries);

	/**
	 * Check whether the device is enabled.
	 *
	 * This method can be used to check whether the device is enabled, i.e., dev_handle is non-null.
	 *
	 * @returns true, if a device was detected and is usable
	 */
	bool deviceEnabled() const;

	/**
	 * Check whether the device is disabled.
	 *
	 * This method can be used to check whether the device is disabled, i.e., dev_handle equals NULL.
	 *
	 * @returns true, if no device was detected or an initialization error occured
	 */
	bool deviceDisabled() const;

	/**
	 * Flip an icon vertically.
	 *
	 * @param icon pointer to a 1024 byte array including the icon data
	 */
	void flipIconVertically(unsigned char *icon) const;

	/**
	 * Flip an icon horizontally.
	 *
	 * @param icon pointer to a 1024 byte array including the icon data
	 */
	void flipIconHorizontally(unsigned char *icon) const;

	/**
	 * Map a 2-dimensional coordinate to an array index.
	 *
	 * This method is used internally to to flip the icons vertically or horizontally.
	 *
	 * @param x x coordinate
	 * @param y y coordinate
	 *
	 * @return array index
	 */
	int toIndex(int x, int y) const;

	/**
	 * Transform a grayscale value to a 4-bit graytone.
	 *
	 * @param grayscale as a floating point value
	 *
	 * @return approximation of the grayscale as a 4-bit value
	 */
	unsigned char transformShade(double shade) const;

	/**
	 * Set the transfer mode of the tablet.
	 *
	 * This method is used to activate or deactivate the icon transfer mode of the tablet.
	 *
	 * @param mode true = enable transfer mode, false = disable transfer mode
	 *
	 * @return true, if operation was successful, false, otherwise.
	 */
	bool setTransferMode(bool mode);

	/**
	 * Transfer an icon to the tablet.
	 *
	 * Ths method transfers an icon to the tablet.
	 *
	 * @param button number of the button to which the icon is assigned (must be between
	 * 0 and 7).
	 * @param icon pointer to a 1024 byte block which contains the bitmap.
	 * @param right_handed orientation of the tablet.
	 *
	 * @return true, if transmission was successfull, false, otherwise.
	 */
	bool setIcon(unsigned int button, const unsigned char *icon, bool right_handed=true);

	/**
	 * Convert an image to an icon.
	 *
	 * @param file  name of the image file.
	 * @param icon pointer to an 1024 byte block to store the converted image
	 *
	 * @return true, if transmission was successful, false, otherwise.
	 */
	bool imageToIcon(const string& file, unsigned char* icon) const;

public:
	/**
	 * Constructor.
	 */
	WacomIntuos4LED();

	/**
	 * Destructor.
	 */
	~WacomIntuos4LED();

	/**
	 * Initialization of the tablet.
	 *
	 * This method initializes the tablet and the class data structures.
	 *
	 * @param vendor_id vendor id of the tablet
	 * @param  product_id product id of the table
	 *
	 * @returns true, if initialization was successful, false, otherwise.
	 */
	bool init(uint16_t vendor_id, uint16_t product_id);

	/**
	 * Autodetect and initialize the first tablet.
	 *
	 * @returns true, if initialization was successful, false, otherwise.
	 */
	bool autoDetectTablet();

	/**
	 * 
	 * Remove libusb interface before exit(0) of the end of main 
	 */
	void releaseInterface();

	/**
	 * Display a palette of all colors (gray tones) of the Intuos 4 LEDs.
	 *
	 * This method displays all available colors of the Intuos 4 tablet.
	 *
	 * @returns true, if the operation was successful, and false, otherwise.
	 */
	bool displayPalette(bool right_handed=true);

	/**
	 * Ths method transfers an library icon to the tablet.
	 *
	 * This method can be used to transfer a predefined library icon to the tablet.
	 *
	 * @param button number of the button to which the icon is assigned (must be between
	 * 0 and 7).
	 * @param name of the library icon.
	 * @param right_handed orientation of the tablet.
	 *
	 * @return true, if transmission was successful, false, otherwise.
	 *
	 */
	bool setLibraryIcon(unsigned int button, const string name, bool right_handed=true);

	/**
	 * Ths method transfers an image to the tablet.
	 *
	 * This method can be used to transfer an image to the tablet.
	 * The image MUST have a dimension of 64 columns and 32 rows. Before the transmission
	 * the image is converted to a grayscale image and thereafter approximated by an image with
	 * 16 grayscale values.
	 *
	 * @param button number of the button to which the icon is assigned (must be between
	 * 0 and 7).
	 * @param file  name of the image file.
	 * @param right_handed orientation of the tablet.
	 *
	 * @return true, if transmission was successful, false, otherwise.
	 */
	bool setImage(unsigned int button, const string file, bool right_handed=true);

	/**
	 * Dump an image as a C static array.
	 *
	 * This method dumps an image file as a 1024 byte C static array.
	 * The method may be used to extend the icon library.
	 *
	 * @param file  name of the image file.
	 * @param name name of the C static array.
	 *
	 * @return true, if transmission was successful, false, otherwise.
	 *
	 */
	bool dumpImageAsStaticData(const string file, const string name="icon");

	/**
	 * @param icon pointer to the 1024 byte array containing the icon data.
	 * @param name name of the C static array.
	 */
	void dumpIconAsStaticData(unsigned char *icon, string name="icon") const;

	/**
	 * Clear the image of a button.
	 *
	 * This method can be used to clear the icon of a particular button.
	 *
	 * @param button number of the button to which the icon is assigned (must be between
	 * 0 and 7).
	 * @param right_handed orientation of the tablet.
	 *
	 * @return true, if transmission was successful, false, otherwise.
	 */
	bool clearButton(unsigned int button, bool right_handed=true);

	/**
	 * Clear the icons of all buttons.
	 *
	 * @return true, if transmission was successful, false, otherwise.
	 */
	bool clearAllButtons();

	/**
	 * Dump names of library icons to standard output.
	 */
	void dumpIconNames() const;

};

#endif /* WACOMINTUOS4LED_H_ */
