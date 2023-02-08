#include <iostream>
#include <thread>
#include <brotli/decode.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <WinhttpAPI.h>
#include "json.hpp"
#include <memory>
#include <string>
#include <windows.h>
#include <sstream>

using std::string;
using std::wstring;
using std::unique_ptr;
using std::runtime_error;

enum WS_MsgType_E {
    WS_OP_HEARTBEAT = 2,
    WS_OP_HEARTBEAT_REPLY = 3,
    WS_OP_MESSAGE = 5,
    WS_OP_USER_AUTHENTICATION = 7,
    WS_OP_CONNECT_SUCCESS = 8,
    WS_PACKAGE_HEADER_TOTAL_LENGTH = 16,
    WS_BODY_PROTOCOL_VERSION_NORMAL = 0,
    WS_BODY_PROTOCOL_VERSION_BROTLI = 3,
    WS_HEADER_DEFAULT_VERSION = 1,
    WS_HEADER_DEFAULT_SEQUENCE = 1,
    WS_AUTH_OK = 0,
    WS_AUTH_TOKEN_ERROR = -101
};

struct InputT {
    uint64_t UID = 0;
    uint64_t roomID = 0;
    uint64_t realRoomID = 0;
    uint64_t liveUpUID = 0;

    string danmuServerHost;
    string danmuServerPort;
    string danmuServerToken;
};

struct EnvT {
    bool debug = false;
    bool saveFile = false;
    FILE *fp = nullptr;

    ~EnvT() {
        fclose(fp);
    }
} gEnv;

string wstrToStr(const wstring &Src, UINT CodePage) {
    if (Src.empty()) return {};
    int len = WideCharToMultiByte(CodePage, 0, Src.c_str(), (int) Src.length(), nullptr, 0, nullptr, nullptr);
    if (len <= 0) throw std::runtime_error("len <= 0");
    string szResult(len, 0);
    WideCharToMultiByte(CodePage, 0, Src.c_str(), (int) Src.length(), &szResult.at(0), len, nullptr, nullptr);
    return szResult;
}

wstring strToWstr(const string &Src, UINT CodePage) {
    if (Src.empty()) return {};
    int len = MultiByteToWideChar(CodePage, 0, Src.c_str(), (int) Src.length(), nullptr, 0);
    if (len <= 0) throw std::runtime_error("len <= 0");
    wstring wszResult(len, 0);
    MultiByteToWideChar(CodePage, 0, Src.c_str(), (int) Src.length(), &wszResult.at(0), len);
    return wszResult;
}

string decompress(const string &Data) {
    auto instance = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
    if (instance == nullptr)
        throw runtime_error("failed BrotliDecoderCreateInstance");

    string result;
    std::array<uint8_t, 4096> buf {};
    size_t availableDataLen = Data.length(), availableBufLen = buf.size();
    auto *pData = (const uint8_t *) Data.c_str();
    uint8_t *pBuf = buf.data();
    BrotliDecoderResult decoderResult;
    do {
        decoderResult = BrotliDecoderDecompressStream(instance, &availableDataLen, &pData, &availableBufLen, &pBuf, nullptr);
        result.append((char *) buf.data(), buf.size() - availableBufLen);
        availableBufLen = buf.size();
        pBuf = buf.data();
    } while (availableDataLen != 0 || decoderResult == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT);

    BrotliDecoderDestroyInstance(instance);
    return result;
}

string requestURL(const string &Url) {
    HttpRequestT httpRequest {Url, "get"};
    HttpResponseT httpResponse;
    WinhttpAPI api(httpRequest, httpResponse);
    api.Request();
    return httpResponse.Body;
}

