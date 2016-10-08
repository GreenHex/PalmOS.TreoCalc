/*
 * AppMain.c
 */

#include <PalmOS.h>
#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <Form.h>
// #include <TonesLib.h>
#include <Clipboard.h>
#include <Window.h>
#include <HardwareUtils68K.h>

#include "Global.h"
#include "PhoneUtils.h"
#include "Calc.h"
#include "AppResources.h"

// Prototypes
static void 				RegisterForNotifications(Boolean bRegister);
static void 				writePrefs(AppPreferences_t* pPrefs);
static void 				readPrefs(AppPreferences_t* pPrefs);
static void 				writeUSPrefs(AppUSPrefs_t* pUSPrefs);
static void 				readUSPrefs(AppUSPrefs_t* pUSPrefs);
static Err 					AppStart(void);
static void 				AppStop(void);
static void 				InitializeMainForm(FormType* pForm);
static Boolean 				MainMenuProcessEvent(UInt16 itemID);
static void 				displayResult(Char* str, resultColor_e color);
static void 				SetButtonEnabledState(FormType* pForm, ControlType* pCtl, Boolean state);
static Boolean 				MainFormHandleEvent(EventType* pEvent);
static Boolean 				AppHandleEvent(EventType* pEvent);
static void 				AppEventLoop(void);

UInt32 						PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags);

/*
 * writePrefs
 */
static void writePrefs(AppPreferences_t* pPrefs)
{
	PrefSetAppPreferences(appFileCreator, appPrefID, appPrefVersionNum, pPrefs, sizeof(AppPreferences_t), true);
}

/*
 * readPrefs
 */
static void readPrefs(AppPreferences_t* pPrefs)
{	
	UInt16 prefSize = sizeof(AppPreferences_t);
	
	if (PrefGetAppPreferences(appFileCreator, appPrefID, pPrefs, &prefSize, true) == noPreferenceFound)
	{	
		// default application preference values
		pPrefs->bEnabled = PREFS_ENABLED;
		pPrefs->keyPressIdx = PREFS_KEYPRESS_IDX;
		pPrefs->optionMaskIdx = PREFS_OPTION_MASK_IDX;
		pPrefs->bCopyToClipboard = PREFS_COPY_TO_CLIPBOARD;
		pPrefs->uPrecision = PREFS_PRECISION;
		pPrefs->bBeep = PREFS_BEEP;
		pPrefs->bVibrate = PREFS_VIBRATE;
		
		writePrefs(pPrefs);
	}
} // readPrefs

/*
 * writeUSPrefs
 */
static void writeUSPrefs(AppUSPrefs_t* pUSPrefs)
{
	PrefSetAppPreferences(appFileCreator, appPrefID, appPrefVersionNum, pUSPrefs, sizeof(AppUSPrefs_t), false);

} // writeUSPrefs

/*
 * readUSPrefs
 */
static void readUSPrefs(AppUSPrefs_t* pUSPrefs)
{	
	UInt16 usPrefSize = sizeof(AppUSPrefs_t);
	
	if (PrefGetAppPreferences(appFileCreator, appPrefID, pUSPrefs, &usPrefSize, false) == noPreferenceFound)
	{	
		pUSPrefs->bActive = false;
		pUSPrefs->inputStr[0] = chrNull;
		pUSPrefs->resultStr[0] = chrNull;

		writeUSPrefs(pUSPrefs);
	}
} // readUSPrefs

/*
 * AppStart
 */
static Err AppStart(void)
{
	FrmGotoForm(MAIN_FORM);
	return errNone;
}

/*
 * AppStop
 */
static void AppStop(void)
{
	FrmCloseAllForms();	
}

/*
 * RegisterForNotifications
 */
static void RegisterForNotifications(Boolean bRegister)
{
	UInt16 				cardNo; 
	LocalID  			dbID;
	
	SysCurAppDatabase(&cardNo, &dbID);
		
	if (bRegister)
	{
		SysNotifyRegister(cardNo, dbID, sysNotifyVirtualCharHandlingEvent, NULL, sysNotifyNormalPriority, NULL);	
	}
	else
	{
		SysNotifyUnregister(cardNo, dbID, sysNotifyVirtualCharHandlingEvent, sysNotifyNormalPriority);
	}
				
} // RegisterForNotifications

