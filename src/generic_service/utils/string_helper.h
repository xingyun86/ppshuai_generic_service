#pragma once

#include <regex>
#include <filesystem>
#include <unordered_map>

#ifdef _MSC_VER
#define PATH_SPLITER_CHAR		'\\'
#define MAX_PATH_LEN			MAX_PATH
#else
#define PATH_SPLITER_CHAR		'/'
#define MAX_PATH_LEN			4096
#include <dirent.h>
#endif

#define NULL_CHAR				'\0'
#define __PROG_PATH_MAIN__		static char PROG_PATH[MAX_PATH_LEN] = { 0 };

#define __PROG_PATH_INIT__(X)	{								\
	strncpy(PROG_PATH, X, sizeof(PROG_PATH));					\
	char* PROG_PATH_PTR = strrchr(PROG_PATH, PATH_SPLITER_CHAR);\
	if (PROG_PATH_PTR != NULL) {								\
		*(PROG_PATH_PTR + 1) = NULL_CHAR;						\
	}															\
	else {														\
		*(PROG_PATH_PTR) = NULL_CHAR;							\
	}															\
}
__inline static
int enum_file(std::unordered_map<std::string, std::string>& file_list, const std::string& root)
{
	std::error_code ec;
	for (auto& item : std::filesystem::recursive_directory_iterator(root, std::filesystem::directory_options::skip_permission_denied | std::filesystem::directory_options::follow_directory_symlink, ec))
	{
		try
		{
			if (item.is_regular_file())
			{
				std::string _p = item.path().string();
				//printf("_p=%s\n", _p.c_str());
			}
		}
		catch (const std::exception & e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return 0;
}
__inline static 
bool string_regex_valid(const std::string& data, const std::string& pattern)
{
    return std::regex_match(data, std::regex(pattern));
}
__inline static 
size_t string_regex_replace_all(std::string& result, std::string& data, const std::string& replace, const std::string& pattern, std::regex_constants::match_flag_type matchflagtype = std::regex_constants::match_default)
{
    try
    {
        data = std::regex_replace(data, std::regex(pattern), replace, matchflagtype);
    }
    catch (const std::exception& e)
    {
        result = e.what();
    }
    return data.length();
}
__inline static 
size_t string_regex_find(std::string& result, std::vector<std::vector<std::string>>& svv, const std::string& data, const std::string& pattern)
{
    std::smatch smatch;
    std::string::const_iterator ite = data.end();
    std::string::const_iterator itb = data.begin();
    try
    {
        //如果匹配成功
        while (std::regex_search(itb, ite, smatch, std::regex(pattern)))
        {
            if (smatch.size() > 1)
            {
                for (size_t stidx = svv.size() + 1; stidx < smatch.size(); stidx++)
                {
                    svv.push_back(std::vector<std::string>());
                }
                for (size_t stidx = 1; stidx < smatch.size(); stidx++)
                {
                    svv.at(stidx - 1).push_back(std::string(smatch[stidx].first, smatch[stidx].second));
                    itb = smatch[stidx].second;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        result = e.what();
    }
    return svv.size();
}
__inline static 
std::string::size_type string_reader(std::string& result, const std::string& strData,
	const std::string& strStart, const std::string& strFinal, std::string::size_type stPos = 0,
	bool bTakeStart = false, bool bTakeFinal = false)
{
    std::string::size_type stRetPos = std::string::npos;
    std::string::size_type stStartPos = stPos;
    std::string::size_type stFinalPos = std::string::npos;

    stStartPos = strData.find(strStart, stStartPos);
    if (stStartPos != std::string::npos)
    {
        stRetPos = stFinalPos = strData.find(strFinal, stStartPos + strStart.length());
        if (stFinalPos != std::string::npos)
        {
            if (!bTakeStart)
            {
                stStartPos += strStart.length();
            }
            if (bTakeFinal)
            {
                stFinalPos += strFinal.length();
            }
            result = strData.substr(stStartPos, stFinalPos - stStartPos);
        }
    }
    return stRetPos;
}
__inline static 
std::string string_reader(const std::string& strData,
	const std::string& strStart, const std::string& strFinal,
	bool bTakeStart = false, bool bTakeFinal = false)
{
    std::string::size_type stStartPos = std::string::npos;
    std::string::size_type stFinalPos = std::string::npos;
    stStartPos = strData.find(strStart);
    if (stStartPos != std::string::npos)
    {
        stFinalPos = strData.find(strFinal, stStartPos + strStart.length());
        if (stFinalPos != std::string::npos)
        {
            if (!bTakeStart)
            {
                stStartPos += strStart.length();
            }
            if (bTakeFinal)
            {
                stFinalPos += strFinal.length();
            }
            return std::move(strData.substr(stStartPos, stFinalPos - stStartPos));
        }
    }
    return ("");
}
__inline static 
std::string& string_replace_all(std::string& strData, const std::string& strDst, const std::string& strSrc, std::string::size_type stPos = 0)
{
    while ((stPos = strData.find(strSrc, stPos)) != std::string::npos)
    {
        strData.replace(stPos, strSrc.length(), strDst);
		stPos += strDst.length() - strSrc.length();
    }
    return strData;
}
__inline static 
size_t string_split_to_vector(std::vector<std::string>& sv, const std::string& strData, const std::string& strSplitter, std::string::size_type stPos = 0)
{
    std::string strTmp = ("");
    std::string::size_type stIdx = 0;
    std::string::size_type stSize = strData.length();
    while ((stPos = strData.find(strSplitter, stIdx)) != std::string::npos)
    {
        strTmp = strData.substr(stIdx, stPos - stIdx);
        if (!strTmp.length())
        {
            strTmp = ("");
        }
        sv.push_back(strTmp);
        stIdx = stPos + strSplitter.length();
    }
    if (stIdx < stSize)
    {
        strTmp = strData.substr(stIdx, stSize - stIdx);
        if (!strTmp.length())
        {
            strTmp = ("");
        }
        sv.push_back(strTmp);
    }
    return sv.size();
}

class CRandomString {
public:
    CRandomString() { srand(time(nullptr)); }
    virtual ~CRandomString() {}
private:
    char _digit()
    {
        return static_cast<char>('0' + rand() % ('9' - '0' + 1));
    }
    char _lower()
    {
        return static_cast<char>('a' + rand() % ('z' - 'a' + 1));
    }
    char _upper()
    {
        return static_cast<char>('A' + rand() % ('Z' - 'A' + 1));
    }
public:
    std::string make_random_string(int length) {
        std::string str = ("");
        while(length-- != 0)
        {
            switch (rand() % 3)
            {
            case 0:str.push_back(_digit()); break;
            case 1:str.push_back(_lower()); break;
            default:str.push_back(_upper()); break;
            }
        }
        return std::move(str);
    }
public:
    SINGLETON_INSTANCE(CRandomString)
};