void showDanmu(std::vector<nlohmann::json> &vJson) {

    for (int i = 0; i < vJson.size(); ++i) {

        if (vJson[i].contains("/cmd"_json_pointer)) {
            if (vJson[i].at("/cmd"_json_pointer) == "DANMU_MSG") {
                std::stringstream ss;
                string guardType[] = {{u8"观众"},
                                      {u8"总督"},
                                      {u8"提督"},
                                      {u8"舰长"}};

                string msg = vJson[i].at("/info/1"_json_pointer).is_null() ? "" : vJson[i].at("/info/1"_json_pointer);
                string userName = vJson[i].at("/info/2/1"_json_pointer);
                int64_t userID = vJson[i].at("/info/2/0"_json_pointer);

                string fansMedalLabel = u8"未佩戴牌子";
                string fansMedalAnchorUsername = u8"未佩戴牌子";
                uint32_t fansMedalLevel = 0;
                uint32_t guardTypeIndex = 0;
                uint64_t fansMedalAnchorId = 0;
                if (!vJson[i].at("/info/3"_json_pointer).empty()) {
                    fansMedalLabel = vJson[i].at("/info/3/1"_json_pointer);
                    fansMedalLevel = vJson[i].at("/info/3/0"_json_pointer);
                    fansMedalAnchorUsername = vJson[i].at("/info/3/2"_json_pointer);
                    fansMedalAnchorId = vJson[i].at("/info/3/12"_json_pointer);
                    guardTypeIndex = vJson[i].at("/info/3/10"_json_pointer);
                }
                if (gEnv.debug) {
                    ss << "[" << fansMedalAnchorUsername << "(uid:" << fansMedalAnchorId << ")]<" << fansMedalLabel << " " << fansMedalLevel << ">["
                       << guardType[guardTypeIndex] << "] " << userName << "(uid:" << userID << ") : " << msg;
                } else {
                    ss << "<" << fansMedalLabel << " " << fansMedalLevel << ">[" << guardType[guardTypeIndex] << "] " << userName << "(uid:" << userID << ") : "
                       << msg;
                }
                std::cout << wstrToStr(strToWstr(ss.str(), CP_UTF8), CP_ACP) << '\n';
                if (gEnv.saveFile) {
                    fwrite(ss.str().c_str(), 1, ss.str().length(), gEnv.fp);
                    fflush(gEnv.fp);
                }
            }
        } else if (vJson[i].contains("/popularValue"_json_pointer)) {
            std::cout << "人气值: " << vJson[i]["popularValue"] << '\n';
        } else if (vJson[i].contains("/code"_json_pointer)) {
            if (vJson[i]["code"] == WS_AUTH_OK)
                std::cout << "连接成功\n";
            else if (vJson[i]["code"] == WS_AUTH_TOKEN_ERROR)
                throw runtime_error("Token错误");
            else throw runtime_error("未知错误");
        } else throw runtime_error("未知分支");

    }

}

class Protocol {
private:
    struct PackageHeaderT {
        uint32_t totalSize = 0; // 包括Header的长度

        uint16_t headerLen = 0;
        uint16_t protocolVersion = 0;

        uint32_t operation = 0;
        uint32_t sequenceId = 0;
    };
    uint64_t uid = 0;
    uint64_t realRoomID = 0;
    string token;
public:
    Protocol(uint64_t Uid, uint64_t RealRoomID, string Token) : uid(Uid), realRoomID(RealRoomID), token(std::move(Token)) {}

    PackageHeaderT parsePackageHeader(const char *Buf) {
        PackageHeaderT packageHeader;
        memcpy(&packageHeader, Buf, WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH);
        packageHeader.totalSize = ntohl(packageHeader.totalSize);
        packageHeader.headerLen = ntohs(packageHeader.headerLen);
        packageHeader.protocolVersion = ntohs(packageHeader.protocolVersion);
        packageHeader.operation = ntohl(packageHeader.operation);
        packageHeader.sequenceId = ntohl(packageHeader.sequenceId);
        return packageHeader;
    }

