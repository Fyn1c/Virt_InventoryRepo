#pragma once
#include "database.h"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// =====================================================================
// Типы объявлены, методы возвращают пустышки.
// Антон реализует: запросы к operations, проверку остатка,
// транзакцию «списание + проверка», агрегаты журнала.
// =====================================================================

enum class OperationType { Income, Expense, Move };

inline std::string to_db(OperationType t) {
    switch (t) {
        case OperationType::Income:  return "приход";
        case OperationType::Expense: return "расход";
        case OperationType::Move:    return "перемещение";
    }
    return "";
}

struct Operation {
    int64_t id{};
    int64_t productId{};
    std::string productName;
    OperationType type{OperationType::Income};
    int64_t quantity{};
    std::optional<std::string> resultStatus;
    std::string createdAt;
};

struct OperationCreate {
    OperationType type{OperationType::Income};
    int64_t quantity{};
    std::optional<std::string> targetStatus;
};

class OperationsRepository {
public:
    explicit OperationsRepository(Database& db) : db_(db) {}

    // Антон реализует (JOIN products, ORDER BY created_at DESC)
    std::vector<Operation> list_all();

    // Антон реализует (для карточки товара)
    std::vector<Operation> list_for_product(int64_t productId);

    // Антон реализует (с проверкой остатка, в транзакции)
    // Возврат std::nullopt = бизнес-ошибка insufficient_stock
    std::optional<Operation> create(int64_t productId, const OperationCreate& req);

private:
    Database& db_;
};