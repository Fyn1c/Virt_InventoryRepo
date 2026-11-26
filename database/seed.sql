BEGIN;

TRUNCATE operations, products RESTART IDENTITY CASCADE;

INSERT INTO products (name, status, created_at)
VALUES ('Table', 'on warehouse', now() + INTERVAL '3 hours');

INSERT INTO operations (product_id, type, quantity, created_at)
SELECT id, 'in', 7, now() + INTERVAL '3 hours'
FROM products WHERE name = 'Table';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at) 
SELECT id, 'move', 3, 'on warehouse', 'given', now() + INTERVAL '3 hours'
FROM products WHERE name = 'Table';


COMMIT;