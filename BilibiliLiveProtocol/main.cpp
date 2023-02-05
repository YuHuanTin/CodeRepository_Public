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

using std::string;
using std::wstring;
using std::unique_ptr;
using std::runtime_error;


enum WS_MSG_E {
    WS_OP_HEARTBEAT = 2,
    WS_OP_HEARTBEAT_REPLY = 3,
    WS_OP_MESSAGE = 5,
    WS_OP_USER_AUTHENTICATION = 7,
    WS_OP_CONNECT_SUCCESS = 8,
    WS_PACKAGE_HEADER_TOTAL_LENGTH = 16,
    WS_PACKAGE_OFFSET = 0,
    WS_HEADER_OFFSET = 4,
    WS_VERSION_OFFSET = 6,
    WS_OPERATION_OFFSET = 8,
    WS_SEQUENCE_OFFSET = 12,
    WS_BODY_PROTOCOL_VERSION_NORMAL = 0,
    WS_BODY_PROTOCOL_VERSION_BROTLI = 3,
    WS_HEADER_DEFAULT_VERSION = 1,
    WS_HEADER_DEFAULT_OPERATION = 1,
    WS_HEADER_DEFAULT_SEQUENCE = 1,
    WS_AUTH_OK = 0,
    WS_AUTH_TOKEN_ERROR = -101
};
struct Input {
    uint64_t UID = 0;
    uint64_t roomID = 0;
    uint64_t realRoomID = 0;
    uint64_t liveUpUID = 0;

