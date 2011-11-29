#ifndef _CHARACTERBORDERITERATOR_H_
#define _CHARACTERBORDERITERATOR_H_

#include "Range.h"

namespace EditTest 
{


/**
 * �X�y�[�X�A�^�u��e�L�X�g�̑I�𓙂̋��E���P�ʂ�
 * �e�L�X�g�𑖍�����C�e���[�^
 */
class CharacterBorderIterator
{
private:

  /**
   * �����Ώۂ̕�����B1�s��
   */
  LPWSTR line;

  /**
   * �I��͈͂̏��B
   * ���ɑ��݂��Ȃ��ꍇNULL�B
   */
  RangeI* selection;

  /**
   * �����̌��݈ʒu
   */
  int currentPosition;


  /**
   * ���̑����̊J�n�ʒu(=���ݑ������̕�����̏I�_)
   */
  int nextPosition;

public:

  /**
   * �R���X�g���N�^
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
   * �I�u�W�F�N�g������������B
   * @param LPWSTR text �����̑ΏۂƂȂ�e�L�X�g(1�s��)
   * @param RangeI* @a text���ɁA�I��͈͂Ɋ܂܂�Ă��镔����
   *                ���݂���ꍇ�A�I��͈͂̏��B���݂��Ȃ��ꍇ��null
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
   * ���̋��E�֐i��
   */
  void Next()
  {
    currentPosition = nextPosition;
    nextPosition = GetNextPosition();
  }


  /**
   * �������I����������Ԃ��B
   * @return BOOL �������I��������
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
   * ���݂̕������Ԃ��B
   * @param Range* range ���݂̋��E�͈̔͂��i�[����ϐ�
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
   * ���̑����ʒu��Ԃ��B
   * @return ���ɑ������J�n����ʒu�B
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

    //�I����Ԃ̃^�C�v�B
    //���݈ʒu�̃e�L�X�g���I��͈͂̓����Ȃ�1�B
    //�O���Ȃ�0�B
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

      //���͓���Space���^�u�͕K��1�����Ńg�[�N����؂�
      if(newTokenType == 1 || newTokenType == 2) {
        break;
      }

    }

    return i;
  }


  /**
   * �f�X�g���N�^
   */
  ~CharacterBorderIterator()
  {
    if(selection != NULL)
      delete selection;
  }
};

};

#endif