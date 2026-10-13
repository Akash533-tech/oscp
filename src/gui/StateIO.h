#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace minios::gui::stateio {
inline std::vector<int> array(const std::string& json, const std::string& key) {
    const auto keyPos=json.find('"'+key+'"'); if(keyPos==std::string::npos)return {};
    const auto begin=json.find('[',keyPos),end=json.find(']',begin);if(begin==std::string::npos||end==std::string::npos)return {};
    std::string text=json.substr(begin+1,end-begin-1);for(char&c:text)if(c==',')c=' ';std::istringstream in(text);std::vector<int>out;int value;while(in>>value)out.push_back(value);return out;
}
inline int integer(const std::string& json,const std::string&key,int fallback){const auto p=json.find('"'+key+'"');if(p==std::string::npos)return fallback;const auto c=json.find(':',p);if(c==std::string::npos)return fallback;try{return std::stoi(json.substr(c+1));}catch(...){return fallback;}}
inline std::string object(const std::string&json,const std::string&key){const auto p=json.find('"'+key+'"');if(p==std::string::npos)return {};const auto begin=json.find('{',p);if(begin==std::string::npos)return {};int depth=0;for(size_t i=begin;i<json.size();++i){if(json[i]=='{')++depth;else if(json[i]=='}'&&--depth==0)return json.substr(begin,i-begin+1);}return {};}
inline std::string list(const std::vector<int>& values){std::ostringstream o;o<<'[';for(size_t i=0;i<values.size();++i){if(i)o<<',';o<<values[i];}o<<']';return o.str();}
}  // namespace minios::gui::stateio
