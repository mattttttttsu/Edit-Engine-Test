#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>
#include "Point.h"

namespace EditTest{

/**
 * 編集可能なテキストデータを扱うクラス
 * 
 */
class Buffer
{
public:

  /**
   * 各行を格納したベクトル
   */
  std::vector<wchar_t*> lines;

  /**
   * 現在のキャレット位置(文字、行数単位)
   */
  LogicalPoint point;

public:

  /**
   * コンストラクタ
   */
  Buffer()
  {
    lines.clear();
    point.SetX(0);
    point.SetY(0);
  }


  /**
   * バッファにテキストをセットする。
   * @param LPWSTR text セットするテキスト
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
    //かなり強引な対応
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
   * 指定した行のテキストを返す。
   * @param int line 行番号
   * @return LPWSTR 行のテキスト
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
   * 指定した行の長さを返す。
   * @param int line 行番号
   * @return int 長さ(文字数)
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
   * 指定した行の指定した場所にテキストを挿入する
   * @param int line 行番号
   * @param int offset 左端からの文字数
   * @param LPWSTR insertion 挿入する文字列
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
   * 指定した位置で改行する
   * @param int line 行番号
   * @param int offset 改行を挿入する文字位置
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
   * 指定した場所のテキストを削除する
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
   * 指定した行を削除する
   * @param int line 行番号
   */
  void DeleteLine(int line)
  {
    delete[] lines[line];
    lines.erase(lines.begin() + line);
  }


  /**
   * 次の行と現在の行を結合する
   * @param int line 行番号
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
   * 行数を返す
   * @return DWORD 行数
   */
  DWORD GetLineCount()
  {
    return lines.size();
  }


  /**
   * 現在のキャレットの場所を返す
   * @param LogicalPoint* position 結果を格納する@a position
   */
  void GetCaretPosition(LogicalPoint* position)
  {
    position->SetX(point.GetX());
    position->SetY(point.GetY());
  }


  /**
   * キャレットの場所を設定する
   * @param LogicalPoint newPosition 新しい座標
   */
  void SetCaretPosition(LogicalPoint* newPosition)
  {
    point.SetX(newPosition->GetX());
    point.SetY(newPosition->GetY());
  }


  /**
   * デストラクタ
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
