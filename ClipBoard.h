#ifndef _CLIPBOARDMANAGER_H_
#define _CLIPBOARDMANAGER_H_


namespace EditTest {


/**
 * �N���b�v�{�[�h�̃��b�p�[�N���X
 */
class Clipboard
{
private:
  /**
   * �N���b�v�{�[�h���J������
   */
  BOOL opened;

  /**
   * �N���b�v�{�[�h�̃I�[�i�[�̃E�B���h�E�n���h��
   */
  HWND window;

  

public:


  /**
   * �R���X�g���N�^
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
   * �f�[�^���Z�b�g����B
   */
  void SetData()
  {
  }


  /**
   * �f�[�^���擾����B
   */
  void GetData()
  {
  }


  /**
   * �e�L�X�g���Z�b�g����B
   * @param LPWSTR text �e�L�X�g
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
   * �N���b�v�{�[�h�̃f�[�^���e�L�X�g�Ƃ��Ď擾����B
   * @return LPWSTR �o�b�t�@�ւ̃|�C���^�B�f�[�^�����݂��Ȃ��A�擾�ł��Ȃ��ꍇ��NULL�B
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
   * �N���b�v�{�[�h�����ݎg�p�\����Ԃ��B
   * @return BOOL ���ݎg�p�\��
   */
  BOOL canUse()
  {
    return opened;
  }


  /**
   * �N���b�v�{�[�h���N���A����B
   */
  void Clear()
  {
    if(opened) {
      ::EmptyClipboard();
    }
  }


  /**
   * �f�X�g���N�^
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