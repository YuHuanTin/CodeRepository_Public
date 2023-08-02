//
// Created by 23945 on 2023/8/1.
//

#include <fmt/format.h>
#include <vector>
#include <sstream>
#include <map>
#include <functional>
#include "CodeCvt.h"
#include "Buffer.h"

std::map<std::size_t, std::string> g_MsgFaceMap{
        {0,   "¾ªÑÈ"},
        {1,   "Æ²×ì"},
        {2,   "É«"},
        {3,   "·¢´ô"},
        {4,   "µÃÒâ"},
        {5,   "Á÷Àá"},
        {6,   "º¦Ðß"},
        {7,   "±Õ×ì"},
        {8,   "Ë¯"},
        {9,   "´ó¿Þ"},
        {10,  "ÞÏÞÎ"},
        {11,  "·¢Å­"},
        {12,  "µ÷Æ¤"},
        {13,  "ßÚÑÀ"},
        {14,  "Î¢Ð¦"},
        {15,  "ÄÑ¹ý"},
        {16,  "¿á"},
        {18,  "×¥¿ñ"},
        {19,  "ÍÂ"},
        {20,  "ÍµÐ¦"},
        {21,  "¿É°®"},
        {22,  "°×ÑÛ"},
        {23,  "°ÁÂý"},
        {24,  "¼¢¶ö"},
        {25,  "À§"},
        {26,  "¾ª¿Ö"},
        {27,  "Á÷º¹"},
        {28,  "º©Ð¦"},
        {29,  "ÓÆÏÐ"},
        {30,  "·Ü¶·"},
        {31,  "ÖäÂî"},
        {32,  "ÒÉÎÊ"},
        {33,  "Ðê"},
        {34,  "ÔÎ"},
        {35,  "ÕÛÄ¥"},
        {36,  "Ë¥"},
        {37,  "÷¼÷Ã"},
        {38,  "ÇÃ´ò"},
        {39,  "ÔÙ¼û"},
        {41,  "·¢¶¶"},
        {42,  "°®Çé"},
        {43,  "ÌøÌø"},
        {46,  "ÖíÍ·"},
        {49,  "Óµ±§"},
        {53,  "µ°¸â"},
        {54,  "ÉÁµç"},
        {55,  "Õ¨µ¯"},
        {56,  "µ¶"},
        {57,  "×ãÇò"},
        {59,  "±ã±ã"},
        {60,  "¿§·È"},
        {61,  "·¹"},
        {63,  "Ãµ¹å"},
        {64,  "µòÐ»"},
        {66,  "°®ÐÄ"},
        {67,  "ÐÄËé"},
        {69,  "ÀñÎï"},
        {74,  "Ì«Ñô"},
        {75,  "ÔÂÁÁ"},
        {76,  "ÔÞ"},
        {77,  "²È"},
        {78,  "ÎÕÊÖ"},
        {79,  "Ê¤Àû"},
        {85,  "·ÉÎÇ"},
        {86,  "âæ»ð"},
        {89,  "Î÷¹Ï"},
        {96,  "Àäº¹"},
        {97,  "²Áº¹"},
        {98,  "¿Ù±Ç"},
        {99,  "¹ÄÕÆ"},
        {100, "ôÜ´óÁË"},
        {101, "»µÐ¦"},
        {102, "×óºßºß"},
        {103, "ÓÒºßºß"},
        {104, "¹þÇ·"},
        {105, "±ÉÊÓ"},
        {106, "Î¯Çü"},
        {107, "¿ì¿ÞÁË"},
        {108, "ÒõÏÕ"},
        {109, "×óÇ×Ç×"},
        {110, "ÏÅ"},
        {111, "¿ÉÁ¯"},
        {112, "²Ëµ¶"},
        {113, "Æ¡¾Æ"},
        {114, "ÀºÇò"},
        {115, "Æ¹ÅÒ"},
        {116, "Ê¾°®"},
        {117, "Æ°³æ"},
        {118, "±§È­"},
        {119, "¹´Òý"},
        {120, "È­Í·"},
        {121, "²î¾¢"},
        {122, "°®Äã"},
        {123, "NO"},
        {124, "OK"},
        {125, "×ªÈ¦"},
        {126, "¿ÄÍ·"},
        {127, "»ØÍ·"},
        {128, "ÌøÉþ"},
        {129, "»ÓÊÖ"},
        {130, "¼¤¶¯"},
        {131, "½ÖÎè"},
        {132, "Ï×ÎÇ"},
        {133, "×óÌ«¼«"},
        {134, "ÓÒÌ«¼«"},
        {136, "Ë«Ï²"},
        {137, "±ÞÅÚ"},
        {138, "µÆÁý"},
        {140, "K¸è"},
        {144, "ºÈ²Ê"},
        {145, "Æíµ»"},
        {146, "±¬½î"},
        {147, "°ô°ôÌÇ"},
        {148, "ºÈÄÌ"},
        {151, "·É»ú"},
        {158, "³®Æ±"},
        {168, "Ò©"},
        {169, "ÊÖÇ¹"},
        {171, "²è"},
        {172, "Õ£ÑÛ¾¦"},
        {173, "Àá±¼"},
        {174, "ÎÞÄÎ"},
        {175, "ÂôÃÈ"},
        {176, "Ð¡¾À½á"},
        {177, "ÅçÑª"},
        {178, "Ð±ÑÛÐ¦"},
        {179, "doge"},
        {180, "¾ªÏ²"},
        {181, "É§ÈÅ"},
        {182, "Ð¦¿Þ"},
        {183, "ÎÒ×îÃÀ"},
        {184, "ºÓÐ·"},
        {185, "ÑòÍÕ"},
        {187, "ÓÄÁé"},
        {188, "µ°"},
        {190, "¾Õ»¨"},
        {192, "ºì°ü"},
        {193, "´óÐ¦"},
        {194, "²»¿ªÐÄ"},
        {197, "ÀäÄ®"},
        {198, "ßÀ"},
        {199, "ºÃ°ô"},
        {200, "°ÝÍÐ"},
        {201, "µãÔÞ"},
        {202, "ÎÞÁÄ"},
        {203, "ÍÐÁ³"},
        {204, "³Ô"},
        {205, "ËÍ»¨"},
        {206, "º¦ÅÂ"},
        {207, "»¨³Õ"},
        {208, "Ð¡Ñù¶ù"},
        {210, "ì­Àá"},
        {211, "ÎÒ²»¿´"},
        {212, "ÍÐÈù"},
        {214, "à£à£"},
        {215, "ºýÁ³"},
        {216, "ÅÄÍ·"},
        {217, "³¶Ò»³¶"},
        {218, "ÌòÒ»Ìò"},
        {219, "²äÒ»²ä"},
        {220, "×§Õ¨Ìì"},
        {221, "¶¥ßÉßÉ"},
        {222, "±§±§"},
        {223, "±©»÷"},
        {224, "¿ªÇ¹"},
        {225, "ÁÃÒ»ÁÃ"},
        {226, "ÅÄ×À"},
        {227, "ÅÄÊÖ"},
        {228, "¹§Ï²"},
        {229, "¸É±­"},
        {230, "³°·í"},
        {231, "ºß"},
        {232, "·ðÏµ"},
        {233, "ÆþÒ»Æþ"},
        {234, "¾ª´ô"},
        {235, "²ü¶¶"},
        {236, "¿ÐÍ·"},
        {237, "Íµ¿´"},
        {238, "ÉÈÁ³"},
        {239, "Ô­ÁÂ"},
        {240, "ÅçÁ³"},
        {241, "ÉúÈÕ¿ìÀÖ"},
        {242, "Í·×²»÷"},
        {243, "Ë¦Í·"},
        {244, "ÈÓ¹·"},
        {245, "¼ÓÓÍ±ØÊ¤"},
        {246, "¼ÓÓÍ±§±§"},
        {247, "¿ÚÕÖ»¤Ìå"},
        {260, "°á×©ÖÐ"},
        {261, "Ã¦µ½·ÉÆð"},
        {262, "ÄÔÀ«ÌÛ"},
        {263, "²×É£"},
        {264, "ÎæÁ³"},
        {265, "À±ÑÛ¾¦"},
        {266, "Å¶Ó´"},
        {267, "Í·Íº"},
        {268, "ÎÊºÅÁ³"},
        {269, "°µÖÐ¹Û²ì"},
        {270, "emm"},
        {271, "³Ô¹Ï"},
        {272, "ºÇºÇßÕ"},
        {273, "ÎÒËáÁË"},
        {274, "Ì«ÄÏÁË"},
        {276, "À±½·½´"},
        {277, "ÍôÍô"},
        {278, "º¹"},
        {279, "´òÁ³"},
        {280, "»÷ÕÆ"},
        {281, "ÎÞÑÛÐ¦"},
        {282, "¾´Àñ"},
        {283, "¿ñÐ¦"},
        {284, "ÃæÎÞ±íÇé"},
        {285, "ÃþÓã"},
        {286, "Ä§¹íÐ¦"},
        {287, "Å¶"},
        {288, "Çë"},
        {289, "ÕöÑÛ"},
        {290, "ÇÃ¿ªÐÄ"},
        {291, "Õð¾ª"},
        {292, "ÈÃÎÒ¿µ¿µ"},
        {293, "Ãþ½õÀð"},
        {294, "ÆÚ´ý"},
        {295, "ÄÃµ½ºì°ü"},
        {296, "ÕæºÃ"},
        {297, "°ÝÐ»"},
        {298, "Ôª±¦"},
        {299, "Å£°¡"},
        {300, "ÅÖÈý½ï"},
        {301, "ºÃÉÁ"},
        {302, "×ó°ÝÄê"},
        {303, "ÓÒ°ÝÄê"},
        {304, "ºì°ü°ü"},
        {305, "ÓÒÇ×Ç×"},
        {306, "Å£Æø³åÌì"},
        {307, "ß÷ß÷"},
        {308, "Çóºì°ü"},
        {309, "Ð»ºì°ü"},
        {310, "ÐÂÄêÑÌ»¨"},
        {311, "´òcall"},
        {312, "±äÐÎ"},
        {313, "à¾µ½ÁË"},
        {314, "×ÐÏ¸·ÖÎö"},
        {315, "¼ÓÓÍ"},
        {316, "ÎÒÃ»ÊÂ"},
        {317, "²ËÍô"},
        {318, "³ç°Ý"},
        {319, "±ÈÐÄ"},
        {320, "Çì×£"},
        {321, "ÀÏÉ«Æ¦"},
        {322, "¾Ü¾ø"},
        {323, "ÏÓÆú"},
        {324, "³ÔÌÇ"},
        {325, "¾ªÏÅ"},
        {326, "ÉúÆø"},
        {327, "¼ÓÒ»"},
        {328, "´íºÅ"},
        {329, "¶ÔºÅ"},
        {330, "Íê³É"},
        {331, "Ã÷°×"}
};

