/*
 * TPCommon.h
 */
 
#ifndef __COMMON_H__
#define __COMMON_H__


#define ALERT_MEMORY_ERROR					"Memory Error"
#define ALERT_PHONE_ERROR					"Phone Error"

// Result display stuff
#define DISP_WIDTH							156
#define DISP_HEIGHT							DISP_WIDTH

#define RECT_SIZE_X							152
#define RECT_SIZE_Y							16

#define RECT_TOP_X							(DISP_WIDTH - RECT_SIZE_X) / 2
#define RECT_TOP_Y							3
#define TEXT_RIGHT_GAP						1

typedef enum {
	colorDefault,
	colorWarn,
	colorError,
	colorResult
} resultColor_e;

/*
 * Miscellaneous stuff
 */
typedef struct { 
	Boolean									bEnabled;
	UInt16									keyPressIdx;
	UInt16									optionMaskIdx;
	Boolean									bCopyToClipboard;
	UInt8									uPrecision;
	Boolean									bBeep;
	Boolean									bVibrate;
} AppPreferences_t;

#define PREFS_ENABLED						true
#define PREFS_OPTION_MASK_IDX				2
#define PREFS_KEYPRESS_IDX					2
#define PREFS_COPY_TO_CLIPBOARD				true
#define PREFS_PRECISION						3
#define PREFS_BEEP							true
#define PREFS_VIBRATE						true

#define INPUT_STR_SIZE						512
#define RESULT_STR_SIZE						64

typedef struct {
	Boolean									bActive;
	Char									inputStr[INPUT_STR_SIZE];
	Char									resultStr[RESULT_STR_SIZE];
} AppUSPrefs_t;

#endif /* __COMMON_H__ */

/*
 * TPCommon.h
 */

