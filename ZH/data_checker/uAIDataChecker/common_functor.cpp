
#include "common_functor.h"


BEGIN_NAMESPACE;

  
std::vector<std::string> Split( const std::string &s, const std::string &seperator )
{
    std::vector<std::string> result;
    typedef std::string::size_type string_size;
    string_size i = 0;

    while(i != s.size())
    {
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while(i != s.size() && flag == 0)
        {
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x)
            {
                if(s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
            }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0)
        {
            for(string_size x = 0; x < seperator.size(); ++x)
            {
                if(s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
                if(flag == 0)
                {
                    ++j;
                }
            }
        }
        if(i != j)
        {
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}


std::string ChangeTextFromUTF8ToAnsi( std::string& str )
{
  
	//std::wstring wszVal;
	//int nLen = ::MultiByteToWideChar(CP_UTF8, 0, 
	//	str.data(),(int)str.size(), NULL, 0);
	//wchar_t* pwChar = new wchar_t[nLen];
	//::MultiByteToWideChar(CP_UTF8, 0, 
	//	str.data(), (int)str.size(), pwChar, nLen);
	//wszVal = std::wstring(pwChar, nLen);
	//delete[] pwChar;

	//nLen = ::WideCharToMultiByte(CP_ACP, 0, 
	//	wszVal.data(), (int)wszVal.size(), NULL, 0, NULL, NULL);
	//char* pszChar = new char[nLen];
	//::WideCharToMultiByte(CP_ACP, 0, 
	//	wszVal.data(), (int)wszVal.size(), pszChar, nLen, NULL, NULL);
	//std::string sReturn = std::string(pszChar, nLen);
	//delete[] pszChar;
	//return sReturn;
    return str;
}

bool ConvertDT( double& dDateTime, tm& timeResult )
{
	unsigned int iDate = (unsigned int)(dDateTime / 1000000);
	unsigned long long iYYYYMMDD = (unsigned long long)iDate * 1000000;
	if (iYYYYMMDD == 0)
	{
		return false;
	}
	double dHHMMSSFF = dDateTime - iYYYYMMDD;
	size_t iDAY = iYYYYMMDD % 100;
	size_t iRemain = iYYYYMMDD / 100;
	size_t iMON = iRemain % 100;
	iRemain = iRemain / 100;
	size_t iYEAR = iRemain;

	// Convert Time
	size_t iHHMMSS = static_cast<size_t>(dHHMMSSFF);
	size_t iSS = iHHMMSS % 100;
	iRemain = iHHMMSS / 100;
	size_t iMM = iRemain % 100;
	iRemain = iRemain / 100;
	size_t iHH = iRemain;
	timeResult.tm_year = static_cast<int>(iYEAR - 1900);/* years since 1900 */
	timeResult.tm_mon = static_cast<int>(iMON - 1);/* months since January - [0,11] */
	timeResult.tm_mday = static_cast<int>(iDAY);/* day of the month - [1,31] */
	timeResult.tm_hour = static_cast<int>(iHH);/* hours since midnight - [0,23] */
	timeResult.tm_min = static_cast<int>(iMM);/* minutes after the hour - [0,59] */
	timeResult.tm_sec = static_cast<int>(iSS);/* seconds after the minute - [0,59] */
	return true;
}

bool ConvertDA( unsigned long long& iYYYYMMDD,tm& timeResult )
{
	if (iYYYYMMDD == 0)
	{
		return false;
	}
	size_t iDAY = iYYYYMMDD % 100;
	size_t iRemain = iYYYYMMDD / 100;
	size_t iMON = iRemain % 100;
	iRemain = iRemain / 100;
	size_t iYEAR = iRemain;

	timeResult.tm_year = static_cast<int>(iYEAR - 1900);/* years since 1900 */
	timeResult.tm_mon = static_cast<int>(iMON - 1);/* months since January - [0,11] */
	timeResult.tm_mday = static_cast<int>(iDAY);/* day of the month - [1,31] */
	return true;
}

bool ConvertTM( double& dHHMMSSFF,tm& timeResult )
{
	// Convert Time
	size_t iHHMMSS = static_cast<size_t>(dHHMMSSFF);
	if (iHHMMSS == 0)
	{
		return false;
	}
	size_t iSS = iHHMMSS % 100;
	size_t iRemain = iHHMMSS / 100;
	size_t iMM = iRemain % 100;
	iRemain = iRemain / 100;
	size_t iHH = iRemain;
	timeResult.tm_hour = static_cast<int>(iHH);/* hours since midnight - [0,23] */
	timeResult.tm_min = static_cast<int>(iMM);/* minutes after the hour - [0,59] */
	timeResult.tm_sec = static_cast<int>(iSS);/* seconds after the minute - [0,59] */
	return true;
}

END_NAMESPACE