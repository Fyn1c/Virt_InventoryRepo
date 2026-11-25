BEGIN;

TRUNCATE operations, products RESTART IDENTITY CASCADE;

INSERT INTO products (name, status, created_at) VALUES
('Стол письменный', 'на складе', now() - interval '60 days');

INSERT INTO operations (product_id, type, quantity, created_at) VALUES
(1, 'in', 7, now() - interval '50 days');

INSERT INTO operations (product_id, type, from_status, to_status, created_at) VALUES
(1, 'move', 'на складе', 'выдано', now() - interval '35 days');

COMMIT;