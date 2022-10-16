#include<iostream>
#include<cstring>
#include<sstream>
#include<map>
#include<vector>
#include<deque>
#include<string>
#include<random>
#include<cmath>
#include<stdexcept>
#include<unicode/unistr.h>
#include<unicode/ustream.h>
#include<unicode/schriter.h>
#include<unicode/chariter.h>
#include<unicode/decimfmt.h>
#include"mt_parser.hpp"
#include"node.hpp"

bool parse_arguments(int32_t argc, char** argv, std::unordered_map<std::string,std::string>& params)
{
    if(argc % 2 == 0) return false;

    for(int32_t i = 1; i < argc; i += 2)
    {
        std::string key = argv[i];
        std::string value = argv[i + 1];
        
        if(key[0] != '-') return false;
        params.emplace(key.substr(1,key.size()), value);
    }
    
    return true;
}

std::vector<icu::UnicodeString> split_string(icu::UnicodeString s, char c)
{
    std::vector<icu::UnicodeString> list = {};
    int32_t b = 0;
    for(icu::StringCharacterIterator it(s); it.getIndex() < it.getLength(); it.next())
    {
        if(it.current() == ' ') {
            list.emplace_back(s.tempSubStringBetween(b, it.getIndex()));
            b = it.getIndex() + 1;
        }
    }

    list.emplace_back(s.tempSubStringBetween(b, s.length()));
    return list;
}

bool matches_string(icu::UnicodeString& s, icu::UnicodeString& d)
{
    if(s.length() != d.length()) return false;
    
    for(int32_t i = 0; i < s.length(); i++)
    {
        if(s.getBuffer()[i] != d.getBuffer()[i]) return false;
    }
    
    return true;
}

int main(int32_t argc, char** argv)
{
    std::unordered_map<std::string,std::string> params = {};
    if(!parse_arguments(argc, argv, params)) {
        std::cout << "Error" << std::endl;
        return -1;
    }

    auto input_with_unicode = [](icu::UnicodeString& s){
        std::string input;
        std::getline(std::cin, input);
        s = icu::UnicodeString(input.data(), input.size());
        return s.length();
    };
    
    MTParser::mttable table;
    
    icu::UnicodeString str[] = {
        "read",
        "test",
        "list"
    };

    
    icu::UnicodeString unistr;
    while(std::cout << ">> ", input_with_unicode(unistr))
    {
        auto elms = split_string(unistr, ' ');
        if(elms.size() == 0) continue;

        bool succeed = false;
        if(matches_string(elms[0], str[0])) {
            // command: read
            if(elms.size() != 2) {
                std::cout << "read [file]" << std::endl;
                continue;
            }

            try {
                MTParser parser;
                table = parser.read(elms.at(1));
            }
            catch(const std::invalid_argument& e) {
                std::cout << "there is no file." << std::endl;
            }

            succeed = true;
        }
        else if(matches_string(elms[0], str[1])) {
            // command: test
            if(elms.size() != 2) {
                std::cout << "test [name]" << std::endl;
                continue;
            }

            if(table.size() == 0) {
                std::cout << "no target" << std::endl;
                continue;
            }
            
            if(table.find(elms[1]) == table.end()) {
                std::cout << "unregistered name" << std::endl;
                continue;
            }
            
            auto entries = table[elms[1]];
            
            std::random_device rd;
            std::mt19937 mt(rd());
            std::shuffle(entries.begin(), entries.end(), mt);

            std::deque<float> result = {};
            for(auto&& e : entries)
            {
                auto [question, answer] = e;
                
                std::cout << "'" << question << "'" << " >>? ";
                icu::UnicodeString user_answer;
                input_with_unicode(user_answer);
                
                const int32_t sub = std::abs(answer.length() - user_answer.length());
                const float max_correctness = 1.0f - std::min(answer.length(), sub) / static_cast<float>(answer.length());
                
                const int32_t length = std::min(answer.length(), user_answer.length());
                int32_t matched_length = 0;
                for(int32_t i = 0; i < length; i++)
                {
                    matched_length += (user_answer.getBuffer()[i] == answer.getBuffer()[i]);
                }
                
                result.emplace_back(max_correctness * matched_length / static_cast<float>(length));
            }
            
            for(int32_t i = 0; i < entries.size(); i++)
            {
                auto [question, answer] = entries[i];
                std::cout << "Q:" << question << ",A:" << answer << " [" << result[i] << "]" << std::endl;
            }
            
            succeed = true;
        }
        else if(elms[0] == str[2]) {
            // command: list
            for(auto&& e : table) std::cout << e.first << std::endl;
            succeed = true;
        }
        else {
            std::cout << "unknown command." << std::endl;
        }
        
        if(succeed) {
            std::cout << "succeed." << std::endl;
        }
    }
    
    return 0;
}