/*
 * InitializeMainForm
 */
static void InitializeMainForm(FormType* pForm)
{	
	FieldType*				pField = NULL;
	MemHandle				hText = NULL;
	Char*					pText = NULL;

	AppUSPrefs_t			usPrefs;
	
	readUSPrefs(&usPrefs);
					
	hText = MemHandleNew(StrLen(usPrefs.inputStr) + 1);
	pText = MemHandleLock(hText);
	StrCopy(pText, usPrefs.inputStr);
	
	pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));
	FldSetTextHandle(pField, NULL);
	FldSetTextHandle(pField, hText);
	
	MemHandleUnlock(hText);
	
	FrmDrawForm(pForm);
	FrmNavRemoveFocusRing(pForm);
	
	FldSetInsPtPosition(pField, StrLen(pText));
	FldDrawField(pField);
	FrmSetFocus(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));
	
} // InitializeMainForm

/*
 * MainMenuProcessEvent
 */
static Boolean MainMenuProcessEvent(UInt16 itemID)
{
	Boolean 				retVal = false;
	FormType*				pForm = NULL;
	ControlType*    		pCtl = NULL;
	ListType*				pList = NULL;
	
	AppPreferences_t		prefs;
	
	switch (itemID)
	{
		case MENU_PREFERENCES:
			
			readPrefs(&prefs);
			
			pForm = FrmInitForm(PREFS_FORM);
			
			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_ENABLE_CHECKBOX));
			CtlSetValue(pCtl, prefs.bEnabled);
			
			pList = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_ACTIVATION_KEY_LIST));
			LstSetSelection(pList, prefs.keyPressIdx);
			LstSetHeight(pList, 10);
			LstSetTopItem(pList, prefs.keyPressIdx);
			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_ACTIVATION_KEY_POPUP));
			CtlSetLabel(pCtl, LstGetSelectionText(pList, LstGetSelection(pList)));
			
			pList = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_MODIFIER_KEY_LIST));
			LstSetSelection(pList, prefs.optionMaskIdx);
			LstSetHeight(pList, 4);
			LstSetTopItem(pList, prefs.optionMaskIdx);
			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_MODIFIER_KEY_POPUP));
			CtlSetLabel(pCtl, LstGetSelectionText(pList, LstGetSelection(pList)));
			
			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_COPY_TO_CLIPBOARD_CHECKBOX));
			CtlSetValue(pCtl, prefs.bCopyToClipboard);
			
			pList = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_PRECISION_LIST));
			LstSetSelection(pList, prefs.uPrecision);
			LstSetHeight(pList, 7);
			LstSetTopItem(pList, prefs.uPrecision);
			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_PRECISION_POPUP));
			CtlSetLabel(pCtl, LstGetSelectionText(pList, LstGetSelection(pList)));

			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_BEEP_CHECKBOX));
			CtlSetValue(pCtl, prefs.bBeep);
			
			pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_VIBRATE_CHECKBOX));
			CtlSetValue(pCtl, prefs.bVibrate);
						
			if (FrmDoDialog(pForm) == PREFS_DONE_BUTTON)
			{
				pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_ENABLE_CHECKBOX));
				prefs.bEnabled = (CtlGetValue(pCtl) == 1);
						
				pList = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_ACTIVATION_KEY_LIST));
				prefs.keyPressIdx = LstGetSelection(pList);
						
				pList = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_MODIFIER_KEY_LIST));
				prefs.optionMaskIdx = LstGetSelection(pList);
						
				pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_COPY_TO_CLIPBOARD_CHECKBOX));
				prefs.bCopyToClipboard = (CtlGetValue(pCtl) == 1);
				
				pList = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_PRECISION_LIST));
				prefs.uPrecision = LstGetSelection(pList);

				pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_BEEP_CHECKBOX));
				prefs.bBeep = (CtlGetValue(pCtl) == 1);

				pCtl = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, PREFS_VIBRATE_CHECKBOX));
				prefs.bVibrate = (CtlGetValue(pCtl) == 1);
						
				writePrefs(&prefs);
				
				RegisterForNotifications(prefs.bEnabled);	
			}
		
			FrmDeleteForm(pForm);	
			
			retVal = true;
			break;
			
		case MENU_ABOUT:
		
			pForm = FrmInitForm(ABOUT_FORM);
			FrmDoDialog(pForm);
			FrmDeleteForm(pForm);	
			
			retVal = true;
			break;
			
		case MENU_EXIT:
			{
				EventType			event;
				
				event.eType = appStopEvent;
				EvtAddEventToQueue(&event);
				EvtWakeup();
			}
			break;
			
		default:
			break;	
	}	
	return retVal;
	
} // MainMenuProcessEvent

