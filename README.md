# JSON Parser in C++

This project implements a JSON Parser in recursive descent style.
Mainly it is used as a way for me to learn C++. Future plans are
to add a language that allows for easy transformation of JSON
files.

## Todo

* parenthesis for binary operators
* conditional
* operator precedence
* use namespaces
* organize code in c++ convensions
* all the binary operators (done)
    * `+` `-` `*` `/`
    * `>` `<` etc
* add transform language (done0)
    * parser
    * intepreter
* implement hex escape sequences to make it work with multi-byte characters (done)
* pretty printer (done)
* `./read-json-file.bin read-json-file.bin` segfaults (done)

## Resources

* https://stackoverflow.com/questions/8788057/how-to-initialize-and-print-a-stdwstring
* https://en.wikipedia.org/wiki/Bitwise_operation
* ["Characters, Symbols and the Unicode Miracle - Computerphile"](https://www.youtube.com/watch?v=MijmeoH9LT4)
                        