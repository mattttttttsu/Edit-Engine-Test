#ifndef _POINT_H_
#define _POINT_H_


namespace EditTest
{
  /**
   * バッファ上のキャレット。
   * 文字単位で座標を管理する。
   */
  class LogicalPoint
  {
  private:

    /**
     * X座標
     */
    int xPosition;

    /**
     * Y座標
     */
    int yPosition;

  public:

    /**
     * コンストラクタ
     */
    LogicalPoint()
    {
      xPosition = 0;
      yPosition = 0;
    }


    /**
     * X座標をセットする
     * @param int x X座標。
     */
    void SetX(int x)
    { xPosition = x; }


    /**
     * Y座標をセットする
     * @param int y Y座標
     */
    void SetY(int y)
    { yPosition = y; }


    /**
     * X座標を返す
     * @return int X座標
     */
    int GetX()
    { return xPosition; }


    /**
     * Y座標を返す
     * @return y座標
     */
    int GetY()
    { return yPosition; }


    /**
     * 他のLogicalPointの値を直接セットする。
     * @param LogicalPoint* source 元になるオブジェクト
     */
    void SetPoint(LogicalPoint* source)
    {
      xPosition = source->GetX();
      yPosition = source->GetY();
    }


    /**
     * デストラクタ
     */
    ~LogicalPoint()
    {
    }
  };


  /**
   * ビュー上のキャレット。
   * ピクセル単位で座標を管理する。
   */
  class PhysicalPoint
  {
  private:

    /**
     * X座標(ピクセル)
     */
    int xPosition;

    /**
     * Y座標(ピクセル)
     */
    int yPosition;

  public:

    /**
     * コンストラクタ
     */
    PhysicalPoint()
    {

    }

    
    /**
     * X座標をセットする
     * @param int x X座標。
     */
    void SetX(int x)
    { xPosition = x; }


    /**
     * Y座標をセットする
     * @param int y Y座標
     */
    void SetY(int y)
    { yPosition = y; }


    /**
     * X座標を返す
     * @return int X座標
     */
    int GetX()
    { return xPosition; }


    /**
     * Y座標を返す
     * @return y座標
     */
    int GetY()
    { return yPosition; }


    /**
     * 他のPhysicalPointの値を直接セットする。
     * @param PhysicalPoint* source 元になるオブジェクト
     */
    void SetPoint(PhysicalPoint* source)
    {
      xPosition = source->GetX();
      yPosition = source->GetY();
    }


    /**
     * デストラクタ
     */
    ~PhysicalPoint()
    {
    }
  };
};

#endif