// Defines and Globals


/*
 * displayResult
 */
static void displayResult(Char* str, resultColor_e color)
{	
	RectangleType 			rect = { {RECT_TOP_X, RECT_TOP_Y}, {RECT_SIZE_X, RECT_SIZE_Y} };
    
	RGBColorType 			rgb;
    IndexedColorType		colorWhite;
    IndexedColorType		colorBlack;

    UInt16					strWidth = 0;
		
	WinPushDrawState();
	
	rgb.r=255; rgb.g=255; rgb.b=255; // white
	colorWhite = WinRGBToIndex(&rgb);	
	
	rgb.r=0; rgb.g=0; rgb.b=0; // black
	colorBlack = WinRGBToIndex(&rgb);
		
	WinSetForeColor(colorBlack);
	// WinSetBackColor(colorWhite);
	WinSetForeColor(UIColorGetTableEntryIndex(UIFormFill));	
	
	WinEraseRectangle(&rect, 0);
	
	if (str)
	{
		IndexedColorType 		colorText;
		FontID					outputFont;
		
		switch (color)
		{
			case colorWarn:
			
				rgb.r=255; rgb.g=153; rgb.b=102; // orange?
				outputFont = largeBoldFont;
				
				break;
				
			case colorError:
				
				rgb.r=255; rgb.g=0; rgb.b=51; // red?
				outputFont = largeBoldFont;
				
				break;
				
			case colorResult:
				{
					Char*	c = str;
					
					rgb.r=0; rgb.g=153; rgb.b=51; // green?
					outputFont = ledFont;
					
					while (*(c++))
					{
						if (*c == 'e')
							*c = 0x002F;
					}
				}
				break;
			
			default:
			
				rgb.r=102; rgb.g=0; rgb.b=255; // blue?
				outputFont = largeBoldFont;
				
				break;
		}
		
		FntSetFont(outputFont);
		colorText = WinRGBToIndex(&rgb);	
		WinSetTextColor(colorText);
		   
		strWidth = FntLineWidth(str, StrLen(str));
	    
	    WinPaintChars(str, StrLen(str),
	    	RECT_TOP_X + (RECT_SIZE_X - strWidth) - TEXT_RIGHT_GAP,
	    	RECT_TOP_Y);
	}
	
	WinPopDrawState();

} // displayResult

/*
 * SetButtonEnabledState
 */
static void SetButtonEnabledState(FormType* pForm, ControlType* pCtl, Boolean state)
{
	UInt16 		numObjects = FrmGetNumberOfObjects(pForm);
	//UInt16		pObject = NULL;
	UInt16		i = 0;
	
	for (i = 0 ; i < numObjects ; i++)
	{
		if (state)
		{
			if (FrmGetObjectType(pForm, i) == frmControlObj)
			{
				CtlSetEnabled(FrmGetObjectPtr(pForm, i), true);
				// FrmShowObject(pForm, i);
			}
		}
		else
		{
			if ((FrmGetObjectPtr(pForm, i) != pCtl) && (FrmGetObjectType(pForm, i) == frmControlObj))
			{
				CtlSetEnabled(FrmGetObjectPtr(pForm, i), false);
				// FrmHideObject(pForm, i);
			}
		}	
	}
		
} // SetButtonEnabledState

