#ifndef _CUSTOMEDIT_H_
#define _CUSTOMEDIT_H_

#include <stdio.h>
#include "Buffer.h"
#include "Point.h"
#include "Selection.h"
#include "CharacterBorderIterator.h"
#include "Longline.h"
#include "WtlHelper.h"

namespace EditTest {

using namespace Longline;


/**
 * 改行の扱い方の列挙。
 */
enum LineEndRules {
  LINE_END_AUTO = 0,
  LINE_END_CR,
  LINE_END_LF,
  LINE_END_CRLF,
  LINE_END_NONE,
  LINE_END_COUNT
};


/**
 * サンプルのエディットボックス
 * 
 */
class CustomEdit : public CWindowImpl<CustomEdit>
//	               public COwnerDraw<CustomEdit>
{
private:

  /**
   * バッファ
   */
  Buffer buffer;

  /**
   * 1行の高さ
   */
	DWORD lineHeight;

  /**
   * フォントサイズ
   */
  DWORD fontSize;

  /**
   * 垂直ルーラーの幅
   */
  DWORD rulerWidth;

  /**
   * 現在の縦方向のスクロール位置
   */
  DWORD vPosition;

  /**
   * 現在の横方向のスクロール位置
   */
  DWORD hPosition;

  /**
   * キャレットの表示場所
   */
  PhysicalPoint caretPosition;

  /**
   * 選択範囲
   */
  Selection* selection;

  /**
   * IMEの状態
   */
  BOOL imeStarted;


  /**
   * マウスがキャプチャーされているか
   */
  BOOL mouseCaptured;

  /**
   * マウスキャプチャ中に呼び出されるタイマーのID
   */
  UINT_PTR mouseCaptureTimer;


  /**
   * デフォルトのテキスト用のブラシ
   */
  CBrush* defaultBrush;

  
  /**
   * 選択中のテキストのブラシ
   */
  CBrush* selectedBrush;

public:

	
  /**
   * コンストラクタ
   */
	CustomEdit()
	{
/*
    buffer.SetText(_T("ABCあいうえおDEF\nかきくけこ  123456\nテストテスト    123漢字テストかきくけこここここ\nテストテストテスト")
      _T("ABCDEFG。\nHIJKLMN\nOPQRSTU\nVWXYZ\nテストテスト。あいうえお\nかきくけこ\nさしすせそ\nたちつてと\n")
						   _T("日本語。記号\nabcdfghjkdf\n,./;:]@["));
*/
    BYTE* fileData = new BYTE[500000];
    DWORD bufferLen = 0;
    HANDLE file = CreateFile(_T("test.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, 0);
    ReadFile(file, fileData, 500000, &bufferLen, 0);
    CloseHandle(file);

    FilterMbFilter* filter = new FilterMbFilter();
    filter->SetInputString(fileData, bufferLen);

    DWORD unicodeBufferLen = 0;
    LPWSTR unicodeBuffer = NULL;
    unicodeBuffer = (LPWSTR)filter->convert(NULL, &unicodeBufferLen, Encodings::UTF16_LE);

    buffer.SetText(unicodeBuffer);

		lineHeight = 20;
    fontSize = 14;
    rulerWidth = 40;

    vPosition = 0;
    hPosition = 0;

    caretPosition.SetX(0);
    caretPosition.SetY(0);

    imeStarted = false;

    mouseCaptured = false;
    mouseCaptureTimer = 100; //マウスのキャプチャー中に実行されるタイマーのID

    //描画用ブラシの作成
    defaultBrush = new CBrush();
    defaultBrush->CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
    selectedBrush = new CBrush();
    selectedBrush->CreateSolidBrush(RGB(0x44, 0x44, 0xAA));

    selection = new Selection();
    delete[] fileData;
    delete filter;
	}



public:


  /**
   * コンポーネント作成直後に実行される処理
   * @param LPCREATESTRUCT createStruct コンポーネントの情報を含んだ配列
   * 
   * コンストラクタの時点ではウィンドウが生成されず、m_hWnd等が
   * 初期化されていないため、この辺りの値を必要とする処理は
   * コンストラクタではなくこのメソッドに記述する。
   */
  LRESULT OnCreate(LPCREATESTRUCT createStruct)
  {
    if(m_hWnd == NULL) {
      ::MessageBox(NULL, _T("NULL"), NULL, 0);
    }

    UpdateScrollBars();
    return 0;
  }

  
  /**
   * コンポーネントのサイズが変更された時に呼び出される処理
   * @param UINT type 不明
   * @param CSize size 変更後のサイズ情報
   */
  void OnSize(UINT type, CSize size)
  {
    UpdateScrollBars();
  }


  /**
   * コンポーネント上でスクロールが発生した場合に呼び出される処理
   */
  void OnVScroll(UINT scrollCode, UINT position, CScrollBar scrollBar)
  {
    DWORD docLineCount = buffer.GetLineCount(),
          viewLineCount = GetViewLineCount();

    DWORD maxScroll = docLineCount;// - viewLineCount;

    switch(scrollCode)
    {
    case SB_LINEDOWN:
      vPosition = min(maxScroll, vPosition+1);
      break;
    case SB_LINEUP:
      vPosition = max(0, (int)vPosition-1);
      break;
    case SB_PAGEDOWN:
      vPosition = min((int)maxScroll, (int)vPosition+15);
      break;
    case SB_PAGEUP:
      vPosition = max(0, (int)vPosition-15);
      break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
      vPosition = position;
      vPosition = min((int)maxScroll, (int)vPosition);
      vPosition = max(0, (int)vPosition);
      break;
    }

    UpdateImeWindowPosition();
    SetScrollPos(SB_VERT, vPosition, TRUE);
    Invalidate();
  }


  /**
   * コンポーネント上でスクロールが発生した場合に呼び出される処理
   */
  void OnHScroll(UINT scrollCode, UINT position, CScrollBar scrollBar)
  {
    switch(scrollCode)
    {
    case SB_LINERIGHT:
      hPosition = min(1000, hPosition+1);
      break;
    case SB_LINELEFT:
      hPosition = max(0, (int)hPosition-1);
      break;
    case SB_PAGERIGHT:
      hPosition = min((int)1000, (int)hPosition+15);
      break;
    case SB_PAGELEFT:
      hPosition = max(0, (int)hPosition-15);
      break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
      hPosition = max(0, (int)position);
      break;
    }

    UpdateImeWindowPosition();
    SetScrollPos(SB_HORZ, hPosition, TRUE);
    Invalidate();
  }


  /**
   * キーダウンが発生した時に呼び出される処理
   * @param UINT charCode 押されたキーのコード
   * @param UINT repeatCount リピート数?
   * @param UINT flags ???
   */
  void OnKeyDown(UINT charCode, UINT repeatCount, UINT flags)
  {
    if(imeStarted) {
      return;
    }

    //改行
    if(charCode == 0x0D) {
      InsertLine();
      Invalidate();
    }

    if(charCode == VK_DELETE) {
      DeleteText(1);
      Invalidate();
    }

    if(charCode == VK_BACK)
    {
      DeleteText(-1);
      Invalidate();
    }

    MoveCaret(charCode);
  }


  /**
   * 文字をタイプした際に呼び出される処理。
   * @param UINT character キャラクタコード
   */
  void OnChar(UINT character, UINT repeatCount, UINT flags)
  {
/*
    wchar_t w[2];
    w[0]=(wchar_t)character;
    w[1]=0;
    wchar_t msg[100];
    wsprintf(msg, _T("%d"), w[0]);
    MessageBox(msg, NULL, 0);
*/
    if(imeStarted) {
      Invalidate();
      return;
    }
    
    switch((wchar_t)character)
    {
    case 0x08:  //Backspace
    case 0x0D:  //Return
      return;
    default:
      wchar_t text[2];
      text[0] = (wchar_t)character;
      text[1] = 0;
      InsertText(text);
    }
    Invalidate();
  }


  /**
   * IMEの入力を開始した時に呼び出される処理
   */
  void OnImeStartComposition()
  {
    UpdateImeWindowPosition();
    imeStarted = true;
  }


  /**
   * IMEの状態が変化した時に呼び出される
   * @param DWORD dbcsChar 入力文字？
   * @param DWORD flags 現在の状態を表すフラグ
   */
  BOOL OnImeComposition(DWORD dbcsChar, DWORD flags)
  {
    if(flags != 0 && (flags & GCS_RESULTSTR) == 0) {
      return false;
    }

    HIMC imc = ::ImmGetContext(m_hWnd);
    if(!imc) {
      return false;
    }

    int len = ::ImmGetCompositionStringW(imc, GCS_RESULTSTR, 0, 0) / sizeof(wchar_t);
    if(len <= 0) {
      return false;
    }
    
    wchar_t* insertion = new wchar_t[len+1];
    ::ImmGetCompositionStringW(imc, GCS_RESULTSTR, insertion, len * sizeof(wchar_t));
    insertion[len] = 0;

    InsertText(insertion);
    delete[] insertion;
    UpdateImeWindowPosition();

    ::ImmReleaseContext(m_hWnd, imc);
    SetMsgHandled(true);
    Invalidate();

    return true;
  }


  /**
   * IMEの入力が完了した時に呼び出される。
   */
  void OnImeEndComposition()
  {
    imeStarted = false;
  }


  /**
   * 左ボタンのクリックの処理
   * @param UINT nFlags 
   */
  void OnLButtonDown(UINT flags, CPoint point)
  {

    //マウスカーソルのキャプチャ開始
    ::SetCapture(m_hWnd);
    mouseCaptured = true;
    ::SetTimer(m_hWnd, mouseCaptureTimer, 100, NULL);

    PhysicalPoint pPoint;
    pPoint.SetX(point.x);
    pPoint.SetY(point.y);
    SetCaretPositionByPPoint(pPoint, flags & MK_SHIFT);

    Invalidate();
  }


  /**
   * マウスカーソルが移動した時の処理を行う
   * @param UINT flags Control、Shift等が押されているかの情報
   * @param CPoint point 座標情報
   */
  void OnMouseMove(UINT flags, CPoint point)
  {
    if(!mouseCaptured) {
      return;
    }
    PhysicalPoint pPoint;
    pPoint.SetX(point.x);
    pPoint.SetY(point.y);
    SetCaretPositionByPPoint(pPoint, true);
  }


  /**
   * マウスの左ボタンが離された時の処理
   * @param UINT flags Control、Shift等が押されているかの情報
   * @param CPoint point 座標情報
   */
  void OnLButtonUp(UINT flags, CPoint point)
  {
    ::ReleaseCapture();
    mouseCaptured = false;
    ::KillTimer(m_hWnd, mouseCaptureTimer);
  }


  /**
   * マウスのホイール入力に対する処理
   * @param UINT flags CONTROLやShiftが押されているかなどのフラグ
   * @param short zDelta ホイールをどれだけ回転させたか
   * @param CPoint point カーソル位置
   */
  BOOL OnMouseWheel(UINT flags, short zDelta, CPoint point)
  {
    int scrollLines = 0;
    if(!::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0)) {
      scrollLines = 5;
    }

    //wchar_t msg[100];
    //wsprintf(msg, _T("Delta:%d"), zDelta);
    //MessageBox(msg, NULL, 0);

    zDelta *= scrollLines;
    OnVScroll(SB_THUMBPOSITION, vPosition - (zDelta / WHEEL_DELTA), NULL);
    return false;
  }


  /**
   * タイマーを更新する時に呼び出される処理
   * @param UINT_PTR timerId タイマーID
   */
  void OnTimer(UINT_PTR timerId)
  {
    if(timerId == mouseCaptureTimer) {
      OnMouseCaptureTimer();
    }
  }


  /**
   * 自分自身を描画する時に呼び出される処理。
   * @param CHandleDC pdc 描画先デバイスコンテキスト。
   */
	void OnPaint(CDCHandle pdc)
	{
    PAINTSTRUCT ps;

    CDCHandle dc = CDCHandle(BeginPaint(&ps));
    CRect rcPaint = CRect(ps.rcPaint);
    if(rcPaint.IsRectEmpty()) {
      EndPaint(&ps);
      return;
    }

		CRect rect(10, 10, 100, 100);
		CBrush brush;
    CBrush frameBrush;
		brush.CreateSolidBrush(RGB(255,255,255));
    frameBrush.CreateSolidBrush(RGB(0,0,0));

    GetClientRect(&rect);
		dc.FillRect(rect, brush);

    DWORD lineCount = buffer.GetLineCount();
    DWORD viewLineCount = GetViewLineCount();
    for(DWORD i=vPosition, loopCount=0; i < lineCount && loopCount <= viewLineCount; i++, loopCount++) {
      DrawLine(dc, buffer.GetLine(i), loopCount * lineHeight, i);
    }

    GetClientRect(&rect);

    DrawCaret(&dc);

		//ルーラーを描画
    DrawRuler(&dc, rect.Height());


    dc.FrameRect(&rect, frameBrush);

    EndPaint(&ps);
	}


  /**
   * 現在選択中のテキストをコピーする。
   */
  void CopyText()
  {
    Clipboard clip(m_hWnd);
    
    LPWSTR text = GetSelectionText(LINE_END_CRLF);

    //::MessageBox(NULL, text, NULL, 0);

    if(text != NULL) {
      clip.Clear();
      clip.SetText(text);
      delete[] text;
    }
  }


  /**
   * 選択テキストをカットする。
   */
  void CutText()
  {
    Clipboard clip(m_hWnd);
    
    LPWSTR text = GetSelectionText(LINE_END_CRLF);

    if(text != NULL) {
      clip.Clear();
      clip.SetText(text);
      delete[] text;
    }

    DeleteSelection();
    Invalidate();
  }


  /**
   * クリップボードのテキストを貼り付ける。
   */
  void PasteText()
  {
    Clipboard clip(m_hWnd);
    
    LPWSTR text = clip.GetText();

    if(text != NULL) {
      Buffer pasteBuffer;
      pasteBuffer.SetText(text);
      int lineCount = pasteBuffer.GetLineCount();
      LPWSTR line = NULL;
      for(int i=0; i < lineCount; i++)
      {
        line = pasteBuffer.GetLine(i);
        InsertText(line);

        if(i < lineCount -1) {
          InsertLine();
        }
      }

      //改行のイテレータを作る必要がある？
      delete[] text;
    }

    Invalidate();
  }

  /**
   * 1行を描画する。
   * @param HDC hDc デバイスコンテキスト
   * @param LPWSTR line 描画しようとしている1行分の文字列
   * @param int y 描画先のY座標。
   * @param int lineY テキスト上のY座標。
   */
  void DrawLine(HDC hDc, LPWSTR line, int y, int lineY)
  {
    int length = wcslen(line);

		CDCHandle dc = hDc;

		int current = 0;
    DWORD next = 0;
    DWORD len = wcslen(line);
    DWORD stopper = 1000;
    DWORD drawLength = 0;

    //X座標はデフォルトでルーラーの隣からスタート
    int x=rulerWidth;

    CSize size;
    BOOL selected;
    RangeI* position = new RangeI();
    RangeI* selectionRange = new RangeI();
    LogicalPoint logicalPoint;
    CharacterBorderIterator itor = CharacterBorderIterator();
    int hScrollBorder = hPosition * fontSize;

    //TODO: イテレータの初期化時はRangeIではなく
    //Selectionを渡すように修正する。
    GetSelectionRangeOnLine(lineY, selectionRange);
    itor.Initialize(line, selectionRange);

    while(!itor.IsEnd() && stopper-- > 0) {
      itor.GetCurrent(position);

      current = position->GetFirst();
      logicalPoint.SetX(position->GetFirst());
      logicalPoint.SetY(lineY);
      if(selectionRange->GetRange() > 0 && current >= selectionRange->GetFirst() &&
         current < selectionRange->GetSecond()) {

        //選択領域内
        dc.SetBkMode(OPAQUE);
        dc.SetBkColor(RGB(0x44, 0x44, 0xAA));
        dc.SetTextColor(RGB(0xEE, 0xEE, 0xEE));
        dc.SelectBrush(*selectedBrush);
        selected = true;
      } else {
        //選択領域外
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(RGB(0x00, 0x00, 0x00));
        dc.SelectBrush(*defaultBrush);
        selected = false;
      }

      if(line[position->GetFirst()] == _T(' ')) {
        if( x >= hScrollBorder || (x+fontSize) >= hScrollBorder) {
          DrawSpace(&dc, x - hScrollBorder, y, selected);
        }
        x += fontSize;
      } else if(line[position->GetFirst()] == _T('\t')) {
        if( x >= hScrollBorder || (x+fontSize*4) >= hScrollBorder) {
          DrawTab(&dc, x - hScrollBorder, y, selected);
        }
        x += fontSize * 4;
      } else {
        dc.GetTextExtent(&line[position->GetFirst()], position->GetRange(), &size);
        if( x >= hScrollBorder || (x+size.cx) >= hScrollBorder) {
          dc.TextOut(x - hScrollBorder, y, &line[position->GetFirst()], position->GetRange());
        }
        x += size.cx;
      }
      

      itor.Next();
    }
    
    delete position;
    delete selectionRange;
	}


  /**
   * 1文字分のスペースを描画する
   * @param CDCHandle* dc デバイスコンテキスト
   * @param int x X座標
   * @param int y Y座標
   */
  void DrawSpace(CDCHandle* dc, int x, int y, BOOL selected)
  {
    COLORREF bgColor;
    if(selected) {
      bgColor = RGB(0x44, 0x44, 0xAA);
    } else {
      bgColor = RGB(0xFF, 0xFF, 0xFF);
    }
    dc->FillSolidRect(new CRect(x, y, x+fontSize, y+lineHeight-2), bgColor);
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0xCC, 0xCC, 0xCC));
    dc->SelectPen(pen);
    dc->Rectangle(new CRect(x, y + (lineHeight * 2 / 3), x+(fontSize / 2), y+lineHeight-2));
  }


