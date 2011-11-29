#ifndef _ENCODING_TEST_H_
#define _ENCODING_TEST_H_

#include "Longline.h"

namespace EncodingTest {

using namespace Longline;

class EncodingTest
{

private:

  mbfl_string string;

public:

  EncodingTest()
  {
    mbfl_string_init(&string);
    
  }


  void Test()
  {
    unsigned char* testBuffer = new unsigned char[1000];
    ZeroMemory(testBuffer, 1000);
    HANDLE hFile = CreateFile(_T("guess.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);

    DWORD read=0;
    ReadFile(hFile, testBuffer, 1000, &read, NULL);
    CloseHandle(hFile);

    FilterMbFilter* filter = new FilterMbFilter();
    filter->SetInputString(testBuffer, read);

    LPWSTR encodingName = new wchar_t[1000];
    ZeroMemory(encodingName, 1000);
    filter->GetEncodingAsText(encodingName, 1000);

    MessageBox(NULL, encodingName, _T("Result"), 0);

    unsigned char* converted = NULL;
    DWORD dwLen = 0;
    converted = filter->convert(NULL, &dwLen, Encodings::EUC_JP);

    if(!converted) {
      MessageBox(NULL, _T("converted=NULL"), NULL, 0);
    }

    HANDLE hConverted = CreateFile(_T("converted.txt"), GENERIC_WRITE, FILE_SHARE_READ,
                        NULL, CREATE_ALWAYS, 0, 0);
    
    WriteFile(hConverted, "---------\n", 10, &read, 0);
    WriteFile(hConverted, converted, dwLen, &read, 0);

    CloseHandle(hConverted);
    delete[] encodingName;

/*
    mbfl_no_encoding* encodingList = new mbfl_no_encoding[7];
    string.no_language = mbfl_no_language::mbfl_no_language_japanese;
	  string.val = (unsigned char *)testBuffer;
	  string.len = 20;

//    encodingList[0] = mbfl_no_encoding_u
    encodingList[0] = mbfl_no_encoding_utf8;
    encodingList[1] = mbfl_no_encoding_euc_jp;
    encodingList[2] = mbfl_no_encoding_sjis;
    encodingList[3] = mbfl_no_encoding_ascii;

    const char* result;// = new char[20];
    //ZeroMemory(result, 20);

	  result = mbfl_identify_encoding_name(&string, encodingList, 7, 0);
  
    hFile = CreateFile(_T("test.txt"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, 0);

    DWORD written=0;
    WriteFile(hFile, "Result is...\n", strlen("Result is...\n"), &written, NULL);
    WriteFile(hFile, result, strlen(result), &written, NULL);

    CloseHandle(hFile);

    //MessageBox(NULL, result, _T("Result is..."), 0);

    //delete[] result;
    delete[] encodingList;
*/
  }


};

}

#endif