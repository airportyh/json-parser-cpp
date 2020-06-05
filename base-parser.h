#include <iostream>

using namespace std;

class ParseException : public exception {
    public:
    string message;

    ParseException(string msg): message(msg) {}

    const char* what() const throw()
    {
        return message.c_str();
    }
};

class BaseParser {
    public:
    int cursor;
    string input;

    BaseParser(string i) {
        input = i;
        cursor = 0;
    }

    JsonValue *parseNull() {
        if (input.substr(cursor, 4) == "null") {
            cursor += 4;
            return JsonValue::null();
        } else {
            return NULL;
        }
    }

    JsonValue *parseBoolean() {
        if (input.substr(cursor, 4) == "true") {
            cursor += 4;
            return JsonValue::boolean(true);
        } else if (input.substr(cursor, 5) == "false") {
            cursor += 5;
            return JsonValue::boolean(false);
        } else {
            return NULL;
        }
    }

    JsonValue *parseString() {
        int cursorStart = cursor;
        if (input[cursor] == '"') {
            cursor++;
            string *str = parseCharacters();
            if (input[cursor] == '"') {
                cursor++;
                return JsonValue::string(*str);
            } else {
                throw ParseException("Expected closing quote for a string.");
            }
        }
        return NULL;
    }

    string *parseCharacters() {
        string *characters = new string("");
        while (parseCharacter(characters));
        return characters;
    }

    bool parseCharacter(std::string *characters) {
        char chr = input[cursor];
        if (chr == '"') {
            return false;
        } else if (chr == '\\') {
            cursor++;
            parseEscape(characters);
            return true;
        } else {
            cursor++;
            characters->push_back(chr);
            return true;
        }
    }

    void parseEscape(string *characters) {
        char chr = input[cursor];
        switch (chr) {
            case '"':
            case '\\':
            case '/':
                cursor++;
                characters->push_back(chr);
                break;
            case 'b':
                cursor++;
                characters->push_back('\b');
                break;
            case 'f':
                cursor++;
                characters->push_back('\f');
                break;
            case 'n':
                cursor++;
                characters->push_back('\n');
                break;
            case 'r':
                cursor++;
                characters->push_back('\r');
                break;
            case 't':
                cursor++;
                characters->push_back('\t');
                break;
            case 'u':
                cursor++;
                {
                    int h1 = parseHex();
                    int h2 = parseHex();
                    int h3 = parseHex();
                    int h4 = parseHex();
                    if (h1 != -1 && h2 != -1 && h3 != -1 && h4 != -1) {
                        // UTF-8 unicode conversion, based on
                        // "Characters, Symbols and the Unicode Miracle - Computerphile"
                        // https://www.youtube.com/watch?v=MijmeoH9LT4
                        int code = ((((h1 * 16) + h2) * 16) + h3) * 16 + h4;
                        char byte1 = 0b11000000 + ((0b11111000000 & code) >> 6);
                        char byte2 = 0b10000000 +  (0b00000111111 & code);
                        characters->push_back(byte1);
                        characters->push_back(byte2);
                    } else {
                        throw ParseException("Expected 4 hexadecimal numbers for u escape sequence.");
                    }
                }
                break;
            default:
                throw ParseException("Invalid escape sequence.");
        }
    }

    int parseHex() {
        char chr = toupper(input[cursor]);

        if (chr >= 48 && chr <= 57) {        /* 0-9 */
            cursor++;
            return chr - 48;
        } else if (chr >= 65 && chr <= 70) { /* A-F */
            cursor++;
            return 10 + (chr - 65);
        } else {
            return -1;
        }

    }

    JsonValue *parseNumber() {
        int cursorStart = cursor;
        if (parseInteger()) {
            parseFraction();
            parseExponent();
            double number = stod(input.substr(cursorStart, cursor - cursorStart));
            return JsonValue::number(number);
        } else {
            return NULL;
        }
    }

    bool parseInteger() {
        bool negative = false;
        if (input[cursor] == '-') {
            cursor++;
            negative = true;
        }
        int cursorStart = cursor;
        if (parseOneNine() && parseDigits()) {
            return true;
        } else {
            cursor = cursorStart;
            if (parseDigit()) {
                return true;
            }
        }
        return false;
    }

    bool parseOneNine() {
        char chr = input[cursor];
        if (chr >= 49 && chr <= 57) {
            cursor++;
            return true;
        }
        return false;
    }

    bool parseDigit() {
        char chr = input[cursor];
        if (chr >= 48 && chr <= 57) {
            cursor++;
            return true;
        }
        return false;
    }

    bool parseDigits() {
        int cursorStart = cursor;
        while (parseDigit());
        if (cursorStart == cursor) {
            return false;
        } else {
            return true;
        }
    }

    void parseFraction() {
        if (input[cursor] == '.') {
            cursor++;
            if (parseDigits()) {
                return;
            } else {
                throw ParseException("No digits after decimal point.");
            }
        }
    }

    void parseExponent() {
        char chr = input[cursor];
        if (chr == 'e' || chr == 'E') {
            cursor++;
            parseSign();
            if (parseDigits()) {
                return;
            } else {
                throw ParseException("No digits after exponent.");
            }
        }
    }

    void parseSign() {
        char chr = input[cursor];
        if (chr == '-' || chr == '+') {
            cursor++;
        }
    }

    void parseWs() {
        while (true) {
            char chr = input[cursor];
            if (chr == ' ' || chr == '\t' || chr == '\n' || chr == 'r') {
                cursor++;
            } else {
                return;
            } 
        }
    }
};