    std::vector<nlohmann::json> parsePackageToJson(const string &Buf) {
        std::vector<nlohmann::json> vJson;
        for (uint32_t scanPos = 0; scanPos < Buf.length();) {
            PackageHeaderT packageHeader = parsePackageHeader(Buf.c_str() + scanPos);
            if (packageHeader.operation == WS_OP_CONNECT_SUCCESS) {
                vJson.push_back(nlohmann::json::parse(string {Buf.c_str() + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                              packageHeader.totalSize - WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH}));
            } else if (packageHeader.operation == WS_OP_HEARTBEAT_REPLY) {
                // 心跳包返回人气值
                uint32_t popularValue = *(uint32_t *) (Buf.c_str() + WS_PACKAGE_HEADER_TOTAL_LENGTH);
                return {{{"popularValue", ntohl(popularValue)}}};
            } else if (packageHeader.operation == WS_OP_MESSAGE) {
                try {
                    if (packageHeader.protocolVersion == WS_BODY_PROTOCOL_VERSION_BROTLI) {
                        vJson = parsePackageToJson(decompress({Buf.c_str() + scanPos + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                               packageHeader.totalSize - WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH}));
                    } else if (packageHeader.protocolVersion == WS_BODY_PROTOCOL_VERSION_NORMAL) {
                        vJson.push_back(nlohmann::json::parse(string {Buf.c_str() + scanPos + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                                      packageHeader.totalSize - WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH}));
                    }
                } catch (runtime_error &error) {
                    std::cout << "runtime error: " << error.what() << '\n';
                } catch (nlohmann::json::parse_error &error) {
                    std::cout << "Json parse error: " << error.what() << '\n';
                }
            }
            scanPos += packageHeader.totalSize;
        }
        return vJson;
    }

    nlohmann::json parsePackageToJson2(const string &Buf) {
        nlohmann::json nJson;
        for (uint32_t scanPos = 0; scanPos < Buf.length();) {
            PackageHeaderT packageHeader = parsePackageHeader(Buf.c_str() + scanPos);
            if (packageHeader.operation == WS_OP_CONNECT_SUCCESS) {
                nJson = nlohmann::json::parse(string {Buf.c_str() + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                      packageHeader.totalSize - WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH});
            } else if (packageHeader.operation == WS_OP_HEARTBEAT_REPLY) {
                // 心跳包返回人气值
                uint32_t popularValue = *(uint32_t *) (Buf.c_str() + WS_PACKAGE_HEADER_TOTAL_LENGTH);
                return {{"popularValue", ntohl(popularValue)}};
            } else if (packageHeader.operation == WS_OP_MESSAGE) {
                try {
                    if (packageHeader.protocolVersion == WS_BODY_PROTOCOL_VERSION_BROTLI) {
                        nJson += parsePackageToJson(decompress({Buf.c_str() + scanPos + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                                packageHeader.totalSize - WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH}));
                    } else if (packageHeader.protocolVersion == WS_BODY_PROTOCOL_VERSION_NORMAL) {
                        nJson += nlohmann::json::parse(string {Buf.c_str() + scanPos + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                               packageHeader.totalSize - WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH});
                    }
                } catch (runtime_error &error) {
                    std::cout << "runtime error: " << error.what() << '\n';
                } catch (nlohmann::json::parse_error &error) {
                    std::cout << "Json parse error: " << error.what() << '\n';
                }
            }
            scanPos += packageHeader.totalSize;
        }
        return nJson;
    }

    string makePackage(const string &Buf, WS_MsgType_E WsMsg) {
        uint32_t totalSize = WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH + Buf.length();
        PackageHeaderT packageHeader;
        packageHeader.totalSize = htonl(totalSize);
        packageHeader.headerLen = htons(WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH);
        packageHeader.protocolVersion = htons(WS_HEADER_DEFAULT_VERSION);
        packageHeader.operation = htonl((uint32_t) WsMsg);
        packageHeader.sequenceId = htonl(WS_HEADER_DEFAULT_SEQUENCE);

        string str(totalSize, 0);
        memcpy((void *) str.c_str(), (void *) &packageHeader, WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH);
        memcpy((void *) (str.c_str() + WS_MsgType_E::WS_PACKAGE_HEADER_TOTAL_LENGTH), Buf.c_str(), Buf.length());
        return str;
    }

    string makeAuthenticationPackage() {
        nlohmann::json nJson = {{"uid",      uid},
                                {"roomid",   realRoomID},
                                {"protover", 3},
                                {"platform", "web"},
                                {"type",     2},
                                {"key",      token}};
        return makePackage(to_string(nJson), WS_MsgType_E::WS_OP_USER_AUTHENTICATION);
    }
};

