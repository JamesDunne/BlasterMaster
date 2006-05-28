#	include "basic.h"
	// Control:
	unsigned char	control_keys;				// Control keys pressed THIS FRAME
	unsigned char	old_control_keys;			// Control keys pressed LAST FRAME

	unsigned long	warp_time;
	unsigned long	warp_tag;
	unsigned long	warp_door;

	unsigned long	items;

	unsigned long	predoorx, predoory;
	unsigned long	postdoorx, postdoory;
	fixed			predoormx, predoormy;
	fixed			postdoormx, postdoormy;
