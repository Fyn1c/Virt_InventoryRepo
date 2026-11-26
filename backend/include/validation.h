#pragma once

#include <nlohmann/json.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace validation {

struct FieldError {
    std::string field;
    std::string message;
};

struct Result {
    std::vector<FieldError> errors;

    bool ok() const { return errors.empty(); }
    void add(const std::string& field, const std::string& message);
};

inline constexpr std::size_t kNameMaxLength = 200;
inline constexpr std::int64_t kQuantityMin  = 1;
inline constexpr std::int64_t kQuantityMax  = 100000;

enum class OperationType {
    Income,
    Expense,
    Move
};

const std::vector<std::string>& storageStatuses();
const std::vector<std::string>& operationTypes();

bool isStorageStatus(const std::string& s);
bool isOperationType(const std::string& s);

std::optional<OperationType> parseOperationType(const std::string& s);
std::string toString(OperationType t);
Result validateProductCreate(const nlohmann::json& body);

struct OperationRequest {
    OperationType              type{};
    std::int64_t               quantity{};
    std::optional<std::string> targetStatus;
};

struct OperationValidation {
    Result           result;
    OperationRequest request;
};

OperationValidation validateOperationCreate(const nlohmann::json& body);

}  