  /**
   * 1文字分のタブを描画する
   * @param CDCHandle* dc デバイスコンテキスト
   * @param int x X座標
   * @param int y Y座標
   */
  void DrawTab(CDCHandle* dc, int x, int y, BOOL selected)
  {
    COLORREF bgColor;
    if(selected) {
      bgColor = RGB(0x44, 0x44, 0xAA);
    } else {
      bgColor = RGB(0xFF, 0xFF, 0xFF);
    }
    dc->FillSolidRect(new CRect(x, y, x+(fontSize * 4), y+lineHeight-2), bgColor);
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0xCC, 0xCC, 0xCC));
    dc->SelectPen(pen);
    dc->MoveTo(x+2, y + lineHeight / 4);
    dc->LineTo(x+10, y + lineHeight / 2);
    dc->MoveTo(x+10, y + lineHeight / 2);
    dc->LineTo(x+2, y + lineHeight * 2 / 3);
    dc->MoveTo(x+2, y + lineHeight * 2 / 3);
    dc->LineTo(x+2, y + lineHeight / 4);
  }


  /**
   * 垂直ルーラーを描画する
   * @param CDCHandle* dc 描画先のデバイスコンテキスト
   * @param int コンポーネントの高さ
   */
  void DrawRuler(CDCHandle* dc, int h)
  {
    DWORD maxLines = buffer.GetLineCount();

    CBrush brush;
    brush.CreateSolidBrush(RGB(228,228,228));

    //背景の描画
    dc->Rectangle(0, 0, rulerWidth, h);

    //1行ずつ行番号を出力
    wchar_t digit[10];
    for(DWORD i=0; i < maxLines; i++)
    {
      wsprintf(digit, _T("%04d"), vPosition+i+1);
      dc->TextOutW(0, i * lineHeight, digit);
    }
  }


  /**
   * キャレットを描画する。
   * @param CDCHandle* dc 描画先のデバイスコンテキスト
   * 
   */
  void DrawCaret(CDCHandle* dc)
  {
    int x=0, y=0;
    int hScrollBorder = hPosition * fontSize;

    y = caretPosition.GetY();
    x = caretPosition.GetX() - hScrollBorder;

    CRect rect;
    rect.left   = rulerWidth + x;
    rect.bottom = y+lineHeight - (vPosition * lineHeight);
    rect.right  = rect.left + fontSize - 5;
    rect.top    = rect.bottom - 5;

    CBrush brush;
    brush.CreateSolidBrush(RGB(200,200,240));

    dc->FillRect(rect, brush);
  }


  /**
   * コンポーネント内に何行表示できるかを返す。
   * @return DWORD 表示できる行数
   * 
   * 完全に表示できる行数のみを返します。
   * 2.5行等、中途半端な行数の場合は2を返します。
   */
  DWORD GetViewLineCount()
  {
    CRect rect;
    GetClientRect(&rect);

    return rect.Height() / lineHeight;
  }


  /**
   * 指定した行の選択領域のサイズを返す
   * @param int line 行番号
   * @param RangeI* out 選択領域の幅を格納する変数
   */
  void GetSelectionRangeOnLine(int line, RangeI* out)
  {
    LogicalPoint beginPoint, endPoint;
    selection->GetBeginPoint(&beginPoint);
    selection->GetEndPoint(&endPoint);

    if(line < beginPoint.GetY() || line > endPoint.GetY()) {
      out->SetFirst(0);
      out->SetSecond(0);
      return;
    }

    CClientDC* dc = new CClientDC(m_hWnd);

    //選択領域の左上,右下座標を一度実座標に変換する。
    //その後、現在の行の何文字目なのかを調べる
    int physicalLeft = 0, physicalRight = 0;
    int curLeft = 0, curRight = 0;
    LogicalPoint logical;
    PhysicalPoint physical;
    logical.SetX(beginPoint.GetX());
    logical.SetY(beginPoint.GetY());
    GetPPointFromLPoint(*dc, &logical, &physical);
    physicalLeft = physical.GetX();


    logical.SetX(endPoint.GetX());
    logical.SetY(endPoint.GetY());
    GetPPointFromLPoint(*dc, &logical, &physical);
    physicalRight = physical.GetX();

    //上で調べた物理座標のLeft, Rightが現在の行の
    //何文字目なのかを調べる。
    physical.SetX(physicalLeft);
    physical.SetY(line * lineHeight);
    GetLPointFromPPoint(*dc, &physical, &logical);
    curLeft = logical.GetX();

    physical.SetX(physicalRight);
    physical.SetY(line * lineHeight);
    GetLPointFromPPoint(*dc, &physical, &logical);
    curRight = logical.GetX();

    LPWSTR lineText = buffer.GetLine(line);
    out->SetFirst((line == beginPoint.GetY()) ? curLeft : 0);
    out->SetSecond((line == endPoint.GetY()) ? curRight : wcslen(lineText));
    delete dc;
  }


  /**
   * 現在選択中のテキストを返す。
   * @param LineEndRules lineEndRule 改行の扱い方。(EditTest::LineEndRulesの値)
   * @return LPWSTR 選択中の文字列を含んだバッファのポインタ。
   * - この関数で取得したバッファは呼び出し側で解放する必要がある。
   */
  LPWSTR GetSelectionText(int lienEndRule)
  {
    if(selection->IsEmpty()) {
      return NULL;
    }

    LogicalPoint start, end;
    selection->GetBeginPoint(&start);
    selection->GetEndPoint(&end);

    LPWSTR line = NULL;
    LPWSTR copyBuffer = NULL;
    DWORD buffLen = 0;
    if(start.GetY() == end.GetY()) {
      line = buffer.GetLine(start.GetY());
      buffLen = end.GetX() - start.GetX();
      copyBuffer = new wchar_t[buffLen + 1];
      wcsncpy(copyBuffer, &line[start.GetX()], buffLen);
      copyBuffer[buffLen] = 0;
    } else {
      //複数行の場合はまず全体の長さを予測する。
      buffLen = 0;
      int cur = 0, lineLen = 0, startPos=0;
      for(int i = start.GetY(); i <= end.GetY(); i++)
      {
        line = buffer.GetLine(i);
        lineLen = wcslen(line);

        startPos = 0;
        if(i == start.GetY()) {
          startPos = start.GetX();
          lineLen -= startPos;
        } else if(i == end.GetY()) {
          lineLen = end.GetX();
        }
        buffLen += lineLen;

        if(i < end.GetY()) {
          buffLen += wcslen(_T("\r\n"));
        }
      }
      copyBuffer = new wchar_t[buffLen + 1];
      for(int i = start.GetY(); i <= end.GetY(); i++)
      {
        line = buffer.GetLine(i);
        lineLen = wcslen(line);
        startPos = 0;
        if(i == start.GetY()) {
          startPos = start.GetX();
          lineLen -= startPos;
        } else if(i == end.GetY()) {
          lineLen = end.GetX();
        }

        wcsncpy(&copyBuffer[cur], &line[startPos], lineLen);
        cur += lineLen;

        if(i < end.GetY()) {
          line = _T("\r\n");
          lineLen = wcslen(line);
          wcsncpy(&copyBuffer[cur], line, lineLen);
          cur += lineLen;
        }
      }
      copyBuffer[buffLen] = 0;
    }

    return copyBuffer;
  }


  /**
   * タイマーで呼び出される、マウスキャプチャー中の処理
   */
  void OnMouseCaptureTimer()
  {
    //AutoFixScrollPosition();
    CRect rect;
    GetClientRect(rect);

    POINT mousePoint;
    ::GetCursorPos(&mousePoint);
    ScreenToClient(&mousePoint);

    if(mousePoint.y <= 0) {
      OnVScroll(SB_THUMBPOSITION, vPosition - 3, NULL);
    }
    if(mousePoint.y >= rect.bottom) {
      OnVScroll(SB_THUMBPOSITION, vPosition + 3, NULL);
    }
    if(mousePoint.x <= rect.left) {
      OnHScroll(SB_THUMBPOSITION, hPosition - 3, NULL);
    }
    if(mousePoint.x >= rect.right) {
      OnHScroll(SB_THUMBPOSITION, hPosition + 3, NULL);
    }

    PhysicalPoint pPoint;
    pPoint.SetX(mousePoint.x);
    pPoint.SetY(mousePoint.y);
    SetCaretPositionByPPoint(pPoint, true);

    Invalidate();
  }

