/*
 * PhoneUtils.h
 */

#ifndef __PHONEUTILS_H__
#define __PHONEUTILS_H__

#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <Form.h>

#include "Global.h"
#include "AppResources.h"

#define MIN_VERSION  			sysMakeROMVersion(5, 0, 0, sysROMStageRelease, 0)
#define LAUNCH_FLAGS 			(sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)

#define SND1_FREQUENCY			400 // 1760
#define SND2_FREQUENCY			300 // 1760
#define SND_DURATION			20

#define PAUSE_FREQUENCY			1 // 0 doesn't play any sound // we don't use this anyway
#define PAUSE_DURATION			50

// Prototypes
extern Boolean 					IsPhoneGSM(void);
extern void						vibrate(void);
extern void 					beep(UInt8 numBeeps, UInt32 frequency, UInt16 duration);
extern void 					getROMID(UInt8* keyVal);
extern void 					Alert(Char* MsgType, Char* Msg, Err err);
extern Err 						RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags);

#endif /* __PHONEUTILS_H__ */
 
/*
 * PhoneUtils.h
 */