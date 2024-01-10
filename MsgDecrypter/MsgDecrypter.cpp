
#include <stdexcept>
#include <fmt/format.h>
#include <vector>
#include "SqliteUtils/SqliteBase.h"
#include "Parser/Parser.h"

int SqliteDataReader(void *, int Count, char **ColumnValues, char **ColumnNames) {
    for (int i = 0; i < Count; i++) {
        printf("%s = %s\n", ColumnNames[i], ColumnValues[i]);
    }
    return 0;
}

auto getAllTable(SqliteBase &Base) {
    if (!Base.prepare("select name from sqlite_master;")) {
        throw std::runtime_error(std::string(__FUNCTION__) + "\tprepare sql statement failed");
    }
    auto result = Base.getData("name");

    std::vector<std::string> tableNames(result.size());

    for (std::size_t i = 0; i < result.size(); ++i) {
        if (result.at(i).type() == typeid(std::string)) {
            tableNames[i] = std::move(std::any_cast<std::string>(result.at(i)));
        }
    }
    return tableNames;
}

int main() {
    setbuf(stdout, nullptr);

    std::vector<std::string> sqlStatement;
    try {
        /**
         *  �ռ����б����Զ�ȡ����
         */
        SqliteBase sqliteBase;
        if (!sqliteBase.open(u8"D:\\Msg3.0.db")) {
            throw std::runtime_error(std::string(__FUNCTION__) + "\topen database failed");
        }
        for (const auto &one: getAllTable(sqliteBase)) {
            sqlStatement.emplace_back("select * from " + one + ";");
        }

        /**
         *  �ռ�����������
         */
        std::vector<std::vector<std::uint8_t>> encodeData;

        for (const auto &i: sqlStatement) {
            // ����ÿ�ű��ܴ򿪵�
            if (!sqliteBase.prepare(i)) {
                continue;
            }
            // ���Ϊ vector<vector<uint8_t>>
            auto temp = sqliteBase.getData("MsgContent");

            for (auto &j: temp) {
                encodeData.emplace_back(std::any_cast<std::vector<std::uint8_t>>(j));
            }
        }

        /**
         *  ��������
         */
        Parser parser;
        size_t cnt = 0;

        for (auto &i: encodeData) {
            auto decodeData = parser.doParser(i);
            parser.clear();

            for (auto &j: decodeData) {
                switch (j.first) {
                    case MsgPacketType::MsgNickName:
                        fmt::println("[{}]", j.second);
                        break;
                    case MsgPacketType::MsgText:
                        fmt::println("{}", j.second);
                        break;
                    case MsgPacketType::MsgFace:
                        fmt::println("face: {}", j.second);
                        break;
                    case MsgPacketType::MsgPrivateImage:
                    case MsgPacketType::MsgGroupImage:
                        fmt::println("img: {}", j.second);
                        break;
                }
            }
            ++cnt;
        }
        fmt::println("count: {}", cnt);
    } catch (std::exception &exception) {
        fmt::println("a exception occur: {}", exception.what());
    }


}