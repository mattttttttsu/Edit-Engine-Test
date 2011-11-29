#ifndef _SELECTION_H_
#define _SELECTION_H_

#include "Point.h"

namespace EditTest {

/**
 * 選択範囲の情報を格納するクラス
 */
class Selection
{
private:

  /**
   * 選択の支点。
   */
  LogicalPoint* anchorPoint;

  /**
   * 現在のキャレットの位置。
   */
  LogicalPoint* activePoint;


public:

  /**
   * コンストラクタ
   */
  Selection()
  {
    anchorPoint = new LogicalPoint();
    activePoint = new LogicalPoint();

    anchorPoint->SetX(0);
    anchorPoint->SetY(0);
    activePoint->SetX(0);
    activePoint->SetY(0);

  }


  /**
   * 支点をセットする。
   * @param LogicalPoint 支点座標。
   */
  void SetAnchorPoint(LogicalPoint* point)
  {
    anchorPoint->SetX(point->GetX());
    anchorPoint->SetY(point->GetY());
  }


  /**
   * 終点をセットする。
   * @param LogicalPoint 終点座標。
   */
  void SetActivePoint(LogicalPoint* point)
  {
    activePoint->SetX(point->GetX());
    activePoint->SetY(point->GetY());
  }

  
  /**
   * 選択領域が空かどうかを返す。
   * @return BOOL 選択領域が空かどうか
   */
  BOOL IsEmpty()
  {
    return (anchorPoint->GetX() == activePoint->GetX() &&
            anchorPoint->GetY() == activePoint->GetY() );
  }


  /**
   * 指定した座標が選択範囲内に含まれているかを返す
   * @param LogicalPoint* criteria 判定の対象となる点
   * @return BOOL @a criteria が選択範囲内に含まれるかどうか
   */
  BOOL IsIntersect(LogicalPoint* criteria)
  {
    if(IsEmpty()) {
      return FALSE;
    }
    int x = criteria->GetX();
    int y = criteria->GetY();

    int left   = min(anchorPoint->GetX(), activePoint->GetX()),
        right  = max(anchorPoint->GetX(), activePoint->GetX()),
        top    = min(anchorPoint->GetY(), activePoint->GetY()),
        bottom = max(anchorPoint->GetY(), activePoint->GetY());

    return (x >= left && x <= right && y >= top && y < bottom);
  }


  /**
   * 選択領域の情報をRect型(4つの点)の情報として返す。
   * @param CRect* out 結果を格納する変数
   */
  void GetRect(CRect* out)
  {
    int left   = min(anchorPoint->GetX(), activePoint->GetX()),
        right  = max(anchorPoint->GetX(), activePoint->GetX()),
        top    = min(anchorPoint->GetY(), activePoint->GetY()),
        bottom = max(anchorPoint->GetY(), activePoint->GetY());

    out->SetRect(left, top, right, bottom);
  }


  /**
   * 選択の開始点を返す
   * @param LogicalPoint* out 結果を格納する変数
   */
  void GetBeginPoint(LogicalPoint* out)
  {
    if(activePoint->GetY() == anchorPoint->GetY()) {
      out->SetPoint(((activePoint->GetX() <= anchorPoint->GetX())) ? activePoint : anchorPoint);
      return;
    } else if(activePoint->GetY() < anchorPoint->GetY()) {
      out->SetPoint(activePoint);
      return;
    }

    out->SetPoint(anchorPoint);
  }


  /**
   * 選択の終点を返す
   * @param LogicalPoint* out 結果を格納する変数
   */
  void GetEndPoint(LogicalPoint* out)
  {
    if(activePoint->GetY() == anchorPoint->GetY()) {
      out->SetPoint(((activePoint->GetX() >= anchorPoint->GetX())) ? activePoint : anchorPoint);
      return;
    } else if(activePoint->GetY() > anchorPoint->GetY()) {
      out->SetPoint(activePoint);
      return;
    }

    out->SetPoint(anchorPoint);
  }


  /**
   * デストラクタ
   */
  ~Selection()
  {
    delete anchorPoint;
    delete activePoint;
  }
};

};


#endif