enum MsgPacketType {
    MsgText         = 1,
    MsgFace         = 2,
    MsgGroupImage   = 3,
    MsgPrivateImage = 6,
    MsgVoice        = 7,
    MsgNickName     = 18,
    MsgVideo        = 26
};

struct MsgPacketHeader {
    std::uint8_t  type[8]{};                  // 8 ×Ö½Ú±êÊ¶
    std::uint32_t timeStamp{};                // 10 Î»³¤Ê±¼ä´Á
    std::uint32_t rand{};
    std::uint32_t color{};
    std::uint8_t  fontSize{};
    std::uint8_t  fontStyle{};
    std::uint8_t  charSet{};
    std::uint8_t  fontFamily{};

    // vvv offset 0x18
    std::uint16_t        fontNameLength{};
    std::vector<wchar_t> fontName;
    std::uint8_t         unknow[2]{};
};

struct MsgPacketContent {
    std::uint8_t              type{};
    std::uint16_t             len{};
    std::vector<std::uint8_t> data;

    MsgPacketContent(std::uint8_t *DataStart, std::size_t Count) {
        Buffer buffer{DataStart, Count};
        type = buffer.read<uint8_t>();
        len  = buffer.read<uint16_t>();
        data.resize(len, '\0');
        buffer.read(data.data(), len);
    }
};

