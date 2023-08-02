

#ifndef MSGDECRYPTER_SQLITEBASE_H
#define MSGDECRYPTER_SQLITEBASE_H

#include <string>
#include <sqlcipher/sqlite3.h>
#include <memory>
#include <any>

class SqliteBase {
private:
    sqlite3      *m_db       = nullptr;
    sqlite3_stmt *m_stmt     = nullptr;
    char         *m_errorMsg = nullptr;

    using dataType = std::vector<std::any>;
public:
    bool open(const std::string &DBPath) {
        return SQLITE_OK == sqlite3_open(DBPath.c_str(), &m_db);
    }

    bool exec(const std::string &SqlStatement, int (*Callback)(void *, int, char **, char **)) {
        sqlite3_free(m_errorMsg);
        return SQLITE_OK == sqlite3_exec(m_db, SqlStatement.c_str(), Callback, nullptr, &m_errorMsg);
    }

    bool prepare(const std::string &SqlStatement) {
        return SQLITE_OK == sqlite3_prepare_v2(m_db, SqlStatement.c_str(), -1, &m_stmt, nullptr);
    }

    std::vector<std::string> getColumnName() {
        auto                     columnCount = sqlite3_column_count(m_stmt);
        std::vector<std::string> columnNames;

        for (std::size_t i = 0; i < columnCount; ++i) {
            columnNames.emplace_back(sqlite3_column_name(m_stmt, i));
        }
        return columnNames;
    }

    dataType getData(const std::string &ColumnName) {
        auto columnCount = sqlite3_column_count(m_stmt);

        for (std::size_t i = 0; i < columnCount; ++i) {
            if (ColumnName == sqlite3_column_name(m_stmt, i)) {
                return getData(i);
            }
        }
        return {};
    }

    dataType getData(std::size_t ColumnIndex) {
        dataType datas;

        while (sqlite3_step(m_stmt) == SQLITE_ROW) {
            int type = sqlite3_column_type(m_stmt, ColumnIndex);
            switch (type) {
                case SQLITE_TEXT: {
                    std::size_t len  = sqlite3_column_bytes(m_stmt, ColumnIndex);
                    auto        *ptr = sqlite3_column_text(m_stmt, ColumnIndex);
                    datas.emplace_back(std::string{ptr, ptr + len});
                    break;
                }
                case SQLITE_BLOB: {
                    std::size_t len  = sqlite3_column_bytes(m_stmt, ColumnIndex);
                    auto        *ptr = sqlite3_column_text(m_stmt, ColumnIndex);
                    std::vector<uint8_t> vu{ptr, ptr + len};
                    datas.emplace_back(vu);
                    break;
                }
            }
        }
        if (SQLITE_OK == sqlite3_finalize(m_stmt))
            m_stmt = nullptr;
        return datas;
    }

    [[nodiscard]] const char *getErrorMsg() const {
        return m_errorMsg;
    }

    ~SqliteBase() {
        sqlite3_free(m_errorMsg);
        sqlite3_finalize(m_stmt);
        sqlite3_close(m_db);
    }

};


#endif //MSGDECRYPTER_SQLITEBASE_H