private:


  /**
   * キャレットの移動を行う。
   * @param keyCode キーコード
   */
  void MoveCaret(UINT keyCode)
  {
    LogicalPoint lPosition, oldPosition;
    CClientDC *dc = new CClientDC(m_hWnd);
    int len = 0;
    //移動前の座標を記憶しておく
    buffer.GetCaretPosition(&oldPosition);

    //キーに応じてキャレットを移動
    switch(keyCode)
    {
    case VK_LEFT:
      buffer.GetCaretPosition(&lPosition);
      lPosition.SetX(max(0, (int)lPosition.GetX() - 1));
      GetPPointFromLPoint(dc->m_hDC, &lPosition, &caretPosition);
      buffer.SetCaretPosition(&lPosition);
      break;

    case VK_RIGHT:
      buffer.GetCaretPosition(&lPosition);
      lPosition.SetX(min(buffer.GetLineLength(lPosition.GetY()), lPosition.GetX() + 1));
      GetPPointFromLPoint(dc->m_hDC, &lPosition, &caretPosition);
      buffer.SetCaretPosition(&lPosition);
      break;

    case VK_UP:
      caretPosition.SetY(max(1, (int)(caretPosition.GetY() - lineHeight)));
      GetLPointFromPPoint(dc->m_hDC, &caretPosition, &lPosition);
      buffer.SetCaretPosition(&lPosition);
      break;

    case VK_DOWN:
      caretPosition.SetY(min((buffer.GetLineCount()-1) * lineHeight, caretPosition.GetY() + lineHeight));
      GetLPointFromPPoint(dc->m_hDC, &caretPosition, &lPosition);
      buffer.SetCaretPosition(&lPosition);
      break;
    default:
      delete dc;
      return;
    }

    //Shiftキーに応じて選択範囲を更新
    if(::GetAsyncKeyState(VK_SHIFT) < 0) {
      if(selection->IsEmpty()) {
        selection->SetAnchorPoint(&oldPosition);
      }
      selection->SetActivePoint(&lPosition);
    } else {
      selection->SetActivePoint(&lPosition);
      selection->SetAnchorPoint(&lPosition);
    }

    delete dc;
    Invalidate();

    AutoFixScrollPosition();
  }


  /**
   * 指定したPhysicalPointの座標にキャレットを移動させる。
   * @param PhysicalPoint point
   * @param BOOL extend 選択領域を拡大するか
   */
  void SetCaretPositionByPPoint(PhysicalPoint point, BOOL extend)
  {
    PhysicalPoint pPoint;
    pPoint.SetX(point.GetX() - rulerWidth + (hPosition * fontSize));
    pPoint.SetY(point.GetY() + (vPosition * lineHeight));

    CDCHandle dc = GetDC();
    LogicalPoint activePoint, anchorPoint;
    if(extend) {
      //領域選択
      if(selection->IsEmpty()) {
        GetLPointFromPPoint(dc, &caretPosition, &anchorPoint);
        selection->SetAnchorPoint(&anchorPoint);
      }
      GetLPointFromPPoint(dc, &pPoint, &activePoint);
      selection->SetActivePoint(&activePoint);

    } else {
      //カーソル移動
      GetLPointFromPPoint(dc, &pPoint, &activePoint);
      selection->SetAnchorPoint(&activePoint);
      selection->SetActivePoint(&activePoint);
    }
    buffer.SetCaretPosition(&activePoint);

    GetPPointFromLPoint(dc, &activePoint, &pPoint);
    caretPosition.SetX(pPoint.GetX());
    caretPosition.SetY(pPoint.GetY());
  }


  /**
   * スクロールバーを更新する。
   */
  void UpdateScrollBars()
  {
    DWORD docLineCount = buffer.GetLineCount(),
          viewLineCount = GetViewLineCount();

    int maxScroll = docLineCount;//-viewLineCount;

    SetScrollRange(SB_VERT, 0, maxScroll, true);
    SetScrollRange(SB_HORZ, 0, 1000, true);

    if(maxScroll <= 0) {
      EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
      vPosition = 0;
      return;
    } else {
      EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
    }
    EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
  }


  /**
   * 論理座標(文字数単位)から物理座標(ピクセル単位)の座標を計算して返す
   * @param HDC dc 描画先デバイスコンテキスト
   * @param logical 論理座標。
   * @param out     結果を格納するPhysicalPointクラス。
   */
  inline void GetPPointFromLPoint(HDC dc, LogicalPoint* logical, PhysicalPoint* out)
  {
    CDCHandle* dcHandle = new CDCHandle(dc);

    LPWSTR line = buffer.GetLine(logical->GetY());

    int x=0, lineLength = wcslen(line);
    CSize size;
    for(int i=0; i < logical->GetX() && i < lineLength; i++)
    {
      if(line[i] == _T(' ')) {
        x += (int)fontSize;
      } else if(line[i] == _T('\t')) {
        x += (int)fontSize * 4;
      } else {
        dcHandle->GetTextExtent(&line[i], 1, &size);
        x += size.cx;
      }
    }

    out->SetX(x);
    out->SetY(logical->GetY() * lineHeight);
    delete dcHandle;
  }


  /**
   * 物理座標(ピクセル単位)から論理座標(文字数単位)の座標を計算して返す
   * @param HDC dc 描画先デバイスコンテキスト
   * @param physical 物理座標。
   * @param out     結果を格納するlogicalPointクラス。
   */
  inline void GetLPointFromPPoint(HDC dc, PhysicalPoint* physical, LogicalPoint* out)
  {
    CDCHandle* dcHandle = new CDCHandle(dc);
    int y = max(0, (int)physical->GetY() / (int)lineHeight);
    
    LPWSTR line = buffer.GetLine(y);

    int i=0, x=0, len=wcslen(line), inc=0, inc2=0;
    CSize size;
    for(i=0; i < len; i++, x += inc)
    {
      if(line[i] == _T(' ')) {
        inc = (int)fontSize;
      } else if(line[i] == _T('\t')) {
        inc = (int)fontSize * 4;
      } else {
        dcHandle->GetTextExtent(&line[i], 1, &size);
        inc = size.cx;
      }

      if(x+inc > physical->GetX()) {
        break;
      }
    }

    if(i+1 < len) {
      if(line[i] == _T(' ')) {
        inc2 = (int)fontSize;
      } else if(line[i] == _T('\t')) {
        inc2 = (int)fontSize * 4;
      } else {
        inc2 = dcHandle->GetTextExtent(&line[i+1], 1, &size);
        inc2 = size.cx;
      }

      if(abs(x - physical->GetX()) > abs(x + inc2 - physical->GetX())) {
        x += inc;
        i++;
      }
    }


    physical->SetX(x);
    out->SetX(i);
    out->SetY(y);
    delete dcHandle;
  }


  /**
   * 文字を挿入する。
   * @param LPWSTR text 文字列
   * - 改行や削除、バックスペース等も文字として挿入します。
   */
  void InsertText(LPWSTR text)
  {
    if(!selection->IsEmpty()) {
      DeleteSelection();
    }

    CDCHandle dc = GetDC();
    LogicalPoint lPoint;
    //LPWSTR w = new wchar_t[2];
    //w[0] = (wchar_t)character;
    //w[1] = _T('\0');
    buffer.GetCaretPosition(&lPoint);

    buffer.InsertText(lPoint.GetY(), lPoint.GetX(), text);

    //通常の文字の場合
    lPoint.SetX(lPoint.GetX()+wcslen(text));
    buffer.SetCaretPosition(&lPoint);
    GetPPointFromLPoint(dc, &lPoint, &caretPosition);
  }


  /**
   * 改行を挿入する
   */
  void InsertLine()
  {
    if(!selection->IsEmpty()) {
      DeleteSelection();
    }
    LogicalPoint lPoint;
    buffer.GetCaretPosition(&lPoint);
    buffer.InsertLine(lPoint.GetY(), lPoint.GetX());

    lPoint.SetX(0);
    lPoint.SetY(lPoint.GetY()+1);
    buffer.SetCaretPosition(&lPoint);

    CDCHandle dc = GetDC();
    GetPPointFromLPoint(dc, &lPoint, &caretPosition);
  }


  /**
   * 文字を削除する
   */
  void DeleteText(int dim)
  {
    int start = 0,
        length = 0;

    LogicalPoint lPoint;
    buffer.GetCaretPosition(&lPoint);

    if(!selection->IsEmpty()) {
      DeleteSelection();
      return;
    }
    else if(dim >= 0) {
      start = lPoint.GetX();
      length = 1;

      int lineEnd = buffer.GetLineLength(lPoint.GetY());
      if(lPoint.GetX() == lineEnd) {
        buffer.ConcatLine(lPoint.GetY());
      } else {
        buffer.DeleteText(lPoint.GetY(), start, length);
      }
    } else {
      start = lPoint.GetX() - 1;
      length = 1;
      

      if(lPoint.GetX() == 0 && lPoint.GetY() > 0) {
        lPoint.SetY(lPoint.GetY() - 1);
        lPoint.SetX(buffer.GetLineLength(lPoint.GetY()));
        buffer.ConcatLine(lPoint.GetY());
      } else if(buffer.GetLineLength(lPoint.GetY()) > 0) {
        lPoint.SetX(start);
        buffer.DeleteText(lPoint.GetY(), start, length);
      }
    }

    buffer.SetCaretPosition(&lPoint);

    CDCHandle dc = GetDC();
    GetPPointFromLPoint(dc, &lPoint, &caretPosition);
  }


  /**
   * 選択中のテキストを削除する。
   */
  void DeleteSelection()
  {
    LogicalPoint startPoint;
    LogicalPoint endPoint;
    selection->GetBeginPoint(&startPoint);
    selection->GetEndPoint(&endPoint);

    //完全に削除してしまう行の数
    for(int i = startPoint.GetY() + 1; i < endPoint.GetY(); i++)
    {
      buffer.DeleteLine(startPoint.GetY() + 1);
    }

    if(startPoint.GetY() != endPoint.GetY()) {
      //終了点から手前は全て削除
      if(endPoint.GetX() > 0) {
        buffer.DeleteText(startPoint.GetY()+1, 0, endPoint.GetX());
      }

      //開始点から後ろは全て削除
      int lineLength = buffer.GetLineLength(startPoint.GetY());
      if(startPoint.GetX() < lineLength) {
        buffer.DeleteText(startPoint.GetY(), startPoint.GetX(), lineLength - startPoint.GetX());
      }
      buffer.ConcatLine(startPoint.GetY());
    } else {

      buffer.DeleteText(endPoint.GetY(), startPoint.GetX(), endPoint.GetX() - startPoint.GetX());
    }

    
    selection->SetActivePoint(&startPoint);
    selection->SetAnchorPoint(&startPoint);

    CDCHandle dc = GetDC();
    buffer.SetCaretPosition(&startPoint);
    GetPPointFromLPoint(dc, &startPoint, &caretPosition);

  }


  /**
   * キャレットの位置にスクロールを移動させる
   * 
   */
  void AutoFixScrollPosition()
  {
    //完全に表示されている行のみを対象にするため-1する。
    int viewLineCount = GetViewLineCount() - 1;

    LogicalPoint lPoint;
    buffer.GetCaretPosition(&lPoint);
    if(lPoint.GetY() > vPosition + viewLineCount) {
      OnVScroll(SB_THUMBPOSITION, lPoint.GetY() - viewLineCount, NULL);
    }

    if(lPoint.GetY() < vPosition)
    {
      OnVScroll(SB_THUMBPOSITION, lPoint.GetY(), NULL);
    }

    CRect rcClient;
    GetClientRect(&rcClient);
    rcClient.left += rulerWidth;
    rcClient.right -= 18;
    if(caretPosition.GetX() > (hPosition-1) * fontSize + rcClient.Width()) {
      //一気に1/3画面分スクロール
      OnHScroll(SB_THUMBPOSITION, (caretPosition.GetX() - (rcClient.Width() / 3)) / fontSize+1, NULL);
    }

    if(caretPosition.GetX() < (hPosition) * fontSize)
    {
      OnHScroll(SB_THUMBPOSITION, caretPosition.GetX() / fontSize, NULL);
    }
  }


  /**
   * IMEの入力文字をキャレットの位置に移動させる。
   */
  void UpdateImeWindowPosition()
  {
	  COMPOSITIONFORM cf;
	  HIMC imc = ::ImmGetContext(m_hWnd);

    if(imc) {
		  cf.dwStyle = CFS_POINT;
		  cf.ptCurrentPos.x = caretPosition.GetX() + rulerWidth - (hPosition * fontSize);
      cf.ptCurrentPos.y = caretPosition.GetY() - (vPosition * lineHeight);
	  	//cf.ptCurrentPos.x -= 1;
		  //cf.ptCurrentPos.y -= 1;
		  ::ImmSetCompositionWindow(imc, &cf);
		  ::ImmReleaseContext(m_hWnd, imc);
    }

  }