std::string MsgPacketDecodeText(std::vector<uint8_t> &Data) {
    Buffer buffer{Data.data(), Data.size()};

    std::string text;
    while (!buffer.eof()) {
        MsgPacketContent msgPacketContent{buffer.getCurrentElement(), buffer.getMaxPos() - buffer.getCurrnetPos()};
        buffer.skip(1 + 2 + msgPacketContent.len);
        switch (msgPacketContent.type) {
            case 1: {
                std::wstring ws{(wchar_t *) msgPacketContent.data.data(), msgPacketContent.data.size() / 2};
                text = CodeCvt::WstrToStr(ws, CP_ACP);
                break;
            }
        }
    }
    return text;
}

std::string MsgPacketDecodeFace(std::vector<uint8_t> &Data) {
    Buffer buffer{Data.data(), Data.size()};

    while (!buffer.eof()) {
        MsgPacketContent msgPacketContent{buffer.getCurrentElement(), buffer.getMaxPos() - buffer.getCurrnetPos()};
        buffer.skip(1 + 2 + msgPacketContent.len);
        switch (msgPacketContent.type) {
            case 1: {
                std::size_t      id = 0;

                for (std::size_t i  = 0; i < msgPacketContent.len; ++i) {
                    id = (id << 8) | msgPacketContent.data.at(i);
                }

                auto it = g_MsgFaceMap.find(id);
                if (it != g_MsgFaceMap.end())
                    return it->second;
                break;
            }
        }
    }
    return {"Î´Öª"};
}

