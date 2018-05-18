CREATE DATABASE water;
\connect water;

CREATE TABLE records (
  _id BIGSERIAL PRIMARY KEY,
  data JSONB,
  timestamp TIMESTAMP default current_timestamp
);