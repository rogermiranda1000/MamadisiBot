DROP DATABASE IF EXISTS MamadisiBot;
CREATE DATABASE MamadisiBot;

USE MamadisiBot;

DROP TABLE IF EXISTS Admins;
DROP TABLE IF EXISTS Responses;
DROP TABLE IF EXISTS Reaction;
DROP TABLE IF EXISTS Message;

CREATE TABLE Admins (
  id BIGINT UNSIGNED NOT NULL, -- user ID (snowflake)
  PRIMARY KEY (id)
);

CREATE TABLE Messages (
  id INTEGER NOT NULL AUTO_INCREMENT,
  sended_by BIGINT UNSIGNED, -- user ID (snowflake)
  message VARCHAR(2000),
  PRIMARY KEY (id)
);

CREATE TABLE Responses (
  id INTEGER NOT NULL,
  response VARCHAR(2000),
  image VARCHAR(255), -- if contains http:// or https:// it's remote; if not local
  PRIMARY KEY (id),
  FOREIGN KEY (id) REFERENCES Messages(id)
);

CREATE TABLE Reactions (
  id INTEGER NOT NULL,
  emoji VARCHAR(255),
  PRIMARY KEY (id),
  FOREIGN KEY (id) REFERENCES Messages(id)
);
