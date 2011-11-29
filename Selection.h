#ifndef _SELECTION_H_
#define _SELECTION_H_

#include "Point.h"

namespace EditTest {

/**
 * �I��͈͂̏����i�[����N���X
 */
class Selection
{
private:

  /**
   * �I���̎x�_�B
   */
  LogicalPoint* anchorPoint;

  /**
   * ���݂̃L�����b�g�̈ʒu�B
   */
  LogicalPoint* activePoint;


public:

  /**
   * �R���X�g���N�^
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
   * �x�_���Z�b�g����B
   * @param LogicalPoint �x�_���W�B
   */
  void SetAnchorPoint(LogicalPoint* point)
  {
    anchorPoint->SetX(point->GetX());
    anchorPoint->SetY(point->GetY());
  }


  /**
   * �I�_���Z�b�g����B
   * @param LogicalPoint �I�_���W�B
   */
  void SetActivePoint(LogicalPoint* point)
  {
    activePoint->SetX(point->GetX());
    activePoint->SetY(point->GetY());
  }

  
  /**
   * �I��̈悪�󂩂ǂ�����Ԃ��B
   * @return BOOL �I��̈悪�󂩂ǂ���
   */
  BOOL IsEmpty()
  {
    return (anchorPoint->GetX() == activePoint->GetX() &&
            anchorPoint->GetY() == activePoint->GetY() );
  }


  /**
   * �w�肵�����W���I��͈͓��Ɋ܂܂�Ă��邩��Ԃ�
   * @param LogicalPoint* criteria ����̑ΏۂƂȂ�_
   * @return BOOL @a criteria ���I��͈͓��Ɋ܂܂�邩�ǂ���
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
   * �I��̈�̏���Rect�^(4�̓_)�̏��Ƃ��ĕԂ��B
   * @param CRect* out ���ʂ��i�[����ϐ�
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
   * �I���̊J�n�_��Ԃ�
   * @param LogicalPoint* out ���ʂ��i�[����ϐ�
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
   * �I���̏I�_��Ԃ�
   * @param LogicalPoint* out ���ʂ��i�[����ϐ�
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
   * �f�X�g���N�^
   */
  ~Selection()
  {
    delete anchorPoint;
    delete activePoint;
  }
};

};


#endif
