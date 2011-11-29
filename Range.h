#ifndef _RANGE_H_
#define _RANGE_H_

namespace EditTest {

/**
 * 2点間の距離の情報を格納するクラス
 */
template<class T>
class Range
{
private:
  
  ///最初の点
  T first;

  ///第2の点
  T second;

public:

  /**
   * コンストラクタ
   */
  Range()
  {
    first = 0;
    second = 0;
  }


  /**
   * 第1の点の値をセットする。
   * @param T 値
   */
  void SetFirst(T value)
  { first = value; }


  /**
   * 第2の点の値をセットする。
   * @param T 値
   */
  void SetSecond(T value)
  { second = value; }


  /**
   * 第1の点の値を返す。
   * @return T 第1の点の値。
   */
  T GetFirst()
  { return first; }


  /**
   * 第2の点の値を返す。
   * @return T 第2の点の値。
   */
  T GetSecond()
  { return second; }


  /**
   * 値の差を返す。
   * @return T 差
   */
  T GetRange()
  {
    return abs(first - second);
  }


  /**
   * デストラクタ
   */
  ~Range()
  {

  }

};

typedef Range<int> RangeI;

};


#endif
