/*
 * intuos4-led-check.cpp
 *
 *  Created on: 11.07.2010
 *      Author: Christoph Karg
 */

#include <iostream>
#include <unistd.h>
#include "WacomIntuos4LED.h"

using namespace std;

void checkWacomIntuos4LED() {
	uint16_t vendor_id = 0x056a;
	uint16_t product_id = 0x00b9;
	WacomIntuos4LED intuos4;
	unsigned char icon[1024];
	bool right_handed=true;
	//bool right_handed=false;

	if (!intuos4.autoDetectTablet()) {
		cerr << "ERROR: No Intuos 4 tablet found." << endl;
		return;
	}

	intuos4.clearAllButtons();
	intuos4.displayPalette(right_handed);
	sleep(2);

	intuos4.clearAllButtons();
	for (int b=0; b<8; b++) {
		//intuos4.setIcon(b%8, icon, right_handed);
		if (b%2==0) {
			intuos4.setLibraryIcon(b%8, "Tux", right_handed);
		} else {
			intuos4.setLibraryIcon(b%8, "WacomLogo", right_handed);
		}
		sleep(1);
		//intuos4.clearButton(b%8, right_handed);
	}

	intuos4.setLibraryIcon(0, "ButtonOne", right_handed);
	intuos4.setLibraryIcon(1, "ButtonTwo", right_handed);
	intuos4.setLibraryIcon(2, "ButtonThree", right_handed);
	intuos4.setLibraryIcon(3, "ButtonFour", right_handed);
	intuos4.setLibraryIcon(4, "ButtonFive", right_handed);
	intuos4.setLibraryIcon(5, "ButtonSix", right_handed);
	intuos4.setLibraryIcon(6, "ButtonSeven", right_handed);
	intuos4.setLibraryIcon(7, "ButtonEight", right_handed);

}

int main() {
	checkWacomIntuos4LED();
	return 0;
}
