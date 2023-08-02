
#ifndef MSGDECRYPTER_PARSER_H
#define MSGDECRYPTER_PARSER_H

#include <vector>
#include <string>
#include <map>
#include <any>

enum MsgPacketType {
    MsgText         = 1,
    MsgFace         = 2,
    MsgGroupImage   = 3,
    MsgPrivateImage = 6,
    MsgVoice        = 7,
    MsgNickName     = 18,
    MsgVideo        = 26
};

class Parser {
private:
    using returnType = std::vector<std::pair<std::uint8_t , std::string>>;
    returnType decodeContent;
public:
    Parser() = default;

    returnType doParser(std::vector<uint8_t> &BytesData);

    void clear();
};


#endif //MSGDECRYPTER_PARSER_H
