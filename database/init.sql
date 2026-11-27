CREATE TABLE IF NOT EXISTS products (
    "id"            INTEGER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    "name"          varchar(200) NOT NULL CHECK (btrim(name) <> ''),
    "status"        TEXT NOT NULL,
    "created_at"    TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS operations (
    "id"            BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    "product_id"    INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    "type"          TEXT NOT NULL CHECK (type IN ('in', 'out', 'move')),
    "quantity"      INTEGER CHECK (quantity >= 1 AND quantity <= 100000),
    "from_status"   TEXT,
    "to_status"     TEXT, 
    "created_at"    TIMESTAMPTZ NOT NULL DEFAULT now()
);

