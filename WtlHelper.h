#ifndef _WTL_HELPER_H_
#define _WTL_HELPER_H_


namespace WtlHelper {

/**
 * メッセージマクロ
 */

// void OnImeStartComposition()
#define MSG_WM_IME_STARTCOMPOSITION(func) \
  if (uMsg == WM_IME_STARTCOMPOSITION) \
	{ \
		SetMsgHandled(FALSE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}


// BOOL OnImeComposition(DWORD dbcsChar, DWORD flags)
#define MSG_WM_IME_COMPOSITION(func) \
  if (uMsg == WM_IME_COMPOSITION) \
	{ \
		SetMsgHandled(func(wParam, lParam)); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}


// void OnImeEndComposition()
#define MSG_WM_IME_ENDCOMPOSITION(func) \
  if (uMsg == WM_IME_ENDCOMPOSITION) \
	{ \
		SetMsgHandled(FALSE); \
		func(); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}



/**
 * WTLで使用する補助的な関数の定義
 */
class WtlHelper {

};

};


#endif
