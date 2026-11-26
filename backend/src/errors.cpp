#include "errors.hpp"

nlohmann::json ApiError::to_json() const {
    nlohmann::json j;
    j["code"]    = code;
    j["message"] = message;
    if (!fields.empty()) {
        j["fields"] = nlohmann::json::array();
        for (auto& f : fields)
            j["fields"].push_back({{"field", f.field}, {"message", f.message}});
    }
    return j;
}

ApiError ApiError::validation(const std::vector<FieldError>& fields) {
    return {400, "validation_error", "Ошибка валидации", fields};
}
ApiError ApiError::not_found(const std::string& what) {
    return {404, "not_found", what, {}};
}
ApiError ApiError::insufficient_stock(const std::string& message) {
    return {400, "insufficient_stock", message, {}};
}
ApiError ApiError::internal(const std::string& message) {
    return {500, "internal_error", message, {}};
}
ApiError ApiError::not_implemented() {
    return {501, "not_implemented", "ещё не реализовано", {}};
}