BEGIN;

TRUNCATE operations, products RESTART IDENTITY CASCADE;

INSERT INTO products (name, status, created_at) VALUES
('Table',    'given',        now() - INTERVAL '40 days'),
('Chair',    'on warehouse', now() - INTERVAL '39 days'),
('Hammer',   'on warehouse', now() - INTERVAL '38 days'),
('Drill',    'on warehouse', now() - INTERVAL '37 days'),
('Lamp',     'given',        now() - INTERVAL '36 days'),
('Shelf',    'reserved',     now() - INTERVAL '35 days'),
('Notebook', 'on warehouse', now() - INTERVAL '34 days'),
('Monitor',  'given',        now() - INTERVAL '33 days'),
('Keyboard', 'on warehouse', now() - INTERVAL '32 days'),
('Mouse',    'reserved',     now() - INTERVAL '31 days'),
('Cable',    'on warehouse', now() - INTERVAL '30 days'),
('Printer',  'given',        now() - INTERVAL '29 days'),
('Paper',    'on warehouse', now() - INTERVAL '28 days'),
('Stapler',  'on warehouse', now() - INTERVAL '27 days'),
('Marker',   'on warehouse', now() - INTERVAL '26 days');

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   7,    NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Table'
UNION ALL
SELECT id, 'out',  5,    NULL,           NULL,    now() - INTERVAL '8 days'  FROM products WHERE name = 'Table'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'given', now() - INTERVAL '5 days'  FROM products WHERE name = 'Table';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   3,    NULL,           NULL,            now() - INTERVAL '10 days' FROM products WHERE name = 'Chair'
UNION ALL
SELECT id, 'in',   5,    NULL,           NULL,            now() - INTERVAL '8 days'  FROM products WHERE name = 'Chair'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'reserved',      now() - INTERVAL '5 days'  FROM products WHERE name = 'Chair'
UNION ALL
SELECT id, 'move', NULL, 'reserved',     'on warehouse',  now() - INTERVAL '3 days'  FROM products WHERE name = 'Chair';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   7,    NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Hammer'
UNION ALL
SELECT id, 'out',  7,    NULL,           NULL,    now() - INTERVAL '8 days'  FROM products WHERE name = 'Hammer';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   100000, NULL,         NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Drill'
UNION ALL
SELECT id, 'out',  10,     NULL,         NULL,    now() - INTERVAL '8 days'  FROM products WHERE name = 'Drill';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   7,    NULL,           NULL,            now() - INTERVAL '10 days' FROM products WHERE name = 'Lamp'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'reserved',      now() - INTERVAL '9 days'  FROM products WHERE name = 'Lamp'
UNION ALL
SELECT id, 'move', NULL, 'reserved',     'on warehouse',  now() - INTERVAL '8 days'  FROM products WHERE name = 'Lamp'
UNION ALL
SELECT id, 'out',  7,    NULL,           NULL,            now() - INTERVAL '5 days'  FROM products WHERE name = 'Lamp'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'given',         now() - INTERVAL '3 days'  FROM products WHERE name = 'Lamp';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   20,   NULL,           NULL,            now() - INTERVAL '10 days' FROM products WHERE name = 'Shelf'
UNION ALL
SELECT id, 'in',   5,    NULL,           NULL,            now() - INTERVAL '9 days'  FROM products WHERE name = 'Shelf'
UNION ALL
SELECT id, 'out',  8,    NULL,           NULL,            now() - INTERVAL '7 days'  FROM products WHERE name = 'Shelf'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'reserved',      now() - INTERVAL '4 days'  FROM products WHERE name = 'Shelf';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   50,   NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Notebook'
UNION ALL
SELECT id, 'out',  50,   NULL,           NULL,    now() - INTERVAL '6 days'  FROM products WHERE name = 'Notebook';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   4,    NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Monitor'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'given', now() - INTERVAL '7 days'  FROM products WHERE name = 'Monitor';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   15,   NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Keyboard'
UNION ALL
SELECT id, 'out',  3,    NULL,           NULL,    now() - INTERVAL '8 days'  FROM products WHERE name = 'Keyboard'
UNION ALL
SELECT id, 'out',  2,    NULL,           NULL,    now() - INTERVAL '4 days'  FROM products WHERE name = 'Keyboard';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   12,   NULL,           NULL,            now() - INTERVAL '10 days' FROM products WHERE name = 'Mouse'
UNION ALL
SELECT id, 'out',  12,   NULL,           NULL,            now() - INTERVAL '8 days'  FROM products WHERE name = 'Mouse'
UNION ALL
SELECT id, 'in',   6,    NULL,           NULL,            now() - INTERVAL '5 days'  FROM products WHERE name = 'Mouse'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'reserved',      now() - INTERVAL '2 days'  FROM products WHERE name = 'Mouse';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   100,  NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Cable'
UNION ALL
SELECT id, 'out',  40,   NULL,           NULL,    now() - INTERVAL '8 days'  FROM products WHERE name = 'Cable'
UNION ALL
SELECT id, 'out',  40,   NULL,           NULL,    now() - INTERVAL '5 days'  FROM products WHERE name = 'Cable'
UNION ALL
SELECT id, 'out',  20,   NULL,           NULL,    now() - INTERVAL '2 days'  FROM products WHERE name = 'Cable';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   2,    NULL,           NULL,            now() - INTERVAL '10 days' FROM products WHERE name = 'Printer'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'reserved',      now() - INTERVAL '7 days'  FROM products WHERE name = 'Printer'
UNION ALL
SELECT id, 'move', NULL, 'reserved',     'given',         now() - INTERVAL '3 days'  FROM products WHERE name = 'Printer';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   500,  NULL,           NULL,    now() - INTERVAL '10 days' FROM products WHERE name = 'Paper'
UNION ALL
SELECT id, 'out',  120,  NULL,           NULL,    now() - INTERVAL '8 days'  FROM products WHERE name = 'Paper'
UNION ALL
SELECT id, 'in',   200,  NULL,           NULL,    now() - INTERVAL '5 days'  FROM products WHERE name = 'Paper'
UNION ALL
SELECT id, 'out',  300,  NULL,           NULL,    now() - INTERVAL '1 day'   FROM products WHERE name = 'Paper';

INSERT INTO operations (product_id, type, quantity, from_status, to_status, created_at)
SELECT id, 'in',   1,    NULL,           NULL,            now() - INTERVAL '10 days' FROM products WHERE name = 'Stapler'
UNION ALL
SELECT id, 'move', NULL, 'on warehouse', 'given',         now() - INTERVAL '6 days'  FROM products WHERE name = 'Stapler'
UNION ALL
SELECT id, 'move', NULL, 'given',        'on warehouse',  now() - INTERVAL '2 days'  FROM products WHERE name = 'Stapler';

COMMIT;