/*
 * MainFormHandleEvent
 */
static Boolean MainFormHandleEvent(EventType* pEvent)
{
	Boolean 				handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	
	FieldType*				pField = NULL;
	MemHandle				hText = NULL;
	Char*					pText = NULL;
	
	AppPreferences_t		prefs;					
	AppUSPrefs_t			usPrefs;

	MemSet(&prefs, sizeof(AppPreferences_t), 0);
	readPrefs(&prefs);
	
	switch (pEvent->eType)
	{		
		case frmOpenEvent:
		
			InitializeMainForm(pForm);
				
			handled = true;
	
			break;
				
		case frmCloseEvent:
		
			break;
			
		case fldChangedEvent:
		
			if (pEvent->data.fldChanged.fieldID == MAIN_INPUT_FIELD)
			{
				pField = pEvent->data.fldChanged.pField; // FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));
					
				hText = FldGetTextHandle(pField);
				if (hText)
				{
					pText = MemHandleLock(hText);
					if (pText)
					{
						StrCopy(usPrefs.inputStr, pText);
						writeUSPrefs(&usPrefs);
					}
					MemHandleUnlock(hText);
				}
				
				// erase the answer field
				/*
				pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_OUTPUT_FIELD));
				FldSetTextHandle(pField, NULL);
				FldDrawField(pField);
				*/
				displayResult(NULL, colorDefault);
							
				handled = true;
			}
			break;
		
		case ctlEnterEvent:
			
			switch (pEvent->data.ctlSelect.controlID)
			{	
				case MAIN_CLEAR_BUTTON:
					
					MemSet(usPrefs.inputStr, INPUT_STR_SIZE, 0);
					MemSet(usPrefs.resultStr, RESULT_STR_SIZE, 0);
					writeUSPrefs(&usPrefs);
					
					pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));
					FldSetTextHandle(pField, NULL);
					
					FldDrawField(pField);
					
					/*
					pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_OUTPUT_FIELD));
					FldSetTextHandle(pField, NULL);

					FldDrawField(pField);
					*/					
					
					displayResult(NULL, colorDefault);
					
					break;
					
				case MAIN_BACKSPACE_BUTTON:
					
					pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));
					
					if (FldGetInsPtPosition(pField))
					{
						FldDelete(pField, FldGetInsPtPosition(pField) - 1, FldGetInsPtPosition(pField));
					}
					FldDrawField(pField);			
					
					break;
				
				case MAIN_EQUALS_BUTTON:
				
					readUSPrefs(&usPrefs);
					MemSet(usPrefs.resultStr, RESULT_STR_SIZE, 0);
					if (calculate(usPrefs.inputStr, usPrefs.resultStr, prefs.uPrecision))
					{
						displayResult(usPrefs.resultStr, colorResult);	
					}
					else
					{
						displayResult(usPrefs.resultStr, colorError);
					}
						
					
					/*
					hText = MemHandleNew(StrLen(usPrefs.resultStr) + 1);
					pText = MemHandleLock(hText);
					StrCopy(pText, usPrefs.resultStr);
					
					pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_OUTPUT_FIELD));
					FldSetTextHandle(pField, NULL);
					FldSetTextHandle(pField, hText);
					
					MemHandleUnlock(hText);
					
					
					FldSetInsPtPosition(pField, StrLen(pText));
					FldDrawField(pField);
					*/
					
					if (prefs.bCopyToClipboard)
						ClipboardAddItem(clipboardText, usPrefs.resultStr, StrLen(usPrefs.resultStr));
									
					break;
					
				default:
					
					pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));
					
					FldInsert(pField, CtlGetLabel(pEvent->data.ctlSelect.pControl), 1);
				
					FldDrawField(pField);
					
					break;
			}
	
			SetButtonEnabledState(pForm, pEvent->data.ctlEnter.pControl, false);
	
			if (prefs.bBeep) beep(1, SND2_FREQUENCY, SND_DURATION);
			if (prefs.bVibrate) vibrate();

			break;
			
		case ctlSelectEvent:
		
			SetButtonEnabledState(pForm, NULL, true);

			// if (prefs.bBeep) beep(1, SND2_FREQUENCY, SND_DURATION);
			// if (prefs.bVibrate) vibrate();

			break;
			
		case ctlExitEvent:
		
			SetButtonEnabledState(pForm, NULL, true);
			
			// if (prefs.bBeep) beep(1, SND2_FREQUENCY, SND_DURATION);
			// if (prefs.bVibrate) vibrate();

			break;
			
		case keyUpEvent:		
			{
				UInt16			chr = pEvent->data.keyUp.chr;
				
				if ((prefs.bBeep)
						&& (((chr >= chrDigitZero) && (chr <= chrDigitNine))
						|| (chr == chrBackspace)))
				{
					beep(1, SND2_FREQUENCY, SND_DURATION);
				}
			}
			break;
			
		case keyDownEvent:
			{
				UInt16			chr = pEvent->data.keyDown.chr;
			
				if ((chr >= chrDigitZero) && (chr <= chrDigitNine))
					handled = false;
				else if (chr == periodChr)
					handled = false;
				else if (chr == chrBackspace)
					handled = false;
				else if (chr == chrVerticalTabulation)
					handled = false;
				else if (chr > 0x0100)
					handled = false;
				else
					handled = true;
				
				if (((chr >= chrDigitZero) && (chr <= chrDigitNine))
						|| (chr == periodChr)
						|| (chr == chrBackspace))
				{
					pField = FrmGetObjectPtr(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));	
					FldSendChangeNotification(pField);

					FldDrawField(pField);
					
					if (prefs.bBeep)
						beep(1, SND1_FREQUENCY, SND_DURATION);
				}
				else if ((chr == chrCarriageReturn)
						|| (chr == chrLineFeed)
						|| (chr == vchrRockerCenter)
						|| (chr == chrSpace))
				{
					readUSPrefs(&usPrefs);
					MemSet(usPrefs.resultStr, RESULT_STR_SIZE, 0);
					if (calculate(usPrefs.inputStr, usPrefs.resultStr, prefs.uPrecision))
					{
						displayResult(usPrefs.resultStr, colorResult);	
					}
					else
					{
						displayResult(usPrefs.resultStr, colorError);
					}
					
					if (prefs.bBeep) beep(1, SND2_FREQUENCY, SND_DURATION);
					if (prefs.bVibrate) vibrate();

				}
			}
			break;
			
		case frmObjectFocusLostEvent:
		case frmObjectFocusTakeEvent:
		
			handled = true;
			break;
		
		case menuEvent:
			
			handled = MainMenuProcessEvent(pEvent->data.menu.itemID);
			
			break;
			
		default:
			break;
	}

	FrmNavRemoveFocusRing(pForm);
	FrmSetFocus(pForm, FrmGetObjectIndex(pForm, MAIN_INPUT_FIELD));

	return handled;
	
} // MainFormHandleEvent

