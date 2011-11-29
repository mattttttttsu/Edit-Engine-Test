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
 * ���s�̈������̗񋓁B
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
 * �T���v���̃G�f�B�b�g�{�b�N�X
 * 
 */
class CustomEdit : public CWindowImpl<CustomEdit>
//	               public COwnerDraw<CustomEdit>
{
private:

  /**
   * �o�b�t�@
   */
  Buffer buffer;

  /**
   * 1�s�̍���
   */
	DWORD lineHeight;

  /**
   * �t�H���g�T�C�Y
   */
  DWORD fontSize;

  /**
   * �������[���[�̕�
   */
  DWORD rulerWidth;

  /**
   * ���݂̏c�����̃X�N���[���ʒu
   */
  DWORD vPosition;

  /**
   * ���݂̉������̃X�N���[���ʒu
   */
  DWORD hPosition;

  /**
   * �L�����b�g�̕\���ꏊ
   */
  PhysicalPoint caretPosition;

  /**
   * �I��͈�
   */
  Selection* selection;

  /**
   * IME�̏��
   */
  BOOL imeStarted;


  /**
   * �}�E�X���L���v�`���[����Ă��邩
   */
  BOOL mouseCaptured;

  /**
   * �}�E�X�L���v�`�����ɌĂяo�����^�C�}�[��ID
   */
  UINT_PTR mouseCaptureTimer;


  /**
   * �f�t�H���g�̃e�L�X�g�p�̃u���V
   */
  CBrush* defaultBrush;

  
  /**
   * �I�𒆂̃e�L�X�g�̃u���V
   */
  CBrush* selectedBrush;

public:

	
  /**
   * �R���X�g���N�^
   */
	CustomEdit()
	{
/*
    buffer.SetText(_T("ABC����������DEF\n����������  123456\n�e�X�g�e�X�g    123�����e�X�g������������������\n�e�X�g�e�X�g�e�X�g")
      _T("ABCDEFG�B\nHIJKLMN\nOPQRSTU\nVWXYZ\n�e�X�g�e�X�g�B����������\n����������\n����������\n�����Ă�\n")
						   _T("���{��B�L��\nabcdfghjkdf\n,./;:]@["));
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
    mouseCaptureTimer = 100; //�}�E�X�̃L���v�`���[���Ɏ��s�����^�C�}�[��ID

    //�`��p�u���V�̍쐬
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
   * �R���|�[�l���g�쐬����Ɏ��s����鏈��
   * @param LPCREATESTRUCT createStruct �R���|�[�l���g�̏����܂񂾔z��
   * 
   * �R���X�g���N�^�̎��_�ł̓E�B���h�E���������ꂸ�Am_hWnd����
   * ����������Ă��Ȃ����߁A���̕ӂ�̒l��K�v�Ƃ��鏈����
   * �R���X�g���N�^�ł͂Ȃ����̃��\�b�h�ɋL�q����B
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
   * �R���|�[�l���g�̃T�C�Y���ύX���ꂽ���ɌĂяo����鏈��
   * @param UINT type �s��
   * @param CSize size �ύX��̃T�C�Y���
   */
  void OnSize(UINT type, CSize size)
  {
    UpdateScrollBars();
  }


  /**
   * �R���|�[�l���g��ŃX�N���[�������������ꍇ�ɌĂяo����鏈��
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
   * �R���|�[�l���g��ŃX�N���[�������������ꍇ�ɌĂяo����鏈��
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
   * �L�[�_�E���������������ɌĂяo����鏈��
   * @param UINT charCode �����ꂽ�L�[�̃R�[�h
   * @param UINT repeatCount ���s�[�g��?
   * @param UINT flags ???
   */
  void OnKeyDown(UINT charCode, UINT repeatCount, UINT flags)
  {
    if(imeStarted) {
      return;
    }

    //���s
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
   * �������^�C�v�����ۂɌĂяo����鏈���B
   * @param UINT character �L�����N�^�R�[�h
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
   * IME�̓��͂��J�n�������ɌĂяo����鏈��
   */
  void OnImeStartComposition()
  {
    UpdateImeWindowPosition();
    imeStarted = true;
  }


  /**
   * IME�̏�Ԃ��ω��������ɌĂяo�����
   * @param DWORD dbcsChar ���͕����H
   * @param DWORD flags ���݂̏�Ԃ�\���t���O
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
   * IME�̓��͂������������ɌĂяo�����B
   */
  void OnImeEndComposition()
  {
    imeStarted = false;
  }


  /**
   * ���{�^���̃N���b�N�̏���
   * @param UINT nFlags 
   */
  void OnLButtonDown(UINT flags, CPoint point)
  {

    //�}�E�X�J�[�\���̃L���v�`���J�n
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
   * �}�E�X�J�[�\�����ړ��������̏������s��
   * @param UINT flags Control�AShift����������Ă��邩�̏��
   * @param CPoint point ���W���
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
   * �}�E�X�̍��{�^���������ꂽ���̏���
   * @param UINT flags Control�AShift����������Ă��邩�̏��
   * @param CPoint point ���W���
   */
  void OnLButtonUp(UINT flags, CPoint point)
  {
    ::ReleaseCapture();
    mouseCaptured = false;
    ::KillTimer(m_hWnd, mouseCaptureTimer);
  }


  /**
   * �}�E�X�̃z�C�[�����͂ɑ΂��鏈��
   * @param UINT flags CONTROL��Shift��������Ă��邩�Ȃǂ̃t���O
   * @param short zDelta �z�C�[�����ǂꂾ����]��������
   * @param CPoint point �J�[�\���ʒu
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
   * �^�C�}�[���X�V���鎞�ɌĂяo����鏈��
   * @param UINT_PTR timerId �^�C�}�[ID
   */
  void OnTimer(UINT_PTR timerId)
  {
    if(timerId == mouseCaptureTimer) {
      OnMouseCaptureTimer();
    }
  }


  /**
   * �������g��`�悷�鎞�ɌĂяo����鏈���B
   * @param CHandleDC pdc �`���f�o�C�X�R���e�L�X�g�B
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

		//���[���[��`��
    DrawRuler(&dc, rect.Height());


    dc.FrameRect(&rect, frameBrush);

    EndPaint(&ps);
	}


  /**
   * ���ݑI�𒆂̃e�L�X�g���R�s�[����B
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
   * �I���e�L�X�g���J�b�g����B
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
   * �N���b�v�{�[�h�̃e�L�X�g��\��t����B
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

      //���s�̃C�e���[�^�����K�v������H
      delete[] text;
    }

    Invalidate();
  }

  /**
   * 1�s��`�悷��B
   * @param HDC hDc �f�o�C�X�R���e�L�X�g
   * @param LPWSTR line �`�悵�悤�Ƃ��Ă���1�s���̕�����
   * @param int y �`����Y���W�B
   * @param int lineY �e�L�X�g���Y���W�B
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

    //X���W�̓f�t�H���g�Ń��[���[�ׂ̗���X�^�[�g
    int x=rulerWidth;

    CSize size;
    BOOL selected;
    RangeI* position = new RangeI();
    RangeI* selectionRange = new RangeI();
    LogicalPoint logicalPoint;
    CharacterBorderIterator itor = CharacterBorderIterator();
    int hScrollBorder = hPosition * fontSize;

    //TODO: �C�e���[�^�̏���������RangeI�ł͂Ȃ�
    //Selection��n���悤�ɏC������B
    GetSelectionRangeOnLine(lineY, selectionRange);
    itor.Initialize(line, selectionRange);

    while(!itor.IsEnd() && stopper-- > 0) {
      itor.GetCurrent(position);

      current = position->GetFirst();
      logicalPoint.SetX(position->GetFirst());
      logicalPoint.SetY(lineY);
      if(selectionRange->GetRange() > 0 && current >= selectionRange->GetFirst() &&
         current < selectionRange->GetSecond()) {

        //�I��̈��
        dc.SetBkMode(OPAQUE);
        dc.SetBkColor(RGB(0x44, 0x44, 0xAA));
        dc.SetTextColor(RGB(0xEE, 0xEE, 0xEE));
        dc.SelectBrush(*selectedBrush);
        selected = true;
      } else {
        //�I��̈�O
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
   * 1�������̃X�y�[�X��`�悷��
   * @param CDCHandle* dc �f�o�C�X�R���e�L�X�g
   * @param int x X���W
   * @param int y Y���W
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
   * 1�������̃^�u��`�悷��
   * @param CDCHandle* dc �f�o�C�X�R���e�L�X�g
   * @param int x X���W
   * @param int y Y���W
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
   * �������[���[��`�悷��
   * @param CDCHandle* dc �`���̃f�o�C�X�R���e�L�X�g
   * @param int �R���|�[�l���g�̍���
   */
  void DrawRuler(CDCHandle* dc, int h)
  {
    DWORD maxLines = buffer.GetLineCount();

    CBrush brush;
    brush.CreateSolidBrush(RGB(228,228,228));

    //�w�i�̕`��
    dc->Rectangle(0, 0, rulerWidth, h);

    //1�s���s�ԍ����o��
    wchar_t digit[10];
    for(DWORD i=0; i < maxLines; i++)
    {
      wsprintf(digit, _T("%04d"), vPosition+i+1);
      dc->TextOutW(0, i * lineHeight, digit);
    }
  }


  /**
   * �L�����b�g��`�悷��B
   * @param CDCHandle* dc �`���̃f�o�C�X�R���e�L�X�g
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
   * �R���|�[�l���g���ɉ��s�\���ł��邩��Ԃ��B
   * @return DWORD �\���ł���s��
   * 
   * ���S�ɕ\���ł���s���݂̂�Ԃ��܂��B
   * 2.5�s���A���r���[�ȍs���̏ꍇ��2��Ԃ��܂��B
   */
  DWORD GetViewLineCount()
  {
    CRect rect;
    GetClientRect(&rect);

    return rect.Height() / lineHeight;
  }


  /**
   * �w�肵���s�̑I��̈�̃T�C�Y��Ԃ�
   * @param int line �s�ԍ�
   * @param RangeI* out �I��̈�̕����i�[����ϐ�
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

    //�I��̈�̍���,�E�����W����x�����W�ɕϊ�����B
    //���̌�A���݂̍s�̉������ڂȂ̂��𒲂ׂ�
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

    //��Œ��ׂ��������W��Left, Right�����݂̍s��
    //�������ڂȂ̂��𒲂ׂ�B
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
   * ���ݑI�𒆂̃e�L�X�g��Ԃ��B
   * @param LineEndRules lineEndRule ���s�̈������B(EditTest::LineEndRules�̒l)
   * @return LPWSTR �I�𒆂̕�������܂񂾃o�b�t�@�̃|�C���^�B
   * - ���̊֐��Ŏ擾�����o�b�t�@�͌Ăяo�����ŉ������K�v������B
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
      //�����s�̏ꍇ�͂܂��S�̂̒�����\������B
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
   * �^�C�}�[�ŌĂяo�����A�}�E�X�L���v�`���[���̏���
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
   * �L�����b�g�̈ړ����s���B
   * @param keyCode �L�[�R�[�h
   */
  void MoveCaret(UINT keyCode)
  {
    LogicalPoint lPosition, oldPosition;
    CClientDC *dc = new CClientDC(m_hWnd);
    int len = 0;
    //�ړ��O�̍��W���L�����Ă���
    buffer.GetCaretPosition(&oldPosition);

    //�L�[�ɉ����ăL�����b�g���ړ�
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

    //Shift�L�[�ɉ����đI��͈͂��X�V
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
   * �w�肵��PhysicalPoint�̍��W�ɃL�����b�g���ړ�������B
   * @param PhysicalPoint point
   * @param BOOL extend �I��̈���g�傷�邩
   */
  void SetCaretPositionByPPoint(PhysicalPoint point, BOOL extend)
  {
    PhysicalPoint pPoint;
    pPoint.SetX(point.GetX() - rulerWidth + (hPosition * fontSize));
    pPoint.SetY(point.GetY() + (vPosition * lineHeight));

    CDCHandle dc = GetDC();
    LogicalPoint activePoint, anchorPoint;
    if(extend) {
      //�̈�I��
      if(selection->IsEmpty()) {
        GetLPointFromPPoint(dc, &caretPosition, &anchorPoint);
        selection->SetAnchorPoint(&anchorPoint);
      }
      GetLPointFromPPoint(dc, &pPoint, &activePoint);
      selection->SetActivePoint(&activePoint);

    } else {
      //�J�[�\���ړ�
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
   * �X�N���[���o�[���X�V����B
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
   * �_�����W(�������P��)���畨�����W(�s�N�Z���P��)�̍��W���v�Z���ĕԂ�
   * @param HDC dc �`���f�o�C�X�R���e�L�X�g
   * @param logical �_�����W�B
   * @param out     ���ʂ��i�[����PhysicalPoint�N���X�B
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
   * �������W(�s�N�Z���P��)����_�����W(�������P��)�̍��W���v�Z���ĕԂ�
   * @param HDC dc �`���f�o�C�X�R���e�L�X�g
   * @param physical �������W�B
   * @param out     ���ʂ��i�[����logicalPoint�N���X�B
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
   * ������}������B
   * @param LPWSTR text ������
   * - ���s��폜�A�o�b�N�X�y�[�X���������Ƃ��đ}�����܂��B
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

    //�ʏ�̕����̏ꍇ
    lPoint.SetX(lPoint.GetX()+wcslen(text));
    buffer.SetCaretPosition(&lPoint);
    GetPPointFromLPoint(dc, &lPoint, &caretPosition);
  }


  /**
   * ���s��}������
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
   * �������폜����
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
   * �I�𒆂̃e�L�X�g���폜����B
   */
  void DeleteSelection()
  {
    LogicalPoint startPoint;
    LogicalPoint endPoint;
    selection->GetBeginPoint(&startPoint);
    selection->GetEndPoint(&endPoint);

    //���S�ɍ폜���Ă��܂��s�̐�
    for(int i = startPoint.GetY() + 1; i < endPoint.GetY(); i++)
    {
      buffer.DeleteLine(startPoint.GetY() + 1);
    }

    if(startPoint.GetY() != endPoint.GetY()) {
      //�I���_�����O�͑S�č폜
      if(endPoint.GetX() > 0) {
        buffer.DeleteText(startPoint.GetY()+1, 0, endPoint.GetX());
      }

      //�J�n�_������͑S�č폜
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
   * �L�����b�g�̈ʒu�ɃX�N���[�����ړ�������
   * 
   */
  void AutoFixScrollPosition()
  {
    //���S�ɕ\������Ă���s�݂̂�Ώۂɂ��邽��-1����B
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
      //��C��1/3��ʕ��X�N���[��
      OnHScroll(SB_THUMBPOSITION, (caretPosition.GetX() - (rcClient.Width() / 3)) / fontSize+1, NULL);
    }

    if(caretPosition.GetX() < (hPosition) * fontSize)
    {
      OnHScroll(SB_THUMBPOSITION, caretPosition.GetX() / fontSize, NULL);
    }
  }


  /**
   * IME�̓��͕������L�����b�g�̈ʒu�Ɉړ�������B
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
   * �f�X�g���N�^
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