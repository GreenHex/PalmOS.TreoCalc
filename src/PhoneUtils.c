/*
 * PhoneUtils.c
 */
 
#include "PhoneUtils.h"

/*
 * Alert
 */
void Alert(Char* MsgType, Char* Msg, Err err)
{
  	Char	errStr[32];

  	StrPrintF(errStr, " 0x%x", err);
  	FrmCustomAlert(10024, MsgType, Msg, errStr);
}

/*
 * vibrate()
 */
void vibrate(void)
{
 	UInt16 		count = 1;
	UInt16		state = kIndicatorForceOn;
	
	// it no worky...
	HsIndicatorState(count, kIndicatorTypeVibrator, &state);
	SysTaskDelay(5);
	state = kIndicatorForceOff;
	HsIndicatorState(count, kIndicatorTypeVibrator, &state);
	
	return;	

} // vibrate

/*
 * beep
 */
void beep(UInt8 numBeeps, UInt32 frequency, UInt16 duration)
{
	SndCommandType 			sndCmd;
	UInt16					SndVolume = PrefGetPreference(prefSysSoundVolume);

	UInt8					i = 0;
	
	sndCmd.cmd = sndCmdFreqDurationAmp; 				
	sndCmd.param1 = frequency; // 1760; 
	sndCmd.param2 = SND_DURATION;
	sndCmd.param3 = SndVolume;
	SndDoCmd(0, &sndCmd, true);	
	
	for (i = 1 ; i < numBeeps ; i++) // i = 1 because we already beeped once
	{
		sndCmd.cmd = sndCmdFreqDurationAmp; 				
		// sndCmd.param1 = PAUSE_FREQUENCY; // 1760; // this don't work
		sndCmd.param2 = PAUSE_DURATION;
		sndCmd.param3 = 0;
		SndDoCmd(0, &sndCmd, true);	
		
		sndCmd.cmd = sndCmdFreqDurationAmp;				
		sndCmd.param1 = frequency; // 1760; 
		sndCmd.param2 = SND_DURATION;
		sndCmd.param3 = SndVolume;
		SndDoCmd(0, &sndCmd, true);
	}
}

/*
 * RomVersionCompatible
 */
Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{	
	UInt32 	rom = 0;
	UInt32 	hal = 0;
	UInt32	company = 0;
	UInt32	device = 0;
	UInt32	hsFtrVersion;

	if(FtrGet(hsFtrCreator, hsFtrIDVersion, &hsFtrVersion) != 0)
		return(1);

    FtrGet(sysFtrCreator, sysFtrNumOEMHALID, &hal);
    FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &company);
    FtrGet(sysFtrCreator, sysFtrNumOEMDeviceID, &device);
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &rom);
		
	if (rom >= requiredVersion
		&&	(company == kPalmCompanyIDPalm
			|| company == kPalmCompanyIDOldPalm
			|| kPalmCompanyIDHandspring)

		&&	(device == kPalmOneDeviceIDTreo650
			|| device == kPalmOneDeviceIDTreo650Sim

			|| device == kPalmOneDeviceIDTreo600
			|| device == kPalmOneDeviceIDTreo600Sim

			|| device == 'D053' // Treo 680
			|| device == 'H104'

			|| device == 'D052' // Treo 700p
			|| device == 'H103'

			|| device == 'D060' // Treo 755p
			|| device == 'H104'
			
			|| device == 'D061' // Centro ???
			|| device == 'D062' // Centro
			)
			
		&&	(hal == hsHALIDHandspringOs5Rev2 // Treo 650
			|| hal == hsHALIDHandspringOs5Rev2Sim

			|| hal == hsHALIDHandspringOs5Rev1 // Treo 600
			|| hal == hsHALIDHandspringOs5Rev1Sim

			|| hal == hsHALIDHandspringOs5Rev4 // Treo 680, 755p
			|| hal == hsHALIDHandspringOs5Rev4Sim

			|| hal == hsHALIDHandspringOs5Rev3 // Treo 700p
			|| hal == hsHALIDHandspringOs5Rev3Sim
			
			|| hal == 'P052' // Centro ???
			|| hal == 'P053' // Centro
		))
	{
		return 0;
	}
	
	if ((launchFlags & LAUNCH_FLAGS) == LAUNCH_FLAGS)
	{
		// Pilot 1.0 will continuously relaunch this app unless we switch to 
		// another safe one.
		if (rom < sysMakeROMVersion(2, 0, 0, sysROMStageRelease, 0))
			AppLaunchWithCommand (sysFileCDefaultApp, 
					sysAppLaunchCmdNormalLaunch, NULL);
	}
	return sysErrRomIncompatible;
}