std::string MsgPacketDecodeNickName(std::vector<uint8_t> &Data) {
    Buffer      buffer{Data.data(), Data.size()};
    std::string nickName;

    while (!buffer.eof()) {
        MsgPacketContent msgPacketContent{Data.data(), Data.size()};
        buffer.skip(1 + 2 + msgPacketContent.len);
        switch (msgPacketContent.type) {
            case 1:
            case 2:
                std::wstring ws{(wchar_t *) msgPacketContent.data.data(), msgPacketContent.data.size() / 2};
                nickName = CodeCvt::WstrToStr(ws, CP_ACP);
                break;
        }
    }
    return nickName;
}

std::string MsgPacketDecodeImage(std::vector<uint8_t> &Data) {
    Buffer buffer{Data.data(), Data.size()};

    // hash with path
    std::pair<std::string, std::string> imageElement;
    while (!buffer.eof()) {
        MsgPacketContent msgPacketContent{buffer.getCurrentElement(), buffer.getMaxPos() - buffer.getCurrnetPos()};
        buffer.skip(1 + 2 + msgPacketContent.len);
        switch (msgPacketContent.type) {
            // case 0 unknow
            case 1:
                // save hex hash
                imageElement.first.resize(msgPacketContent.data.size() * 2, '\0');
                for (std::size_t i = 0; i < msgPacketContent.data.size(); ++i) {
                    char h = (char) (msgPacketContent.data[i] >> 4);
                    char l = (char) (msgPacketContent.data[i] & 0xF);
                    if (h < 10) {
                        h += '0';
                    } else {
                        h += 'A' - 10;
                    }
                    if (l < 10) {
                        l += '0';
                    } else {
                        l += 'A' - 10;
                    }
                    imageElement.first.at(i * 2)     = h;
                    imageElement.first.at(i * 2 + 1) = l;
                }
                break;
            case 2: {
                std::wstring ws{(wchar_t *) msgPacketContent.data.data(), msgPacketContent.data.size() / 2};
                imageElement.second = CodeCvt::WstrToStr(ws, CP_ACP);
                break;
            }
        }
    }

    std::string img;
    img.append(imageElement.first).append("\r\n").append(imageElement.second);
    return img;
}

