//
// Created by AFETT on 2024/7/25.
//
#ifndef DECRYPTALGO_HPP
#define DECRYPTALGO_HPP


#include <algorithm>
#include <span>

/**
 * 解密算法
 * @param CryptedData 加密的数据
 * @param DecryptionKey 解密的密钥（16字节）
 * @return 解密后的数据，失败返回空
 */
inline std::span<uint8_t> DecryptAlgo(std::span<uint8_t> CryptedData, std::span<uint8_t> DecryptionKey) {
    constexpr int              headerlength = 16;
    constexpr std::string_view SIGNATURE { "\x52\x50\x47\x4d\x56\x00\x00\x00", 8 };
    constexpr std::string_view VER { "\x00\x03\x01", 3 };
    constexpr std::string_view REMAIN { "\x00\x00\x00\x00\x00", 5 };

    if (CryptedData.empty() || CryptedData.size() < headerlength) {
        return {};
    }

    /// check header
    std::span<uint8_t> header = CryptedData.subspan(0, headerlength);
    std::string        ref;
    ref.append(SIGNATURE).append(VER).append(REMAIN);
    if (!std::ranges::equal(header, ref)) {
        throw std::runtime_error("Header is wrong");
    }

    /// cut header
    auto body = CryptedData.subspan(headerlength);
    if (body.empty()) {
        return {};
    }

    /// decrypt
    for (int i = 0; i < headerlength; i++) {
        body[i] = body[i] ^ DecryptionKey[i];
    }
    return CryptedData.subspan(headerlength);
}
#endif // DECRYPTALGO_HPP
