#ifndef _CHARACTERBORDERITERATOR_H_
#define _CHARACTERBORDERITERATOR_H_

#include "Range.h"

namespace EditTest 
{


/**
 * スペース、タブやテキストの選択等の境界線単位で
 * テキストを走査するイテレータ
 */
class CharacterBorderIterator
{
private:

  /**
   * 走査対象の文字列。1行分
   */
  LPWSTR line;

  /**
   * 選択範囲の情報。
   * 特に存在しない場合NULL。
   */
  RangeI* selection;

  /**
   * 走査の現在位置
   */
  int currentPosition;


  /**
   * 次の走査の開始位置(=現在走査中の文字列の終点)
   */
  int nextPosition;

public:

  /**
   * コンストラクタ
   */
  CharacterBorderIterator()
  {
    line = NULL;
    selection = NULL;

    currentPosition = 0;
    nextPosition = 0;

    selection = NULL;
  }


  /**
   * オブジェクトを初期化する。
   * @param LPWSTR text 走査の対象となるテキスト(1行分)
   * @param RangeI* @a text内に、選択範囲に含まれている部分が
   *                存在する場合、選択範囲の情報。存在しない場合はnull
   */
  void Initialize(LPWSTR text, RangeI* range)
  {
    line = text;

    if(range != NULL) {
      selection = new RangeI();
      selection->SetFirst(range->GetFirst());
      selection->SetSecond(range->GetSecond());
    }
    nextPosition = GetNextPosition();
  }


  /**
   * 次の境界へ進む
   */
  void Next()
  {
    currentPosition = nextPosition;
    nextPosition = GetNextPosition();
  }


  /**
   * 走査が終了したかを返す。
   * @return BOOL 走査が終了したか
   */
  BOOL IsEnd()
  {
    if(line == NULL) {
      return TRUE;
    }

    int len = wcslen(line);
    return (currentPosition > len-1);
  }


  /**
   * 現在の文字列を返す。
   * @param Range* range 現在の境界の範囲を格納する変数
   */
  void GetCurrent(RangeI* range)
  {
    if(line == NULL) {
      return;
    }
/*
    int len = wcslen(line);
    if(currentPosition >= len) {
      currentPosition = nextPosition = len - 1;
    }
*/
    range->SetFirst(currentPosition);
    range->SetSecond(nextPosition);
  }


  /**
   * 次の走査位置を返す。
   * @return 次に走査を開始する位置。
   */
  int GetNextPosition()
  {
    if(line == NULL) {
      return 0;
    }

    int len = wcslen(line);
/*
    if(currentPosition >= len) {
      currentPosition = nextPosition = len - 1;
    }
*/    
    int i = currentPosition;
    int tokenType = 0, newTokenType = 0,
        selectionType = 0, newSelectionType = 0;

    if(line[i] == _T(' ')) tokenType = 1; //space
    if(line[i] == _T('\t')) tokenType = 2; //TAB

    //選択状態のタイプ。
    //現在位置のテキストが選択範囲の内側なら1。
    //外側なら0。
    selectionType = (selection->GetRange() > 0 && i >= selection->GetFirst() && i < selection->GetSecond());

    for(i++; i < len; i++)
    {
      newTokenType = 0;
      if(line[i] == _T(' '))  newTokenType = 1; //space
      if(line[i] == _T('\t')) newTokenType = 2; //TAB

      newSelectionType = (selection->GetRange() > 0 && i >= selection->GetFirst() && i < selection->GetSecond());

      if(tokenType != newTokenType || selectionType != newSelectionType) {
        break;
      }

      //今は特別Spaceかタブは必ず1文字でトークンを切る
      if(newTokenType == 1 || newTokenType == 2) {
        break;
      }

    }

    return i;
  }


  /**
   * デストラクタ
   */
  ~CharacterBorderIterator()
  {
    if(selection != NULL)
      delete selection;
  }
};

};

#endif