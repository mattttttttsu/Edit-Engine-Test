#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include "Point.h"

namespace EditTest{

/**
 * �ҏW�\�ȃe�L�X�g�f�[�^�������N���X
 * 
 */
class Buffer
{
public:

  /**
   * �e�s���i�[�����x�N�g��
   */
  std::vector<wchar_t*> lines;

  /**
   * ���݂̃L�����b�g�ʒu(�����A�s���P��)
   */
  LogicalPoint point;

public:

  /**
   * �R���X�g���N�^
   */
  Buffer()
  {
    lines.clear();
    point.SetX(0);
    point.SetY(0);
  }


  /**
   * �o�b�t�@�Ƀe�L�X�g���Z�b�g����B
   * @param LPWSTR text �Z�b�g����e�L�X�g
   */
  void SetText(LPWSTR text)
  {
    lines.clear();
    DWORD len = wcslen(text);

    int lineLength = 500;
    int linePos=0;
    LPWSTR line = new wchar_t[lineLength];
    for(DWORD i=0; i < len; i++)
    {
      if(i < len && text[i] == _T('\r') && text[i + 1] == _T('\n')) {
        line[linePos] = _T('\0');
        lines.push_back(line);
        linePos = 0;
        line = new wchar_t[lineLength];
        i++;
        continue;
      } else if(text[i] == _T('\r') || text[i] == _T('\n')) {
        line[linePos] = _T('\0');
        lines.push_back(line);
        linePos = 0;
        line = new wchar_t[lineLength];
        continue;
      }
      line[linePos] = text[i];
      linePos++;
    }

/*
    //���Ȃ苭���ȑΉ�
    if(text[len-1] == _T('\r') || text[len-1] == _T('\n')) {
      line = new wchar_t[lineLength];
      line[0] = _T('\0');
      lines.push_back(line);
    }
*/
//    if(linePos > 0) {
      line[linePos] = _T('\0');
      lines.push_back(line);
//    } else {
//      delete[] line;
//    }
  }


  /**
   * �w�肵���s�̃e�L�X�g��Ԃ��B
   * @param int line �s�ԍ�
   * @return LPWSTR �s�̃e�L�X�g
   */
  LPWSTR GetLine(int line)
  {
    int size = lines.size();
    if(line >= size) {
      return NULL;
    }

    return lines[line];
  }


  /**
   * �w�肵���s�̒�����Ԃ��B
   * @param int line �s�ԍ�
   * @return int ����(������)
   */
  int GetLineLength(int line)
  {
    LPWSTR text = GetLine(line);
    if(text == NULL) {
      return 0;
    }

    return wcslen(text);
  }


  /**
   * �w�肵���s�̎w�肵���ꏊ�Ƀe�L�X�g��}������
   * @param int line �s�ԍ�
   * @param int offset ���[����̕�����
   * @param LPWSTR insertion �}�����镶����
   */
  void InsertText(int line, int offset, LPWSTR insertion)
  {
    LPWSTR text = GetLine(line);
    
    int insLen = wcslen(insertion);
    int len = wcslen(text);
    LPWSTR newLine = new wchar_t[len + insLen + 1];
    ZeroMemory(newLine, len + insLen);
    
    wcsncpy(newLine, text, offset);
    wcsncpy(&newLine[offset], insertion, insLen);
    wcsncpy(&newLine[offset+insLen], &text[offset], len-offset);
    newLine[len + insLen] = _T('\0');

    lines[line] = newLine;
    delete[] text;
  }


  /**
   * �w�肵���ʒu�ŉ��s����
   * @param int line �s�ԍ�
   * @param int offset ���s��}�����镶���ʒu
   */
  void InsertLine(int line, int offset)
  {
    LPWSTR currentLine = GetLine(line);
    int currentLen = wcslen(currentLine);

    LPWSTR newLine = new wchar_t[currentLen - offset + 1];
    wcsncpy(newLine, &currentLine[offset], currentLen - offset);
    newLine[currentLen - offset] = 0;
    lines.insert(lines.begin() + line + 1, newLine);

    newLine = new wchar_t[offset + 1];
    wcsncpy(newLine, currentLine, offset);
    newLine[offset] = 0;
    lines[line] = newLine;
  }


  /**
   * �w�肵���ꏊ�̃e�L�X�g���폜����
   */
  void DeleteText(int line, int offset, int length)
  {
    LPWSTR currentLine = GetLine(line);
    int lineLength = wcslen(currentLine);
    int restPoint = offset + length;
    int restLen = lineLength - restPoint;

    LPWSTR tempLine = new wchar_t[restLen+1];
    wcsncpy(tempLine, &currentLine[restPoint], restLen);
    tempLine[restLen] = 0;

    wcsncpy(&currentLine[offset], tempLine, restLen);
    currentLine[offset + restLen] = 0;

    delete[] tempLine;
  }


  /**
   * �w�肵���s���폜����
   * @param int line �s�ԍ�
   */
  void DeleteLine(int line)
  {
    delete[] lines[line];
    lines.erase(lines.begin() + line);
  }


  /**
   * ���̍s�ƌ��݂̍s����������
   * @param int line �s�ԍ�
   */
  void ConcatLine(int line)
  {
    LPWSTR currentLine = GetLine(line);
    int currentLen = wcslen(currentLine);

    LPWSTR nextLine = GetLine(line+1);
    int nextLen = wcslen(nextLine);

    LPWSTR newLine = new wchar_t[currentLen + nextLen + 1];
    wcsncpy(newLine, currentLine, currentLen);
    wcsncpy(&newLine[currentLen], nextLine, nextLen);
    newLine[currentLen + nextLen] = 0;
    
    lines[line] = newLine;
    delete[] currentLine;
    DeleteLine(line+1);
  }


  /**
   * �s����Ԃ�
   * @return DWORD �s��
   */
  DWORD GetLineCount()
  {
    return lines.size();
  }


  /**
   * ���݂̃L�����b�g�̏ꏊ��Ԃ�
   * @param LogicalPoint* position ���ʂ��i�[����@a position
   */
  void GetCaretPosition(LogicalPoint* position)
  {
    position->SetX(point.GetX());
    position->SetY(point.GetY());
  }


  /**
   * �L�����b�g�̏ꏊ��ݒ肷��
   * @param LogicalPoint newPosition �V�������W
   */
  void SetCaretPosition(LogicalPoint* newPosition)
  {
    point.SetX(newPosition->GetX());
    point.SetY(newPosition->GetY());
  }


  /**
   * �f�X�g���N�^
   */
  ~Buffer()
  {
    int size = lines.size();
    for(int i=0; i < size; i++) {
      delete[] lines[i];
    }
    lines.clear();
  }
};

};

#endif
