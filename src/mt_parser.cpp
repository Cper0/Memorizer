#include"mt_parser.hpp"

#include<deque>
#include<fstream>
#include<iostream>
#include<stdexcept>
#include<unicode/ustdio.h>
#include<unicode/ustream.h>
#include<unicode/schriter.h>

std::deque<icu::UnicodeString> MTParser::split(icu::UnicodeString s, UChar c)
{
    std::deque<icu::UnicodeString> list = {};
    
    int32_t b = 0;
    for(icu::StringCharacterIterator it(s); it.getIndex() < it.getLength(); it.next())
    {
        if(it.current() == c) {
            list.emplace_back(s.tempSubStringBetween(b, it.getIndex()));
            b = it.getIndex() + 1;
        }
    }
    list.emplace_back(s.tempSubString(b));
    return list;
}

MTParser::MTParser()
{
}

MTParser::mttable MTParser::read(icu::UnicodeString path)
{
    UFILE* file = u_fopen_u(path.getTerminatedBuffer(), "r", NULL, NULL);
    if(file == NULL) throw std::invalid_argument("there is no file.");
    icu::UnicodeString s;
    while(!u_feof(file)) s.append(u_fgetc(file));
    u_fclose(file);
    
    mttable table;
    
    auto entries = split(s, '/');
    for(auto&& entry : entries)
    {
        mtvector vec = {};

        auto expression = split(entry, '=');
        if(expression.size() != 2) break;
        
        auto key = expression[0];
        auto objects = split(*expression[1].clone(), '|');
        
        for(auto&& obj : objects)
        {
            auto values = split(obj, ':');
            if(values.size() != 2) break;
            
            vec.emplace_back(std::make_pair(values[0], values[1]));
        }
        
        //expression[0]はエラー
        table.emplace(key, vec);
    }
    
    return table;
}

void MTParser::dump(mttable& t)
{
    for(auto&& e : t)
    {
        auto [key, vec] = e;
        std::cout << key << "=" << std::endl;
        for(auto&& f : vec)
        {
            std::cout << f.first << ":" << f.second << std::endl;
        }
    }
}