int main() {
    srand(time(nullptr));
    setbuf(stdout, nullptr);

    nlohmann::json nJson;
    InputT input;
    try {
        // 获取信息
        {
            std::cout << "请输入你的UID:\n";
            std::cin >> input.UID;
            std::cout << "请输入房间号:\n";
            std::cin >> input.roomID;
            std::cout << "是否保存弹幕文件(与输出一致 1/0):\n";
            std::cin >> gEnv.saveFile;
            if (gEnv.saveFile) {
                fopen_s(&gEnv.fp, "danmu.txt", "a+");
                if (gEnv.fp == nullptr) throw runtime_error("failed open file");
            }
            std::cout << "是否调试模式(1/0):\n";
            std::cin >> gEnv.debug;
            if (gEnv.debug) std::cout << "debug on\n";
        }

        // 获取真实房间号
        {
            string responseBody = requestURL("https://api.live.bilibili.com/room/v1/Room/room_init?id=" + std::to_string(input.roomID));
            nJson = nlohmann::json::parse(responseBody);
            input.realRoomID = nJson["data"]["room_id"];
            input.liveUpUID = nJson["data"]["uid"];

            if (gEnv.debug)
                std::cout << "房间真实id号: " << input.realRoomID << "\n开播up uid: " << input.liveUpUID << '\n';
        }

        // 随机选取一个弹幕服务器
        {
            nJson = nlohmann::json::parse(
                    requestURL("https://api.live.bilibili.com/xlive/web-room/v1/index/getDanmuInfo?id=" + std::to_string(input.realRoomID) + "&type=0"));
            size_t index = rand() % nJson["data"]["host_list"].size();
            input.danmuServerHost = nJson["data"]["host_list"][index]["host"];
            input.danmuServerPort = to_string(nJson["data"]["host_list"][index]["wss_port"]);
            input.danmuServerToken = nJson["data"]["token"];

            if (gEnv.debug)
                std::cout << "选取弹幕服务器: " << input.danmuServerHost << ":" << input.danmuServerPort << '\n';
        }

        // websocket part
        {
            Protocol danmuProtocol(input.UID, input.realRoomID, input.danmuServerToken);

            if (!ix::initNetSystem()) throw runtime_error("failed initNetSystem");

            ix::WebSocket webSocket;
            webSocket.setUrl("wss://" + input.danmuServerHost + ":" + input.danmuServerPort + "/sub");

            webSocket.setOnMessageCallback([&danmuProtocol, &webSocket](const ix::WebSocketMessagePtr &msg) {
                if (msg->type == ix::WebSocketMessageType::Message) {
                    try {
                        std::vector<nlohmann::json> vJson = danmuProtocol.parsePackageToJson(msg->str);
                        showDanmu(vJson);
                    } catch (runtime_error &error) {
                        std::cout << "runtime error: " << error.what() << '\n';
                    } catch (nlohmann::json::parse_error &error) {
                        std::cout << "Json parse error: " << error.what() << '\n';
                    } catch (nlohmann::json::exception &exception) {
                        std::cout << "Json exception error: " << exception.what() << '\n';
                    }
                } else if (msg->type == ix::WebSocketMessageType::Open) {
                    if (!webSocket.sendBinary(danmuProtocol.makeAuthenticationPackage()).success)
                        throw runtime_error("failed send auth package");
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    std::cout << "Connection error: " << msg->errorInfo.reason << std::endl;
                    webSocket.close();
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    std::cout << "WebSocket Close, code: " << msg->closeInfo.code << "close msg: " << msg->closeInfo.reason << '\n';
                }
            });
            webSocket.start();
            std::thread([&webSocket, &danmuProtocol] {
                while (true) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    auto status = webSocket.getReadyState();
                    if (status == ix::ReadyState::Open) {
                        if (!webSocket.sendBinary(danmuProtocol.makePackage("[object Object]", WS_OP_HEARTBEAT)).success)
                            throw runtime_error("failed send heart package");
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                    } else if (status == ix::ReadyState::Closed) {
                        break;
                    }
                }
            }).join();

            webSocket.stop();
            ix::uninitNetSystem();
        }
    } catch (runtime_error &error) {
        std::cout << "runtime error: " << error.what() << '\n';
    } catch (nlohmann::json::parse_error &error) {
        std::cout << "Json parse error: " << error.what() << '\n';
    }
    return 0;
}
