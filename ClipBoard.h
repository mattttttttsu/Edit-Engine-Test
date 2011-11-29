#ifndef _CLIPBOARDMANAGER_H_
#define _CLIPBOARDMANAGER_H_


namespace EditTest {


/**
 * クリップボードのラッパークラス
 */
class Clipboard
{
private:
  /**
   * クリップボードを開けたか
   */
  BOOL opened;

  /**
   * クリップボードのオーナーのウィンドウハンドル
   */
  HWND window;

  

public:


  /**
   * コンストラクタ
   */
  Clipboard(HWND w)
  {
    window = w;

    opened = false;
    if(::OpenClipboard(window) != 0) {
      opened = true;
    }
  }


  /**
   * データをセットする。
   */
  void SetData()
  {
  }


  /**
   * データを取得する。
   */
  void GetData()
  {
  }


  /**
   * テキストをセットする。
   * @param LPWSTR text テキスト
   */
  void SetText(LPWSTR text)
  {
    if(!opened) {
      MessageBox(NULL, _T("Not opened."), _T(""), 0);
      return;
    }

    DWORD dataLen = wcslen(text) * sizeof(wchar_t);
    HGLOBAL gMem = ::GlobalAlloc(GMEM_MOVEABLE, dataLen + 2);

    unsigned char* memPtr = (unsigned char*)::GlobalLock(gMem);

    CopyMemory(memPtr, text, dataLen);
    memPtr[dataLen]   = 0;
    memPtr[dataLen+1] = 0;

    ::GlobalUnlock(gMem);

    ::SetClipboardData(CF_UNICODETEXT, gMem);
  }


  /**
   * クリップボードのデータをテキストとして取得する。
   * @return LPWSTR バッファへのポインタ。データが存在しない、取得できない場合はNULL。
   * - 
   */
  LPWSTR GetText()
  {
    if(!opened) {
      MessageBox(NULL, _T("Not opened."), _T(""), 0);
      return NULL;
    }

    HGLOBAL gMem = ::GetClipboardData(CF_UNICODETEXT);
    if(gMem == NULL) {
      return NULL;
    }

    LPWSTR memPtr = (LPWSTR)::GlobalLock(gMem);
    if(!memPtr) {
      return NULL;
    }

    DWORD dataLen = wcslen(memPtr);
    LPWSTR buffer = NULL;
    buffer = new wchar_t[dataLen + 1];
    wcsncpy(buffer, memPtr, dataLen);
    buffer[dataLen] = 0;

    ::GlobalUnlock(gMem);

    return buffer;
  }


  /**
   * クリップボードが現在使用可能かを返す。
   * @return BOOL 現在使用可能か
   */
  BOOL canUse()
  {
    return opened;
  }


  /**
   * クリップボードをクリアする。
   */
  void Clear()
  {
    if(opened) {
      ::EmptyClipboard();
    }
  }


  /**
   * デストラクタ
   */
  ~Clipboard()
  {
    if(opened) {
      ::CloseClipboard();
    }
  }
};


};



#endif