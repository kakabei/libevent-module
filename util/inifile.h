/** 
 * @file:     inifile.h
 * @Author:   fangsh
 * @email:    1447675994@qq.com
 * @data:     2023年01月03日 星期二 14时27分05秒
 * @brief:   
 */


#ifndef _INI_FILE_
#define _INI_FILE_

namespace kkb
{
class CIniFile
{
public:
    const static int SUCCESS    = 0;
    const static int ERROR      = -1;

    const static int E_INI_FILE = -4001;

public:

    CIniFile(const char *szIniFile);
    ~CIniFile();

public:
    int GetString(const char *sSection, const char *sItem,
                const char *sDefault, char *sValue, const int nValueLen);

    int GetInt(const char *sSection, const char *sItem, const int nDefault,
            int *nValue);

    int GetULongLong(const char *szSection, const char *szItem, unsigned long long ullDefault,
            unsigned long long *pullValue);

    bool IsValid();

private:

    char *  m_szBuffer;
    int     m_iSize;
};
} // namespace kkb

#endif // _INI_FILE_
