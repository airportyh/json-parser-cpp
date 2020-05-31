#include <clocale>
#include <iostream>
using namespace std;

int main() {
    setlocale(LC_ALL, "");
    wchar_t c[] = {L'h', L'e', L'l', L'l', L'o', L'\u03c0', L'\u1234', L'\n', 0};
    wstring s = L"helloπworld";
    //wcout << s << endl;
    cout << L'\u03c0';

}