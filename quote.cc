#include <iostream>
using namespace std;

// https://stackoverflow.com/questions/1494399/how-do-i-search-find-and-replace-in-a-standard-string
void replaceAll(std::string& str,
               const std::string& oldStr,
               const std::string& newStr)
{
    std::string::size_type pos = 0u;
    while((pos = str.find(oldStr, pos)) != std::string::npos){
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

void quote(string& str) {
    replaceAll(str, "\"", "\\\"");
    replaceAll(str, "\t", "\\t");
    replaceAll(str, "\n", "\\n");
    replaceAll(str, "\f", "\\f");
    replaceAll(str, "\b", "\\b");
    replaceAll(str, "\r", "\\r");
    str.insert(0, "\"");
    str.push_back('"');
}

int main() {
    string s = "hello \t\"hello";
    quote(s);
    cout << s;
}