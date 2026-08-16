#include "CommandParser.h"
#include "utility.h"
using namespace std;

ParsedCommand CommandParser::parse(
    const string &input
) const {

    ParsedCommand result;

    string inputTrimmed = trim(input);

    if(inputTrimmed.empty()){
        return result;
    }

    size_t pos = inputTrimmed.find(' ');

    if(pos == string::npos){
        result.command = inputTrimmed;
        return result;
    }

    result.command = inputTrimmed.substr(0, pos);

    string argumentsString =trim(inputTrimmed.substr(pos + 1));

    if(argumentsString.empty()){
        return result;
    }

    size_t start = 0;

    while(start < argumentsString.size()){

        size_t space =
            argumentsString.find(' ', start);

        if(space == string::npos){
            result.arguments.push_back(argumentsString.substr(start));
            break;
        }

        result.arguments.push_back(argumentsString.substr(start,space - start));

        start = space + 1;
    }

    return result;
}
