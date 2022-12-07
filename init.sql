CREATE TABLE users
(
    id       SERIAL PRIMARY KEY,
    username VARCHAR(31)  NOT NULL UNIQUE,
    email    VARCHAR(255) NOT NULL UNIQUE,
    password CHAR(64)     NOT NULL,
    is_admin BOOLEAN
);

CREATE TABLE tokens
(
    id          SERIAL PRIMARY KEY,
    user_id     INT REFERENCES users (id) ON DELETE CASCADE NOT NULL,
    create_time TIMESTAMP                                   NOT NULL
);

CREATE TABLE categories
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(31) NOT NULL
);

CREATE TABLE notes
(
    id          SERIAL PRIMARY KEY,
    title       VARCHAR(255) NOT NULL,
    text        TEXT         NOT NULL,
    author_id   INT          REFERENCES users (id) ON DELETE SET NULL,
    category_id INT          REFERENCES categories (id) ON DELETE SET NULL
);

