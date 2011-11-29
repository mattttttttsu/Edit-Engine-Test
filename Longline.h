#ifndef _CPP_MB_FILTER_
#define _CPP_MB_FILTER_

#include <Windows.h>
#include <mbfl/mbfilter.h>

namespace Longline {


/**
 * エンコーディングの種類
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
 * マルチバイト文字列のエンコード変換や
 * エンコード検出を行う。
 */
class Longline
{

public:


};




/**
 * 文字コード変換フィルターのインターフェース
 */
class IFilter {
public:

  /**
   * 入力文字列をセットする。
   * @param unsigned char* input 入力文字列
   * @param DWORD length 文字列の長さ
   */
  virtual void SetInputString(unsigned char* input, DWORD length) = 0;


  /**
   * エンコーディングの変換を行う
   * @param unsigned char* buffer 変換後のデータを格納するバッファ
   * @param DWORD* resultLength 変換後の文字列の長さ
   * @param Longline::Encodings toEncoding 変換後のエンコーディング
   */
  virtual unsigned char* convert(unsigned char* buffer, DWORD* resultLength, Encodings toEncoding) = 0;

  /**
   * エンコーディングの検出を行う
   */
  virtual Encodings GetEncoding() = 0;

  /**
   * エンコーディングの検出を行う。
   * 結果を文字列で返す。
   * @param LPWSTR name 結果を格納するバッファ
   * @param int bufferLength バッファの最大長
   */
  virtual void GetEncodingAsText(LPWSTR name, int bufferLength) = 0;


  /**
   * エンコーディング名の文字列から対応するエンコーディング値を返す。
   * @param LPWSTR エンコーディング名
   * @return Longline::Encoding エンコーディング値
   */
  virtual Encodings GetEncodingValueFromText(LPWSTR name) = 0;


  /**
   * エンコーディング値から対応するエンコーディング名文字列を返す。
   * @param Longline::Encoding value エンコーディング値
   * @param LPWSTR name 結果を格納するバッファ
   * @param DWORD bufferLength バッファの最大長
   */
  virtual void GetEncodingTextFromValue(Encodings value, LPWSTR name, DWORD bufferLength) = 0;
};


/**
 * mbfilterを使用したフィルター
 */
class FilterMbFilter : public IFilter {

private:

  ///入力文字列
  unsigned char* inputString;

  ///入力文字列の長さ
  DWORD inputLength;

  ///入力文字列のエンコーディング
  Encodings inputEncoding;

  ///エンコーディング検出の順序
  mbfl_no_encoding* encodingList;

public:

  /**
   * コンストラクタ
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
   * 入力文字列をセットする。
   * @param unsigned char* input 入力文字列
   * @param DWORD length 文字列の長さ
   */
  void SetInputString(unsigned char* input, DWORD length)
  {
    inputString = input;
    inputLength = length;
  }


  /**
   * エンコーディングの変換を行う
   * @param unsigned char* buffer 変換後のデータを格納するバッファ
   * @param DWORD* resultLength 変換後の文字列の長さ
   * @param Longline::Encodings toEncoding 変換後のエンコーディング
   */
  unsigned char* convert(unsigned char* buffer, DWORD* resultLength, Encodings toEncoding)
  {
    mbfl_string base, result, *ret;
    mbfl_buffer_converter *converter;
    Encodings fromEncoding = GetEncoding();
    mbfl_no_encoding fromMbFilterEncoding = GetMbFilterEncodingFromValue(fromEncoding);
    mbfl_no_encoding toMbFilterEncoding = GetMbFilterEncodingFromValue(toEncoding);

    //BOMを含んでいる場合はその分の長さを引く
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
   * エンコーディングの検出を行う
   * @return Longline::Encodings 文字コードの種類
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
   * エンコーディングの検出を行う。
   * 結果を文字列で返す。
   * @param LPWSTR name 結果を格納するバッファ
   * @param int bufferLength バッファの最大長
   */
  void GetEncodingAsText(LPWSTR name, int bufferLength)
  {
    Encodings encoding = GetEncoding();
    GetEncodingTextFromValue(encoding, name, bufferLength);
  }


  /**
   * エンコーディング名の文字列から対応するエンコーディング値を返す。
   * @param LPWSTR エンコーディング名
   * @return Longline::Encoding エンコーディング値
   */
  Encodings GetEncodingValueFromText(LPWSTR name)
  {
    return Encodings::INVALID;
  }


  /**
   * エンコーディング値から対応するエンコーディング名文字列を返す。
   * @param Longline::Encoding value エンコーディング値
   * @param LPWSTR name 結果を格納するバッファ
   * @param DWORD bufferLength バッファの最大長
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
   * デストラクタ
   */
  ~FilterMbFilter()
  {
    delete[] encodingList;
  }


private:

  /**
   * mbfilterが使用しているmbfl_no_encodingの各値に対応する
   * Longline::Encodingsの値を返す。
   * @param mbfl_no_encoding mbfilterが使用するエンコーディング値
   * @return Longline::Encodings Longlineが使用する円コーディング値
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
   * Longlineが使用するエンコーディング値をmbfilterのエンコーディング値に
   * 変換する。
   * @param mbfl_no_encoding mbfilterが使用するエンコーディング値
   * @return Longline::Encodings Longlineが使用する円コーディング値
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