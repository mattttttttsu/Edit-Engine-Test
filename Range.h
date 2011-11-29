#ifndef _RANGE_H_
#define _RANGE_H_

namespace EditTest {

/**
 * 2�_�Ԃ̋����̏����i�[����N���X
 */
template<class T>
class Range
{
private:
  
  ///�ŏ��̓_
  T first;

  ///��2�̓_
  T second;

public:

  /**
   * �R���X�g���N�^
   */
  Range()
  {
    first = 0;
    second = 0;
  }


  /**
   * ��1�̓_�̒l���Z�b�g����B
   * @param T �l
   */
  void SetFirst(T value)
  { first = value; }


  /**
   * ��2�̓_�̒l���Z�b�g����B
   * @param T �l
   */
  void SetSecond(T value)
  { second = value; }


  /**
   * ��1�̓_�̒l��Ԃ��B
   * @return T ��1�̓_�̒l�B
   */
  T GetFirst()
  { return first; }


  /**
   * ��2�̓_�̒l��Ԃ��B
   * @return T ��2�̓_�̒l�B
   */
  T GetSecond()
  { return second; }


  /**
   * �l�̍���Ԃ��B
   * @return T ��
   */
  T GetRange()
  {
    return abs(first - second);
  }


  /**
   * �f�X�g���N�^
   */
  ~Range()
  {

  }

};

typedef Range<int> RangeI;

};


#endif