public:
	DECLARE_WND_CLASS(_T("CustomEdit"));

	BEGIN_MSG_MAP_EX(CustomEdit);

    MSG_WM_CREATE(OnCreate);
    MSG_WM_SIZE(OnSize);
    MSG_WM_VSCROLL(OnVScroll);
    MSG_WM_HSCROLL(OnHScroll);
    //MSG_WM_KEYDOWN(OnKeyDown);
    MSG_WM_PAINT(OnPaint);
    MSG_WM_IME_STARTCOMPOSITION(OnImeStartComposition);
    MSG_WM_IME_COMPOSITION(OnImeComposition);
    MSG_WM_IME_ENDCOMPOSITION(OnImeEndComposition);
    MSG_WM_LBUTTONDOWN(OnLButtonDown);
    MSG_WM_MOUSEMOVE(OnMouseMove);
    MSG_WM_LBUTTONUP(OnLButtonUp);

    MSG_WM_MOUSEWHEEL(OnMouseWheel);

    MSG_WM_TIMER(OnTimer);

		//CHAIN_MSG_MAP_ALT(COwnerDraw<CustomEdit>, 1);
		DEFAULT_REFLECTION_HANDLER();
	END_MSG_MAP();


  /**
   * デストラクタ
   */
	~CustomEdit()
	{
    delete defaultBrush;
    delete selectedBrush;
    delete selection;
	}

};

};

#endif