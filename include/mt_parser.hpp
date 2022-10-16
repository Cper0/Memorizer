#pragma once

#include<unicode/unistr.h>
#include<unicode/regex.h>
#include<map>
#include<deque>
#include<vector>

class MTParser
{
    private:

    public:
        using mtpair = std::pair<icu::UnicodeString,icu::UnicodeString>;
        using mtvector = std::deque<mtpair>;
        using mttable = std::map<icu::UnicodeString,mtvector>;

        MTParser();
        
        mttable read(icu::UnicodeString path);

        static void dump(mttable& t);

    private:
        static std::deque<icu::UnicodeString> split(icu::UnicodeString s, UChar c);
};
