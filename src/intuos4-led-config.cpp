/*
 * intuos4led-config.cpp
 *
 *  Created on: 20.07.2010
 *      Author: Christoph Karg
 */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <getopt.h>
#include "WacomIntuos4LED.h"

using namespace std;

void help(char* name) {
	cout << endl << "Usage: " << endl << endl;
	cout << name << " [--lefthanded] "
			<< "[--vendorid <vid> --productid <pid>] "
			<< " --button <id> --icon <name> --> Set library icon <name> to button <id>"
			<< endl << endl;

	cout << name << " [--lefthanded] "
			<< "[--vendorid <vid> --productid <pid>] "
			<< "--button <id> --image <filename> --> Set image <filename> to button <id>"
			<< endl << endl;

	cout << name
			<< " --dump <filename> --> Dump image <filename> as C static array"
			<< endl << endl;
	cout << name << " --list" << " -> List library icon names" << endl << endl;
	cout << name << " --help" << " -> Display this help text" << endl << endl;
}

int main(int argc, char **argv) {
	// List of allowed options
	static struct option long_options[] = { { "help", no_argument, 0, 'h' }, {
			"list", no_argument, 0, 'l' },
			{ "lefthanded", no_argument, 0, 'e' }, { "button",
					required_argument, 0, 'b' }, { "icon", required_argument,
					0, 'i' }, { "image", required_argument, 0, 'm' }, { "dump",
					required_argument, 0, 'd' }, { "vendorid",
					required_argument, 0, 'v' }, { "productid",
					required_argument, 0, 'p' }, { 0, 0, 0, 0 } };
	WacomIntuos4LED intuos4;
	int c;
	bool done = false;
	int button = -1;
	bool tablet_ok = false;
	bool right_handed = true;
	bool set_icon = false;
	bool set_image = false;
	bool dump_image = false;

	string name;
	int cmd_counter = 0;
	uint16_t vendor_id = 0, product_id = 0;

	while (!done) {
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long_only(argc, argv, "hleb:i:m:v:p:d:", long_options,
				&option_index);

		/* Detect the end of the options. */
		if (c == -1) {
			done = true;
		} else {
			switch (c) {
			case 'h':
				help(argv[0]);
				exit(0);
				break;

			case 'b':
				button = atoi(optarg);
				break;

			case 'v':
				vendor_id = atoi(optarg);
				break;

			case 'p':
				product_id = atoi(optarg);
				break;

			case 'l':
				cmd_counter++;
				intuos4.dumpIconNames();
				exit(0);
				break;

			case 'e':
				right_handed = false;
				break;

			case 'i':
				cmd_counter++;
				set_icon = true;
				name = optarg;
				break;

			case 'd':
				cmd_counter++;
				dump_image = true;
				name = optarg;
				break;

			case 'm':
				cmd_counter++;
				set_image = true;
				name = optarg;
				break;

			case '?':
				help(argv[0]);
				exit(1);

			default:
				help(argv[0]);
				exit(1);
			}

		}

	}

	if (cmd_counter != 1) {
		help(argv[0]);
		exit(1);
	}

	if ((set_icon || set_image) && (button < 1 || button > 8)) {
		cout << "ERROR: invalid button id." << endl;
		help(argv[0]);
		exit(1);
	}

	if (set_icon || set_image) {
		if (vendor_id > 0 && product_id > 0) {
			tablet_ok = intuos4.init(vendor_id, product_id);
		} else {
			tablet_ok = intuos4.autoDetectTablet();
		}

		if (!tablet_ok) {
			cout << "ERROR: " << "tablet could not be initialized." << endl;
			exit(1);
		}
	}

	if (set_icon) {
		intuos4.setLibraryIcon(button - 1, name, right_handed);
	}

	if (set_image) {
		intuos4.setImage(button - 1, name, right_handed);
	}

	if (dump_image) {
		intuos4.dumpImageAsStaticData(name);
		return 0;
	}

	intuos4.releaseInterface();

	exit(0);
}