std::string MsgPacketDecodeVoice(std::vector<uint8_t> &Data) {
    return {};
}

std::string MsgPacketDecodeVideo(std::vector<uint8_t> &Data) {
    return {};
}

struct MsgPacketParser {
    std::vector<std::pair<std::uint8_t, std::string>> decodeContent;

    MsgPacketParser(uint8_t *DataPtr, size_t Count) {
        try {
            Buffer          bufferReader{DataPtr, Count};
            /**
             *  Header ½âÎö
             */
            MsgPacketHeader msgPacket{};
            bufferReader.read<8>(msgPacket.type);
            msgPacket.timeStamp      = bufferReader.read<uint32_t>();
            msgPacket.rand           = bufferReader.read<uint32_t>();
            msgPacket.color          = bufferReader.read<uint32_t>();
            msgPacket.fontSize       = bufferReader.read<uint8_t>();
            msgPacket.fontStyle      = bufferReader.read<uint8_t>();
            msgPacket.charSet        = bufferReader.read<uint8_t>();
            msgPacket.fontFamily     = bufferReader.read<uint8_t>();
            msgPacket.fontNameLength = bufferReader.read<uint16_t>();
            msgPacket.fontName.resize(msgPacket.fontNameLength / 2 + 1, '\0');
            bufferReader.read((uint8_t *) msgPacket.fontName.data(), msgPacket.fontNameLength);
            bufferReader.skip(2);

            /**
             *  Content ½âÎö
             */
            while (!bufferReader.eof()) {
                MsgPacketContent msgPacketContent{bufferReader.getCurrentElement(), bufferReader.getMaxPos() - bufferReader.getCurrnetPos()};
                // skip type + len + data
                bufferReader.skip(1 + 2 + msgPacketContent.len);

                switch (msgPacketContent.type) {
                    case MsgPacketType::MsgText:
                        decodeContent.emplace_back(msgPacketContent.type, MsgPacketDecodeText(msgPacketContent.data));
                        break;
                    case MsgPacketType::MsgFace:
                        decodeContent.emplace_back(msgPacketContent.type, MsgPacketDecodeFace(msgPacketContent.data));
                        break;
                    case MsgPacketType::MsgGroupImage:
                    case MsgPacketType::MsgPrivateImage:
                        decodeContent.emplace_back(msgPacketContent.type, MsgPacketDecodeImage(msgPacketContent.data));
                        break;
                    case MsgPacketType::MsgVoice:
                        decodeContent.emplace_back(msgPacketContent.type, MsgPacketDecodeVoice(msgPacketContent.data));
                        break;
                    case MsgPacketType::MsgVideo:
                        decodeContent.emplace_back(msgPacketContent.type, MsgPacketDecodeVideo(msgPacketContent.data));
                        break;
                    case MsgPacketType::MsgNickName:
                        decodeContent.emplace_back(msgPacketContent.type, MsgPacketDecodeNickName(msgPacketContent.data));
                        break;
                }
                fmt::println("type: {}\nlen: {}", msgPacketContent.type, msgPacketContent.len);
            }
        } catch (std::exception &exception) {
            fmt::println("raise exception: {}", exception.what());
        }

        fmt::println("\n");
        for (auto &i: decodeContent) {
            fmt::println("type: {}\ndecode text: {}", i.first, i.second);
        }
    }
};


int main() {
    setbuf(stdout, nullptr);

    uint8_t data[] = {};

    MsgPacketParser msgPacketParser{data, sizeof(data) / sizeof(data[0])};

}