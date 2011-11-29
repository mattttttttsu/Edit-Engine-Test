// editorView.h : interface of the CEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _EDITORVIEW_H_
#define _EDITORVIEW_H_

#include "CustomEdit.h"
#include "Resource.h"
#include "WtlHelper.h"

namespace EditTest{

class CEditorView : public CWindowImpl<CEditorView>
{
private:

	CStatic stat;
  CustomEdit edit;


public:
	DECLARE_WND_CLASS(NULL);

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CEditorView);
		MSG_WM_CREATE(OnCreate);
    MSG_WM_SIZE(OnSize);
    MSG_WM_KEYDOWN(OnKeyDown);
    MSG_WM_CHAR(OnChar);
    MSG_WM_IME_STARTCOMPOSITION(OnImeStartComposition);
    MSG_WM_IME_COMPOSITION(OnImeComposition);
    MSG_WM_IME_ENDCOMPOSITION(OnImeEndComposition);

    MSG_WM_MOUSEWHEEL(OnMouseWheel);

		//MSG_WM_PAINT(OnPaint)
		REFLECT_NOTIFICATIONS();
	END_MSG_MAP();

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(LPCREATESTRUCT lpcs)
	{
		DWORD style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL;
    edit.Create(m_hWnd, CRect(10, 10, 400, 200), CustomEdit::GetWndClassName(), style, 0, IDC_CUSTOMEDIT);

		

		//edit.SubclassWindow(stat);
		return 0;
	}


	void OnPaint(HDC hDC)
	{

	}


  /**
   * ウィンドウのサイズが変更された時に呼び出される処理
   * @param UINT type 不明
   * @param CSize size 変更後のサイズ情報
   */
  void OnSize(UINT type, CSize size)
  {
    CRect rect;
    GetClientRect(&rect);


    edit.MoveWindow(rect);
  }


  void OnKeyDown(UINT keyCode, UINT repeatCount, UINT flags)
  {
    edit.OnKeyDown(keyCode, repeatCount, flags);
  }


  void OnChar(UINT character, UINT repeatCount, UINT flags)
  {
//    wchar_t msg[100];
//    wsprintf(msg, _T("%d"), m_hWnd);
//    MessageBox(msg, NULL, 0);

    edit.OnChar(character, repeatCount, flags);
  }

  void OnImeStartComposition()
  {
    edit.OnImeStartComposition();
  }

  BOOL OnImeComposition(DWORD dbcsChar, DWORD flags)
  {
    return edit.OnImeComposition(dbcsChar, flags);
  }

  void OnImeEndComposition()
  {
    edit.OnImeEndComposition();
  }


	void OnCopy()
	{
    edit.CopyText();

	}


	void OnCut()
	{
    edit.CutText();
	}


	void OnPaste()
	{
    edit.PasteText();
	}


  BOOL OnMouseWheel(UINT flags, short zDelta, CPoint point)
  {
    return edit.OnMouseWheel(flags, zDelta, point);
  }
};


};

#endif