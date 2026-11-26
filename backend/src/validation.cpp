#include "../include/validation.h"

#include <algorithm>

namespace validation {

namespace {

std::string trim(const std::string& s) {
    const char* ws = " \t\n\r";
    const auto b = s.find_first_not_of(ws);
    if (b == std::string::npos) return "";
    const auto e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

std::optional<std::int64_t> extractQuantity(const nlohmann::json& j) {
    if (!j.is_number()) return std::nullopt;
    if (!j.is_number_integer()) return std::nullopt;   // 5.0 → отклонить

    const std::int64_t v = j.get<std::int64_t>();
    if (v < kQuantityMin || v > kQuantityMax) return std::nullopt;
    return v;
}

}

void Result::add(const std::string& field, const std::string& message) {
    errors.push_back({field, message});
}

const std::vector<std::string>& storageStatuses() {
    static const std::vector<std::string> v = {
        "на складе",
        "выдано",
        "в резерве"
    };
    return v;
}

const std::vector<std::string>& operationTypes() {
    static const std::vector<std::string> v = {
        "приход",
        "расход",
        "перемещение"
    };
    return v;
}

bool isStorageStatus(const std::string& s) {
    const auto& v = storageStatuses();
    return std::find(v.begin(), v.end(), s) != v.end();
}

bool isOperationType(const std::string& s) {
    const auto& v = operationTypes();
    return std::find(v.begin(), v.end(), s) != v.end();
}

std::optional<OperationType> parseOperationType(const std::string& s) {
    if (s == "приход")      return OperationType::Income;
    if (s == "расход")      return OperationType::Expense;
    if (s == "перемещение") return OperationType::Move;
    return std::nullopt;
}

std::string toString(OperationType t) {
    switch (t) {
        case OperationType::Income:  return "приход";
        case OperationType::Expense: return "расход";
        case OperationType::Move:    return "перемещение";
    }
    return "";
}

Result validateProductCreate(const nlohmann::json& body) {
    Result r;

    if (!body.is_object()) {
        r.add("body", "тело запроса должно быть JSON-объектом");
        return r;
    }

    if (!body.contains("name")) {
        r.add("name", "наименование обязательно");
        return r;
    }

    if (!body["name"].is_string()) {
        r.add("name", "наименование должно быть строкой");
        return r;
    }

    const std::string name = trim(body["name"].get<std::string>());

    if (name.empty()) {
        r.add("name", "наименование не может быть пустым");
    } else if (name.size() > kNameMaxLength) {
        r.add("name", "наименование не более 200 символов");
    }

    return r;
}

OperationValidation validateOperationCreate(const nlohmann::json& body) {
    OperationValidation out;
    Result& r = out.result;

    if (!body.is_object()) {
        r.add("body", "тело запроса должно быть JSON-объектом");
        return out;
    }

    if (!body.contains("type") || !body["type"].is_string()) {
        r.add("type", "тип операции обязателен и должен быть строкой");
        return out;
    }

    const auto parsedType = parseOperationType(body["type"].get<std::string>());
    if (!parsedType.has_value()) {
        r.add("type", "тип должен быть одним из: приход, расход, перемещение");
        return out;
    }
    out.request.type = *parsedType;
    if (body.contains("quantity")) {
        const auto q = extractQuantity(body["quantity"]);
        if (!q.has_value()) {
            r.add("quantity", "количество должно быть целым числом от 1 до 100000");
        } else {
            out.request.quantity = *q;
        }
    } else if (out.request.type == OperationType::Move) {
        out.request.quantity = 1;
    } else {
        r.add("quantity", "количество обязательно");
    }
    if (out.request.type == OperationType::Move) {
        if (!body.contains("targetStatus") || body["targetStatus"].is_null()) {
            r.add("targetStatus", "для перемещения обязательно указать новый статус");
        } else if (!body["targetStatus"].is_string()) {
            r.add("targetStatus", "статус должен быть строкой");
        } else {
            const std::string s = body["targetStatus"].get<std::string>();
            if (!isStorageStatus(s)) {
                r.add("targetStatus",
                      "статус должен быть одним из: на складе, выдано, в резерве");
            } else {
                out.request.targetStatus = s;
            }
        }
    } else {
        if (body.contains("targetStatus") && !body["targetStatus"].is_null()) {
            r.add("targetStatus",
                  "targetStatus допустим только для типа «перемещение»");
        }
    }

    return out;
}

}