    std::string danmuServerHost;
    std::string danmuServerPort;
    std::string danmuServerToken;
};
struct Env {
    bool debug = false;
    bool saveFile = false;
    FILE *fp = nullptr;
    ~Env(){
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

std::string decompress(const std::string &Data) {
    auto instance = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
    if (instance == nullptr)
        throw runtime_error("failed BrotliDecoderCreateInstance");

    std::string result;
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

void showDanmu(nlohmann::json &Json) {
    for (int i = 0; i < Json[0].size(); ++i) {
        if (Json[0].is_array()) {

            // 弹幕Json
            if (Json[0][i]["cmd"] == "DANMU_MSG") {
                std::stringstream ss;
                ss << "[" << Json[0][i]["info"][3][2] << ":" << Json[0][i]["info"][3][1] << "]" << Json[0][i]["info"][2][1] << "(uid: "
                   << Json[0][i]["info"][2][0] << "):" << Json[0][i]["info"][1] << '\n';

                std::string gbkStr = wstrToStr(strToWstr(ss.str(), CP_UTF8), CP_ACP);
                std::cout << gbkStr;

                if (gEnv.saveFile) {
                    fwrite(gbkStr.c_str(), 1, gbkStr.length(), gEnv.fp);
                    fflush(gEnv.fp);
                }
            }
        }
    }
}

class Protocol {
private:
    struct PackageHeaderT {
        uint32_t packageSize = 0; // 包括Header的长度

        uint16_t headerLen = 0;
        uint16_t protocolVersion = 0;

        uint32_t operation = 0;
        uint32_t sequenceId = 0;
    };
    uint64_t uid = 0;
    uint64_t realRoomID = 0;
    std::string token;
public:
    Protocol(uint64_t Uid, uint64_t RealRoomID, std::string Token) : uid(Uid), realRoomID(RealRoomID), token(std::move(Token)) {}

    std::string MakeAuthenticationPackage() {
        nlohmann::json nJson = {{"uid",      uid},
                                {"roomid",   realRoomID},
                                {"protover", 3},
                                {"platform", "web"},
                                {"type",     2},
                                {"key",      token}};
        return convertToPackage(to_string(nJson), WS_MSG_E::WS_OP_USER_AUTHENTICATION);
    }

    nlohmann::json convertToJson(const std::string &RecvBuf) {
        nlohmann::json nJson;
        for (uint32_t scanPos = 0; scanPos < RecvBuf.length();) {
            PackageHeaderT packageHeader;
            memcpy(&packageHeader, RecvBuf.c_str() + scanPos, WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH);
            packageHeader.packageSize = ntohl(packageHeader.packageSize);
            packageHeader.headerLen = ntohs(packageHeader.headerLen);
            packageHeader.protocolVersion = ntohs(packageHeader.protocolVersion);
            packageHeader.operation = ntohl(packageHeader.operation);
            packageHeader.sequenceId = ntohl(packageHeader.sequenceId);

            if (packageHeader.operation == WS_OP_HEARTBEAT_REPLY) {
                return {};
            }
            if (packageHeader.protocolVersion == WS_BODY_PROTOCOL_VERSION_BROTLI) {
                nJson += convertToJson(decompress({RecvBuf.c_str() + scanPos + WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                   packageHeader.packageSize - WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH}));
            } else {
                try {
                    nJson += nlohmann::json::parse(std::string {RecvBuf.c_str() + scanPos + WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH,
                                                                packageHeader.packageSize - WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH});
                } catch (nlohmann::json::parse_error &error) {
                    std::cout << "Json parse error: " << error.what() << '\n';
                }
            }
            scanPos += packageHeader.packageSize;
        }
        return nJson;
    }

    std::string convertToPackage(const std::string &Str, WS_MSG_E WsMsg) {
        uint32_t totalSize = WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH + Str.length();
        PackageHeaderT packageHeader;
        packageHeader.packageSize = htonl(totalSize);
        packageHeader.headerLen = htons(16);
        packageHeader.protocolVersion = htons(1);
        packageHeader.operation = htonl((uint32_t) WsMsg);
        packageHeader.sequenceId = htonl(1);

        std::string str(totalSize, 0);
        memcpy((void *) str.c_str(), (void *) &packageHeader, WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH);
        memcpy((void *) (str.c_str() + WS_MSG_E::WS_PACKAGE_HEADER_TOTAL_LENGTH), Str.c_str(), Str.length());
        return str;
    }
};

std::string requestURL(const std::string &Url) {
    HttpRequestT httpRequest {Url, "get"};
    HttpResponseT httpResponse;
    WinhttpAPI api(httpRequest, httpResponse);
    api.Request();
    return httpResponse.Body;
}

int main() {
    srand(time(nullptr));
    setbuf(stdout, nullptr);

    nlohmann::json nJson;
    Input input;
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
            std::string responseBody = requestURL("https://api.live.bilibili.com/room/v1/Room/room_init?id=" + std::to_string(input.roomID));
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

            if (!ix::initNetSystem())
                throw runtime_error("failed initNetSystem");

            ix::WebSocket webSocket;
            webSocket.setUrl("wss://" + input.danmuServerHost + ":" + input.danmuServerPort + "/sub");

            webSocket.setOnMessageCallback([&danmuProtocol, &webSocket](const ix::WebSocketMessagePtr &msg) {
                if (msg->type == ix::WebSocketMessageType::Message) {
                    nlohmann::json json = danmuProtocol.convertToJson(msg->str);
                    showDanmu(json);
                } else if (msg->type == ix::WebSocketMessageType::Open) {
                    if (!webSocket.sendBinary(danmuProtocol.MakeAuthenticationPackage()).success)
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
                        if (!webSocket.sendBinary(danmuProtocol.convertToPackage("[object Object]", WS_OP_HEARTBEAT)).success)
                            throw runtime_error("failed send heart package");
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                    } else if (status == ix::ReadyState::Closed) {
                        break;
                    }
                }
            }).join();

            webSocket.stop();
        }

    } catch (runtime_error &error) {
        std::cout << "runtime error: " << error.what() << '\n';
    } catch (nlohmann::json::parse_error &error) {
        std::cout << "json parse error: " << error.what() << '\n';
    }
    return 0;
}
