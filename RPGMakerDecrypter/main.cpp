#include <filesystem>
#include <fstream>
#include <print>
#include <span>

#include "DecryptAlgo.hpp"


int DecryptDirectory(const std::filesystem::path &InputPath, const std::filesystem::path &OutputPath, std::string_view DecryptionKey) {
    auto enum_all_files = [](const std::filesystem::path &DirectoryPathCollection, std::vector<std::string_view> Filter) {
        std::vector<std::filesystem::path> files;
        for (const auto &entry: std::filesystem::recursive_directory_iterator(DirectoryPathCollection)) {
            if (entry.is_regular_file() && std::ranges::find(Filter, entry.path().extension()) != Filter.end()) {
                files.push_back(entry.path());
            }
        }
        return files;
    };
    auto read_file = [](const std::filesystem::path &FilePath) {
        std::ifstream file(FilePath, std::ios::binary | std::ios::in);
        file.seekg(0, std::ios::end);
        const auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> data(size);
        file.read(reinterpret_cast<char *>(data.data()), size);
        file.close();
        return data;
    };
    auto write_file = [](const std::filesystem::path &FilePath, std::span<uint8_t> Data) {
        std::ofstream file(FilePath, std::ios::binary | std::ios::out);
        file.write(reinterpret_cast<char *>(Data.data()), Data.size());
        file.close();
    };
    auto check_if_dir_exists = [](const std::filesystem::path &Path) {
        if (!std::filesystem::exists(Path)) {
            std::filesystem::create_directories(Path);
            // std::filesystem::create_directory(Path);
        }
    };

    const auto all_files = enum_all_files(InputPath, std::vector { std::string_view { ".rpgmvp" }, std::string_view { ".rpgmvo" } });
    std::println("encountered {} files", all_files.size());


    std::vector<uint8_t> DecryptionKeyBytes;
    for (int i = 0; i < DecryptionKey.size(); i += 2) {
        DecryptionKeyBytes.push_back(std::stol(std::string { DecryptionKey.substr(i, 2) }, nullptr, 16));
    }

    int successDecrypted = 0;
    for (const auto &file: all_files) {
        // 获取除了根目录的相对目录
        std::filesystem::path relative_path = std::filesystem::relative(file, InputPath);
        // 更改文件扩展名
        if (file.extension() == ".rpgmvp") {
            relative_path.replace_extension(".png");
        } else if (file.extension() == ".rpgmvo") {
            relative_path.replace_extension(".ogg");
        }

        std::filesystem::path output_path = OutputPath / relative_path;

        // 解密
        try {
            auto       data      = read_file(file);
            const auto decrypted = DecryptAlgo(std::span { data }, std::span { DecryptionKeyBytes });
            check_if_dir_exists(output_path.parent_path());
            write_file(output_path, decrypted);
            successDecrypted++;
        } catch (std::filesystem::filesystem_error &Exception) {
            std::println("fs error: {}", Exception.what());
        } catch (std::exception &Exception) {
            std::println("{}", Exception.what());
        }
    }

    std::println("successfully decrypted {}/{} files", successDecrypted, all_files.size());

    return 0;
}

int main(int argc, char *argv[]) {
    // disable output buffering
    setvbuf(stdout, nullptr, _IONBF, 0);

    // setting global encoding utf-8
    // std::locale::global(std::locale("zh_CN.UTF-8")); // if you needn't input Chinese characters, you can uncomment this line

    std::string_view input_dir;
    std::string_view out_dir;
    std::string_view decryption_key;

    if (argc < 4) {
        std::println("Usage: {} -i <input_dir> -o <output_dir> -k <decryption_key>", argv[0]);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (std::string_view { argv[i] } == "-i") {
            input_dir = argv[++i];
        } else if (std::string_view { argv[i] } == "-o") {
            out_dir = argv[++i];
        } else if (std::string_view { argv[i] } == "-k") {
            decryption_key = argv[++i];
        }
    }

    DecryptDirectory(input_dir, out_dir, decryption_key);
    return 0;
}
