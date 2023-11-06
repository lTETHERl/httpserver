#include "httplib.h"
#include "sqlite3/sqlite3.h"
#include <unordered_map>

using namespace std;

httplib::Server svr;

sqlite3* db;
char* zErrMsg = 0;
int rc;
const char* sql;
sqlite3_stmt* stmt;

unordered_map<string, string> mp;

string select()
{
    string temp = "SELECT * FROM Students";

    sql = temp.c_str();

    string s = ": ";
    string to_res = "";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int columnCount = sqlite3_column_count(stmt);
            for (int i = 0; i < columnCount; i++) {
                to_res += static_cast<string>(sqlite3_column_name(stmt, i)) + s + static_cast<string>(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))) + "\n";
                mp[static_cast<string>(sqlite3_column_name(stmt, i))] = static_cast<string>(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            }
        }
        sqlite3_finalize(stmt);

    }
    return to_res;
}

string select(string name)
{
    string temp = "SELECT * FROM Students WHERE Name = '" + name + "'";

    sql = temp.c_str();

    string s = ": ";
    string to_res = "";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int columnCount = sqlite3_column_count(stmt);
            for (int i = 0; i < columnCount; i++) {
                to_res += static_cast<string>(sqlite3_column_name(stmt, i)) + s + static_cast<string>(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)))+"\n";
                mp[static_cast<string>(sqlite3_column_name(stmt, i))] = static_cast<string>(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            }
        }
        sqlite3_finalize(stmt);

    }
    return to_res;
}

void update(string name, string subject, string mark)
{
    string temp = "UPDATE Students SET " + subject + " = ? WHERE Name = ?";

    sql = temp.c_str();

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc == SQLITE_OK) {
        
        sqlite3_bind_int(stmt, 1, stoi(mark));
        sqlite3_bind_text(stmt, 2, name.c_str(), name.length(), SQLITE_STATIC);

        
        rc = sqlite3_step(stmt);
    }
}

void handle_select_request(const httplib::Request& req, httplib::Response& res)
{
    res.status = 200;

    string name = req.get_param_value("name");

    if (name != "")
    {
        res.set_content(select(name), "text/plain");
    }

    else
    {
        res.set_content(select(), "text/plain");
    }

    for (auto& x : mp)
    {
        res.set_header(x.first, x.second);
    }
}

void handle_update_request(const httplib::Request& req, httplib::Response& res)
{
    res.status = 200;

    string name = req.get_param_value("name");
    string subject = req.get_param_value("subject");
    string mark = req.get_param_value("mark");

    update(name, subject, mark);

}

int main() {

    rc = sqlite3_open("test.db", &db);
    
    svr.Get("/update", handle_update_request);

    svr.Get("/select", handle_select_request);

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Ўкольный ∆урнал", "text/plain");
        });

    svr.listen("0.0.0.0", 8080);

    return 0;
}