/*
 * AppHandleEvent
 */
static Boolean AppHandleEvent(EventType* pEvent)
{
	UInt16 		formID;
	FormType* 	pForm;
	Boolean		handled = false;

	if (pEvent->eType == frmLoadEvent)
	{
		// Load the form resource.
		formID = pEvent->data.frmLoad.formID;
		
		pForm = FrmInitForm(formID);
		FrmSetActiveForm(pForm);

		// Set the event handler.
		if (formID == MAIN_FORM)
			FrmSetEventHandler(pForm, MainFormHandleEvent);
			
		handled = true;
	}
	
	return handled;

} // AppHandleEvent

/*
 * AppEventLoop
 */
static void AppEventLoop(void)
{
	Err			error;
	EventType	event;

	do {
		EvtGetEvent(&event, evtWaitForever);

		if (SysHandleEvent(&event))
			continue;
			
		if (MenuHandleEvent(0, &event, &error))
			continue;
			
		if (AppHandleEvent(&event))
			continue; 

		FrmDispatchEvent(&event);
		
		if ((event.eType == fldEnterEvent)
				&& (event.data.fldEnter.fieldID == MAIN_OUTPUT_FIELD))
		{
			break;
		}
			
	} while (event.eType != appStopEvent);

} // AppEventLoop

