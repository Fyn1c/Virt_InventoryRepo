DROP TABLE IF EXISTS operations;
DROP TABLE IF EXISTS products;

CREATE TABLE products (
    id         integer GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    name       varchar(200) NOT NULL CHECK (btrim(name) <> ''),
    status     text NOT NULL DEFAULT 'на складе'
               CHECK (status IN ('на складе', 'выдано', 'в резерве')),
    created_at timestamptz NOT NULL DEFAULT now()
);

CREATE TABLE operations (
    id          bigint GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    product_id  integer NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    type        text NOT NULL CHECK (type IN ('in', 'out', 'move')),
    quantity    integer CHECK (quantity BETWEEN 1 AND 100000),
    from_status text CHECK (from_status IN ('на складе', 'выдано', 'в резерве')),
    to_status   text CHECK (to_status   IN ('на складе', 'выдано', 'в резерве')),
    created_at  timestamptz NOT NULL DEFAULT now(),
    CONSTRAINT operation_shape CHECK (

        (type IN ('in', 'out') AND quantity IS NOT NULL
                               AND from_status IS NULL AND to_status IS NULL)

        OR (type = 'move' AND quantity IS NULL
                          AND from_status IS NOT NULL AND to_status IS NOT NULL)
    )
);

CREATE INDEX idx_operations_product ON operations(product_id);
CREATE INDEX idx_operations_created ON operations(created_at DESC);
