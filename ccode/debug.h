/*	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
 *	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	----------------------------------------------------------------------
 */	

#ifndef __DEBUG__
#define __DEBUG__

#include <simstruc.h>

#define debug(...) ssPrintf(__VA_ARGS__)
#define printArray(source, nx) {int i; for (i=0; i<nx; i++) {ssPrintf("(%d)%f; ", i, (double)source[i]);}ssPrintf("\n");}

#endif /* __DEBUG__ */