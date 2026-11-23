#include "./operations_routes.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace {

std::string utc_now_iso8601() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const std::time_t t = system_clock::to_time_t(now);
    std::tm tm_utc{};
#if defined(_WIN32)
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string escape_json(const std::string& s) {
    std::ostringstream out;
    for (char c : s) {
        switch (c) {
            case '"':  out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n";  break;
            case '\r': out << "\\r";  break;
            case '\t': out << "\\t";  break;
            default:   out << c;      break;
        }
    }
    return out.str();
}

// Простейший поиск строкового поля в заглушке (позже заменим на полноценный JSON-парсер)
std::string extract_string_field(const std::string& json_text, const std::string& key, const std::string& fallback) {
    const std::string pattern = "\"" + key + "\"";
    const auto key_pos = json_text.find(pattern);
    if (key_pos == std::string::npos) {
        return fallback;
    }
    const auto colon_pos = json_text.find(':', key_pos + pattern.size());
    if (colon_pos == std::string::npos) {
        return fallback;
    }
    const auto first_quote = json_text.find('"', colon_pos + 1);
    if (first_quote == std::string::npos) {
        return fallback;
    }
    const auto second_quote = json_text.find('"', first_quote + 1);
    if (second_quote == std::string::npos) {
        return fallback;
    }
    return json_text.substr(first_quote + 1, second_quote - first_quote - 1);
}

} 

MockHttpResponse handle_create_operation_mock(int product_id, const std::string& raw_body) {
    const std::string op_type = extract_string_field(raw_body, "type", "income");
    const std::string new_status = extract_string_field(raw_body, "status", "");
    const std::string ts = utc_now_iso8601();

    std::ostringstream json_out;
    json_out << "{"
             << "\"id\":1,"
             << "\"product_id\":" << product_id << ","
             << "\"type\":\"" << escape_json(op_type) << "\","
             << "\"previous_status\":" << (op_type == "transfer" ? "\"на складе\"" : "null") << ","
             << "\"new_status\":" << ((op_type == "transfer" && !new_status.empty())
                                         ? ("\"" + escape_json(new_status) + "\"")
                                         : "null") << ","
             << "\"created_at\":\"" << ts << "\""
             << "}";

    return MockHttpResponse{201, "application/json; charset=utf-8", json_out.str()};
}

MockHttpResponse handle_list_operations_mock() {
    const std::string ts = utc_now_iso8601();
    std::ostringstream json_out;
    json_out << "{"
             << "\"items\":["
             << "{\"id\":1,\"product_id\":1,\"product_name\":\"Стол письменный\","
             << "\"type\":\"income\",\"quantity\":10,"
             << "\"previous_status\":null,\"new_status\":null,"
             << "\"created_at\":\"" << ts << "\"}"
             << "]"
             << "}";

    return MockHttpResponse{200, "application/json; charset=utf-8", json_out.str()};
}