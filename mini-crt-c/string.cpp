#include "minicrt.h"

namespace std{
class string
{
    unsigned len;
    char* pbuf;
public:
    explicit string(const char* str);
    string(const string&);
    ~string();
    string& operator=(const string&);
    string& operator=(const char* s);
    const char* c_str() const;
    char operator[](unsigned idx) const;
    char& operator[](unsigned idx);
    unsigned length() const;
    unsigned size() const;
};

string::string(const char* str) :
    len(0), pbuf(0)
{
    *this = str; // =运算符已重载 string& string::operator=(const char* s) 
}

string::string(const string& s) :
    len(0), pbuf(0)
{
    *this = s;
}

string::~string()
{
    if(pbuf != 0) {
        delete[] pbuf;
        pbuf = 0;
    }
}

string& string::operator=(const string& s)
{
    if (&s == this)
        return *this;
    this->~string();
    len = s.len;
    pbuf = strcpy(new char[len + 1], s.pbuf);
    return *this;
}

string& string::operator=(const char* s)
{
    this->~string();
    len = strlen(s);
    pbuf = strcpy(new char[len + 1], s);
    return *this;
}
}
