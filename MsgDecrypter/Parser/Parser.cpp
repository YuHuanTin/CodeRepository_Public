

#include "Parser.h"
#include "../Utils/Buffer.h"
#include "../Utils/CodeCvt.h"

std::map<std::size_t, std::string> MsgFaceMap{
        {0,   "����"},
        {1,   "Ʋ��"},
        {2,   "ɫ"},
        {3,   "����"},
        {4,   "����"},
        {5,   "����"},
        {6,   "����"},
        {7,   "����"},
        {8,   "˯"},
        {9,   "���"},
        {10,  "����"},
        {11,  "��ŭ"},
        {12,  "��Ƥ"},
        {13,  "����"},
        {14,  "΢Ц"},
        {15,  "�ѹ�"},
        {16,  "��"},
        {18,  "ץ��"},
        {19,  "��"},
        {20,  "͵Ц"},
        {21,  "�ɰ�"},
        {22,  "����"},
        {23,  "����"},
        {24,  "����"},
        {25,  "��"},
        {26,  "����"},
        {27,  "����"},
        {28,  "��Ц"},
        {29,  "����"},
        {30,  "�ܶ�"},
        {31,  "����"},
        {32,  "����"},
        {33,  "��"},
        {34,  "��"},
        {35,  "��ĥ"},
        {36,  "˥"},
        {37,  "����"},
        {38,  "�ô�"},
        {39,  "�ټ�"},
        {41,  "����"},
        {42,  "����"},
        {43,  "����"},
        {46,  "��ͷ"},
        {49,  "ӵ��"},
        {53,  "����"},
        {54,  "����"},
        {55,  "ը��"},
        {56,  "��"},
        {57,  "����"},
        {59,  "���"},
        {60,  "����"},
        {61,  "��"},
        {63,  "õ��"},
        {64,  "��л"},
        {66,  "����"},
        {67,  "����"},
        {69,  "����"},
        {74,  "̫��"},
        {75,  "����"},
        {76,  "��"},
        {77,  "��"},
        {78,  "����"},
        {79,  "ʤ��"},
        {85,  "����"},
        {86,  "���"},
        {89,  "����"},
        {96,  "�亹"},
        {97,  "����"},
        {98,  "�ٱ�"},
        {99,  "����"},
        {100, "�ܴ���"},
        {101, "��Ц"},
        {102, "��ߺ�"},
        {103, "�Һߺ�"},
        {104, "��Ƿ"},
        {105, "����"},
        {106, "ί��"},
        {107, "�����"},
        {108, "����"},
        {109, "������"},
        {110, "��"},
        {111, "����"},
        {112, "�˵�"},
        {113, "ơ��"},
        {114, "����"},
        {115, "ƹ��"},
        {116, "ʾ��"},
        {117, "ư��"},
        {118, "��ȭ"},
        {119, "����"},
        {120, "ȭͷ"},
        {121, "�"},
        {122, "����"},
        {123, "NO"},
        {124, "OK"},
        {125, "תȦ"},
        {126, "��ͷ"},
        {127, "��ͷ"},
        {128, "����"},
        {129, "����"},
        {130, "����"},
        {131, "����"},
        {132, "����"},
        {133, "��̫��"},
        {134, "��̫��"},
        {136, "˫ϲ"},
        {137, "����"},
        {138, "����"},
        {140, "K��"},
        {144, "�Ȳ�"},
        {145, "��"},
        {146, "����"},
        {147, "������"},
        {148, "����"},
        {151, "�ɻ�"},
        {158, "��Ʊ"},
        {168, "ҩ"},
        {169, "��ǹ"},
        {171, "��"},
        {172, "գ�۾�"},
        {173, "�ᱼ"},
        {174, "����"},
        {175, "����"},
        {176, "С����"},
        {177, "��Ѫ"},
        {178, "б��Ц"},
        {179, "doge"},
        {180, "��ϲ"},
        {181, "ɧ��"},
        {182, "Ц��"},
        {183, "������"},
        {184, "��з"},
        {185, "����"},
        {187, "����"},
        {188, "��"},
        {190, "�ջ�"},
        {192, "���"},
        {193, "��Ц"},
        {194, "������"},
        {197, "��Į"},
        {198, "��"},
        {199, "�ð�"},
        {200, "����"},
        {201, "����"},
        {202, "����"},
        {203, "����"},
        {204, "��"},
        {205, "�ͻ�"},
        {206, "����"},
        {207, "����"},
        {208, "С����"},
        {210, "���"},
        {211, "�Ҳ���"},
        {212, "����"},
        {214, "��"},
        {215, "����"},
        {216, "��ͷ"},
        {217, "��һ��"},
        {218, "��һ��"},
        {219, "��һ��"},
        {220, "קը��"},
        {221, "������"},
        {222, "����"},
        {223, "����"},
        {224, "��ǹ"},
        {225, "��һ��"},
        {226, "����"},
        {227, "����"},
        {228, "��ϲ"},
        {229, "�ɱ�"},
        {230, "����"},
        {231, "��"},
        {232, "��ϵ"},
        {233, "��һ��"},
        {234, "����"},
        {235, "����"},
        {236, "��ͷ"},
        {237, "͵��"},
        {238, "����"},
        {239, "ԭ��"},
        {240, "����"},
        {241, "���տ���"},
        {242, "ͷײ��"},
        {243, "˦ͷ"},
        {244, "�ӹ�"},
        {245, "���ͱ�ʤ"},
        {246, "���ͱ���"},
        {247, "���ֻ���"},
        {260, "��ש��"},
        {261, "æ������"},
        {262, "������"},
        {263, "��ɣ"},
        {264, "����"},
        {265, "���۾�"},
        {266, "ŶӴ"},
        {267, "ͷͺ"},
        {268, "�ʺ���"},
        {269, "���й۲�"},
        {270, "emm"},
        {271, "�Թ�"},
        {272, "�Ǻ���"},
        {273, "������"},
        {274, "̫����"},
        {276, "������"},
        {277, "����"},
        {278, "��"},
        {279, "����"},
        {280, "����"},
        {281, "����Ц"},
        {282, "����"},
        {283, "��Ц"},
        {284, "���ޱ���"},
        {285, "����"},
        {286, "ħ��Ц"},
        {287, "Ŷ"},
        {288, "��"},
        {289, "����"},
        {290, "�ÿ���"},
        {291, "��"},
        {292, "���ҿ���"},
        {293, "������"},
        {294, "�ڴ�"},
        {295, "�õ����"},
        {296, "���"},
        {297, "��л"},
        {298, "Ԫ��"},
        {299, "ţ��"},
        {300, "������"},
        {301, "����"},
        {302, "�����"},
        {303, "�Ұ���"},
        {304, "�����"},
        {305, "������"},
        {306, "ţ������"},
        {307, "����"},
        {308, "����"},
        {309, "л���"},
        {310, "�����̻�"},
        {311, "��call"},
        {312, "����"},
        {313, "ྵ���"},
        {314, "��ϸ����"},
        {315, "����"},
        {316, "��û��"},
        {317, "����"},
        {318, "���"},
        {319, "����"},
        {320, "��ף"},
        {321, "��ɫƦ"},
        {322, "�ܾ�"},
        {323, "����"},
        {324, "����"},
        {325, "����"},
        {326, "����"},
        {327, "��һ"},
        {328, "���"},
        {329, "�Ժ�"},
        {330, "���"},
        {331, "����"}
};

