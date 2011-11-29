#ifndef _CPP_MB_FILTER_
#define _CPP_MB_FILTER_

#include <Windows.h>
#include <mbfl/mbfilter.h>

namespace Longline {


/**
 * �G���R�[�f�B���O�̎��
 */
enum Encodings
{
  UNKNOWN = -1,
  INVALID,
  ASCII,
  SHIFT_JIS,
  EUC_JP,
  UTF_8,
  UTF16_BE,
  UTF16_LE
};


/**
 * �}���`�o�C�g������̃G���R�[�h�ϊ���
 * �G���R�[�h���o���s���B
 */
class Longline
{

public:


};




/**
 * �����R�[�h�ϊ��t�B���^�[�̃C���^�[�t�F�[�X
 */
class IFilter {
public:

  /**
   * ���͕�������Z�b�g����B
   * @param unsigned char* input ���͕�����
   * @param DWORD length ������̒���
   */
  virtual void SetInputString(unsigned char* input, DWORD length) = 0;


  /**
   * �G���R�[�f�B���O�̕ϊ����s��
   * @param unsigned char* buffer �ϊ���̃f�[�^���i�[����o�b�t�@
   * @param DWORD* resultLength �ϊ���̕�����̒���
   * @param Longline::Encodings toEncoding �ϊ���̃G���R�[�f�B���O
   */
  virtual unsigned char* convert(unsigned char* buffer, DWORD* resultLength, Encodings toEncoding) = 0;

  /**
   * �G���R�[�f�B���O�̌��o���s��
   */
  virtual Encodings GetEncoding() = 0;

  /**
   * �G���R�[�f�B���O�̌��o���s���B
   * ���ʂ𕶎���ŕԂ��B
   * @param LPWSTR name ���ʂ��i�[����o�b�t�@
   * @param int bufferLength �o�b�t�@�̍ő咷
   */
  virtual void GetEncodingAsText(LPWSTR name, int bufferLength) = 0;


  /**
   * �G���R�[�f�B���O���̕����񂩂�Ή�����G���R�[�f�B���O�l��Ԃ��B
   * @param LPWSTR �G���R�[�f�B���O��
   * @return Longline::Encoding �G���R�[�f�B���O�l
   */
  virtual Encodings GetEncodingValueFromText(LPWSTR name) = 0;


  /**
   * �G���R�[�f�B���O�l����Ή�����G���R�[�f�B���O���������Ԃ��B
   * @param Longline::Encoding value �G���R�[�f�B���O�l
   * @param LPWSTR name ���ʂ��i�[����o�b�t�@
   * @param DWORD bufferLength �o�b�t�@�̍ő咷
   */
  virtual void GetEncodingTextFromValue(Encodings value, LPWSTR name, DWORD bufferLength) = 0;
};


/**
 * mbfilter���g�p�����t�B���^�[
 */
class FilterMbFilter : public IFilter {

private:

  ///���͕�����
  unsigned char* inputString;

  ///���͕�����̒���
  DWORD inputLength;

  ///���͕�����̃G���R�[�f�B���O
  Encodings inputEncoding;

  ///�G���R�[�f�B���O���o�̏���
  mbfl_no_encoding* encodingList;

public:

  /**
   * �R���X�g���N�^
   */
  FilterMbFilter()
  {
    inputString = NULL;

    inputEncoding = Encodings::UNKNOWN;

    encodingList = new mbfl_no_encoding[4];
    encodingList[0] = mbfl_no_encoding_utf8;
    encodingList[1] = mbfl_no_encoding_euc_jp;
    encodingList[2] = mbfl_no_encoding_sjis;
    encodingList[3] = mbfl_no_encoding_ascii;
  }

  /**
   * ���͕�������Z�b�g����B
   * @param unsigned char* input ���͕�����
   * @param DWORD length ������̒���
   */
  void SetInputString(unsigned char* input, DWORD length)
  {
    inputString = input;
    inputLength = length;
  }


  /**
   * �G���R�[�f�B���O�̕ϊ����s��
   * @param unsigned char* buffer �ϊ���̃f�[�^���i�[����o�b�t�@
   * @param DWORD* resultLength �ϊ���̕�����̒���
   * @param Longline::Encodings toEncoding �ϊ���̃G���R�[�f�B���O
   */
  unsigned char* convert(unsigned char* buffer, DWORD* resultLength, Encodings toEncoding)
  {
    mbfl_string base, result, *ret;
    mbfl_buffer_converter *converter;
    Encodings fromEncoding = GetEncoding();
    mbfl_no_encoding fromMbFilterEncoding = GetMbFilterEncodingFromValue(fromEncoding);
    mbfl_no_encoding toMbFilterEncoding = GetMbFilterEncodingFromValue(toEncoding);

    //BOM���܂�ł���ꍇ�͂��̕��̒���������
    DWORD startOffset = 0;
    if(fromEncoding == Encodings::UTF16_BE || fromEncoding == Encodings::UTF16_LE) {
      startOffset = 2;
    }

    mbfl_string_init(&base);
    mbfl_string_init(&result);

    base.no_language = mbfl_no_language::mbfl_no_language_japanese;
    base.no_encoding = fromMbFilterEncoding;
    base.val = &inputString[startOffset];
    base.len = inputLength - startOffset;

    converter = mbfl_buffer_converter_new(fromMbFilterEncoding, toMbFilterEncoding, 10);
    
    unsigned char* output;
    buffer = NULL;
    ret = mbfl_buffer_converter_feed_result(converter, &base, &result);
    if(ret) {
      if(ret->len) {
        *resultLength = ret->len;
      }
      output = ret->val;
    }

    //wchar_t msg[1000];
    //wsprintf(msg, _T("input=%d, result=%d"), inputLength, ret->len);
    //MessageBox(NULL, msg, NULL, 0);


    mbfl_buffer_converter_delete(converter);
    return output;
  }


