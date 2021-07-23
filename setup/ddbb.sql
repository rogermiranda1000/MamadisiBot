DROP DATABASE IF EXISTS MamadisiBot;
CREATE DATABASE MamadisiBot;

USE MamadisiBot;

DROP TABLE IF EXISTS Admins;
DROP TABLE IF EXISTS Writers;
DROP TABLE IF EXISTS Responses;
DROP TABLE IF EXISTS Reactions;
DROP TABLE IF EXISTS Messages;
DROP TABLE IF EXISTS Servers;

CREATE TABLE Servers (
  id BIGINT UNSIGNED NOT NULL, -- server ID (snowflake)
  PRIMARY KEY (id)
);

CREATE TABLE Admins (
  id BIGINT UNSIGNED NOT NULL, -- user ID (snowflake)
  PRIMARY KEY (id)
);

CREATE TABLE Writers (
  id BIGINT UNSIGNED NOT NULL, -- user ID (snowflake)
  PRIMARY KEY (id)
);

CREATE TABLE Messages (
  server BIGINT UNSIGNED, -- null if it's a global message
  id INTEGER NOT NULL AUTO_INCREMENT,
  sended_by BIGINT UNSIGNED, -- user ID (snowflake)
  message TEXT, -- regex of the message
  PRIMARY KEY (id),
  FOREIGN KEY (server) REFERENCES Servers(id)
);

CREATE TABLE Responses (
  id INTEGER NOT NULL,
  response TEXT,
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

DROP TRIGGER IF EXISTS addMessage;
CREATE TRIGGER addMessage
BEFORE INSERT ON Messages
FOR EACH ROW
    INSERT IGNORE INTO Servers(id) VALUE (`server`);