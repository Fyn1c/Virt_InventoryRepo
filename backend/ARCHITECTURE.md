### Формат ошибок
{ "code": "...", "message": "...", "fields": [{"field":"...","message":"..."}] }

| HTTP | code |
|---|---|
| 400 | validation_error |
| 400 | insufficient_stock |
| 404 | not_found |
| 500 | internal_error |

### Операции

| Метод | Путь | Запрос | Ответ |
|---|---|---|---|
| GET | /api/products?search= | — | Product[] |
| POST | /api/products | {"name":"..."} | 201 Product |
| GET | /api/products/{id} | — | Product & {operations: Operation[]} |
| DELETE | /api/products/{id} | — | 204 |
| POST | /api/products/{id}/operations | {"type":"...","quantity":N,"targetStatus":"..."} | 201 Operation |
| GET | /api/operations | — | Operation[] (createdAt DESC) |
| GET | /api/statuses | — | {"statuses": [...]} |
