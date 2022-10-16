#pragma once

#include<algorithm>
#include<sstream>
#include<string>
#include<deque>
#include<nlohmann/json.hpp>
#include<nlohmann/adl_serializer.hpp>
#include<unicode/unistr.h>
#include<unicode/ustream.h>

struct Node
{
    std::deque<float> validity;
    icu::UnicodeString question;
    icu::UnicodeString answer;
    
    std::string to_string()
    {
        std::stringstream ss;
        
        const float avg_val = std::accumulate(validity.begin(), validity.end(), 0.0f) / validity.size();

        ss << "avg:" << avg_val << " ";
        ss << "Q:" << question << " ";
        ss << "A:" << answer << " ";
        
        return ss.str();
    }
};