  /**
   * �G���R�[�f�B���O�̌��o���s��
   * @return Longline::Encodings �����R�[�h�̎��
   */
  Encodings GetEncoding()
  {
    Encodings encoding;

    if(inputString[0] == 0xFE && inputString[1] == 0xFF) {
      encoding = Encodings::UTF16_BE;
    } else if(inputString[0] == 0xFF && inputString[1] == 0xFE) {
      encoding = Encodings::UTF16_LE;
    } else {
      mbfl_string string;
      mbfl_string_init(&string);

      string.no_language = mbfl_no_language::mbfl_no_language_japanese;
	    string.val = (unsigned char *)inputString;
	    string.len = inputLength;

      mbfl_no_encoding result = mbfl_identify_encoding_no(&string, encodingList, 4, 0);
      encoding = GetEncodingValueFromMbFilter(result);
    }


    return encoding;
  }

  /**
   * �G���R�[�f�B���O�̌��o���s���B
   * ���ʂ𕶎���ŕԂ��B
   * @param LPWSTR name ���ʂ��i�[����o�b�t�@
   * @param int bufferLength �o�b�t�@�̍ő咷
   */
  void GetEncodingAsText(LPWSTR name, int bufferLength)
  {
    Encodings encoding = GetEncoding();
    GetEncodingTextFromValue(encoding, name, bufferLength);
  }


  /**
   * �G���R�[�f�B���O���̕����񂩂�Ή�����G���R�[�f�B���O�l��Ԃ��B
   * @param LPWSTR �G���R�[�f�B���O��
   * @return Longline::Encoding �G���R�[�f�B���O�l
   */
  Encodings GetEncodingValueFromText(LPWSTR name)
  {
    return Encodings::INVALID;
  }


  /**
   * �G���R�[�f�B���O�l����Ή�����G���R�[�f�B���O���������Ԃ��B
   * @param Longline::Encoding value �G���R�[�f�B���O�l
   * @param LPWSTR name ���ʂ��i�[����o�b�t�@
   * @param DWORD bufferLength �o�b�t�@�̍ő咷
   */
  void GetEncodingTextFromValue(Encodings value, LPWSTR name, DWORD bufferLength)
  {
    LPWSTR encName = NULL;

    switch(value)
    {
    case Encodings::ASCII:     encName = _T("ascii");      break;
    case Encodings::EUC_JP:    encName = _T("EUC-JP");     break;
    case Encodings::SHIFT_JIS: encName = _T("Shift_JIS");  break;
    case Encodings::UTF_8:      encName = _T("UTF-8");      break;
    case Encodings::UTF16_BE:   encName = _T("UTF-16BE");   break;
    case Encodings::UTF16_LE:   encName = _T("UTF-16LE");   break;
    default:
      encName = _T("INVALID");
      break;
    }

    wsprintf(name, _T("%s"), encName);
  }


  /**
   * �f�X�g���N�^
   */
  ~FilterMbFilter()
  {
    delete[] encodingList;
  }


private:

  /**
   * mbfilter���g�p���Ă���mbfl_no_encoding�̊e�l�ɑΉ�����
   * Longline::Encodings�̒l��Ԃ��B
   * @param mbfl_no_encoding mbfilter���g�p����G���R�[�f�B���O�l
   * @return Longline::Encodings Longline���g�p����~�R�[�f�B���O�l
   */
  Encodings GetEncodingValueFromMbFilter(mbfl_no_encoding no)
  {
    switch(no)
    {
    case mbfl_no_encoding::mbfl_no_encoding_ascii:   return Encodings::ASCII;
    case mbfl_no_encoding::mbfl_no_encoding_euc_jp:  return Encodings::EUC_JP;
    case mbfl_no_encoding::mbfl_no_encoding_sjis:    return Encodings::SHIFT_JIS;
    case mbfl_no_encoding::mbfl_no_encoding_utf8:    return Encodings::UTF_8;
    case mbfl_no_encoding::mbfl_no_encoding_utf16be: return Encodings::UTF16_BE;
    case mbfl_no_encoding::mbfl_no_encoding_utf16le: return Encodings::UTF16_LE;
    default:
      return Encodings::INVALID;
    }
  }


  /**
   * Longline���g�p����G���R�[�f�B���O�l��mbfilter�̃G���R�[�f�B���O�l��
   * �ϊ�����B
   * @param mbfl_no_encoding mbfilter���g�p����G���R�[�f�B���O�l
   * @return Longline::Encodings Longline���g�p����~�R�[�f�B���O�l
   */
  mbfl_no_encoding GetMbFilterEncodingFromValue(Encodings encoding)
  {
    switch(encoding)
    {
    case Encodings::ASCII:      return mbfl_no_encoding::mbfl_no_encoding_ascii;
    case Encodings::EUC_JP:     return mbfl_no_encoding::mbfl_no_encoding_euc_jp;
    case Encodings::SHIFT_JIS:  return mbfl_no_encoding::mbfl_no_encoding_sjis;
    case Encodings::UTF_8:      return mbfl_no_encoding::mbfl_no_encoding_utf8;
    case Encodings::UTF16_BE:   return mbfl_no_encoding::mbfl_no_encoding_utf16be;
    case Encodings::UTF16_LE:   return mbfl_no_encoding::mbfl_no_encoding_utf16le;
    default:
      return mbfl_no_encoding::mbfl_no_encoding_invalid;
    }
  }



};




}


#endif