struct MsgPacketHeader {
    std::uint8_t  type[8]{};                  // 8 �ֽڱ�ʶ
    std::uint32_t timeStamp{};                // 10 λ��ʱ���
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
                std::size_t id = 0;

                for (std::size_t i = 0; i < msgPacketContent.len; ++i) {
                    id = (id << 8) | msgPacketContent.data.at(i);
                }

                auto it = MsgFaceMap.find(id);
                if (it != MsgFaceMap.end())
                    return it->second;
                break;
            }
        }
    }
    return {"δ֪"};
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

Parser::returnType Parser::doParser(std::vector<uint8_t> &BytesData) {
    Buffer          bufferReader{BytesData.data(), BytesData.size()};
    /**
     *  Header ����
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
    msgPacket.fontName.resize(msgPacket.fontNameLength / 2, '\0');
    bufferReader.read((uint8_t *) msgPacket.fontName.data(), msgPacket.fontNameLength);
    bufferReader.skip(2);

    /**
     *  Content ����
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
            default:
                // �����͵���Ϣ���ܱ�����,���ظ����ݳ���
                decodeContent.emplace_back(msgPacketContent.type, std::to_string(msgPacketContent.len));
                break;
        }
    }
    return decodeContent;
}

void Parser::clear() {
    decodeContent.clear();
}
