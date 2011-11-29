#ifndef _POINT_H_
#define _POINT_H_


namespace EditTest
{
  /**
   * �o�b�t�@��̃L�����b�g�B
   * �����P�ʂō��W���Ǘ�����B
   */
  class LogicalPoint
  {
  private:

    /**
     * X���W
     */
    int xPosition;

    /**
     * Y���W
     */
    int yPosition;

  public:

    /**
     * �R���X�g���N�^
     */
    LogicalPoint()
    {
      xPosition = 0;
      yPosition = 0;
    }


    /**
     * X���W���Z�b�g����
     * @param int x X���W�B
     */
    void SetX(int x)
    { xPosition = x; }


    /**
     * Y���W���Z�b�g����
     * @param int y Y���W
     */
    void SetY(int y)
    { yPosition = y; }


    /**
     * X���W��Ԃ�
     * @return int X���W
     */
    int GetX()
    { return xPosition; }


    /**
     * Y���W��Ԃ�
     * @return y���W
     */
    int GetY()
    { return yPosition; }


    /**
     * ����LogicalPoint�̒l�𒼐ڃZ�b�g����B
     * @param LogicalPoint* source ���ɂȂ�I�u�W�F�N�g
     */
    void SetPoint(LogicalPoint* source)
    {
      xPosition = source->GetX();
      yPosition = source->GetY();
    }


    /**
     * �f�X�g���N�^
     */
    ~LogicalPoint()
    {
    }
  };


  /**
   * �r���[��̃L�����b�g�B
   * �s�N�Z���P�ʂō��W���Ǘ�����B
   */
  class PhysicalPoint
  {
  private:

    /**
     * X���W(�s�N�Z��)
     */
    int xPosition;

    /**
     * Y���W(�s�N�Z��)
     */
    int yPosition;

  public:

    /**
     * �R���X�g���N�^
     */
    PhysicalPoint()
    {

    }

    
    /**
     * X���W���Z�b�g����
     * @param int x X���W�B
     */
    void SetX(int x)
    { xPosition = x; }


    /**
     * Y���W���Z�b�g����
     * @param int y Y���W
     */
    void SetY(int y)
    { yPosition = y; }


    /**
     * X���W��Ԃ�
     * @return int X���W
     */
    int GetX()
    { return xPosition; }


    /**
     * Y���W��Ԃ�
     * @return y���W
     */
    int GetY()
    { return yPosition; }


    /**
     * ����PhysicalPoint�̒l�𒼐ڃZ�b�g����B
     * @param PhysicalPoint* source ���ɂȂ�I�u�W�F�N�g
     */
    void SetPoint(PhysicalPoint* source)
    {
      xPosition = source->GetX();
      yPosition = source->GetY();
    }


    /**
     * �f�X�g���N�^
     */
    ~PhysicalPoint()
    {
    }
  };
};

#endif