/*
 * PilotMain
 */
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	Err 						error = errNone;
	
	UInt16						keyPressOptionMask[4] = { 0, shiftKeyMask, optionKeyMask, doubleTapKeyMask };
	UInt16 						keyPress[10] = { 0, vchrRockerUp, vchrRockerDown, vchrRockerLeft,
										vchrRockerRight, vchrRockerCenter, vchrLaunch, vchrMenu, vchrHard2, vchrHard3 };
	
	AppUSPrefs_t				usPrefs;
	AppPreferences_t 			prefs;

	MemSet(&prefs, sizeof(AppPreferences_t), 0);
  	readPrefs(&prefs);
	
	switch (cmd)
	{
		case sysAppLaunchCmdNormalLaunch:

			// Check device
			if ((error = RomVersionCompatible(MIN_VERSION, launchFlags)))
			{
				Alert(ALERT_PHONE_ERROR, "Incompatible Device.", error);
				return error;
			}
	
			if ((error = AppStart()) == 0)
			{
				RectangleType		rect = { { 2, 2 }, { 156, 156 } };
    
    			WinPushDrawState();
    		
				WinSetForeColor(UIColorGetTableEntryIndex(UIDialogFrame));
	
				WinPaintRectangleFrame(dialogFrame, &rect);
				WinPopDrawState();
				
				readUSPrefs(&usPrefs);
				usPrefs.bActive = true;
				writeUSPrefs(&usPrefs);
							
				AppEventLoop();				
				AppStop();
				
				readUSPrefs(&usPrefs);
				usPrefs.bActive = false;
				writeUSPrefs(&usPrefs);
			}
			break;
			
		// Register for notifications on reset
		case sysAppLaunchCmdSystemReset:
			// Check device
			if (!(error = RomVersionCompatible(MIN_VERSION, launchFlags)))
			{
				RegisterForNotifications(prefs.bEnabled);
			}		
			break;
			
		case sysAppLaunchCmdNotify:
		
			if (((SysNotifyParamType*)cmdPBP)->notifyType == sysNotifyVirtualCharHandlingEvent)
			{
				readUSPrefs(&usPrefs);
				
				if ((prefs.keyPressIdx) && (!usPrefs.bActive))
				{
					SysNotifyParamType* 		notifyParam = (SysNotifyParamType *)cmdPBP;
				
					UInt16						keyMask = ((SysNotifyVirtualCharHandlingType *)notifyParam->notifyDetailsP)->keyDown.modifiers;
					UInt16						keyCode = ((SysNotifyVirtualCharHandlingType *)notifyParam->notifyDetailsP)->keyDown.keyCode;
					
					if ((keyMask & keyPressOptionMask[prefs.optionMaskIdx]) //  That's "&" NOT "&&"
							|| (prefs.optionMaskIdx == 0))
					{
						if (keyCode == keyPress[prefs.keyPressIdx])
						{
							FormType*				pForm = NULL;
							
							usPrefs.bActive = true;
							writeUSPrefs(&usPrefs);
							
							if (prefs.bBeep)
								beep(1, SND1_FREQUENCY / 2, 10);
														
							pForm = FrmInitForm(MAIN_FORM);
							FrmSetEventHandler(pForm, MainFormHandleEvent);
							InitializeMainForm(pForm);
							FrmSetActiveForm(pForm);
							FrmDrawForm(pForm);
							
							AppEventLoop();
							
							readUSPrefs(&usPrefs);
							usPrefs.bActive = false;
							writeUSPrefs(&usPrefs);
	
							if (prefs.bBeep)						
								beep(2, SND1_FREQUENCY / 2, 10);
							
							FrmReturnToForm(0);
							
							((SysNotifyParamType*)cmdPBP)->handled = true;
						}	
					}
				}
			}
			break;
			
		default:
			break;
	}

	return error;
	
} // PilotMain

/*
 * AppMain.c
 */

