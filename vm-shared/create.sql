CREATE TABLE Movie(
	id INT, 
	title VARCHAR(100),
	year INT,
	rating VARCHAR(10),
	comany VARCHAR(50),
	PRIMARY KEY (id),
	CHECK (title IS NOT NULL AND title <> ''));

CREATE TABLE Actor(
	id INT,
	last VARCHAR(20),
	first VARCHAR(20),
	sex VARCHAR(6),
	dob DATE,
	dod DATE,
	PRIMARY KEY (id),
	CHECK (last IS NOT NULL AND last <> ''),
	CHECK (first IS NOT NULL AND first <> ''),
	CHECK (sex IS NOT NULL AND sex <> ''),
	CHECK (dob IS NOT NULL));

CREATE TABLE Director(
	id INT,
	last VARCHAR(20),
	first VARCHAR(20),
	dob DATE,
	dod DATE,
	PRIMARY KEY (id),
	CHECK (last IS NOT NULL AND last <> ''),
	CHECK (first IS NOT NULL AND first <> ''),
	CHECK (dob IS NOT NULL));

CREATE TABLE MovieGenre(
	mid INT,
	genre VARCHAR(20),
	FOREIGN KEY	(mid) REFERENCES Movie(id),
	CHECK (genre IS NOT NULL AND genre <> '')
	);

CREATE TABLE MovieDirector(
	mid INT,
	did INT,
	FOREIGN KEY (did) REFERENCES Director(id),
	FOREIGN KEY	(mid) REFERENCES Movie(id));

CREATE TABLE MovieActor(
	mid INT,
	aid INT,
	role VARCHAR(50),
	FOREIGN KEY (mid) REFERENCES Movie(id),
	FOREIGN KEY (aid) REFERENCES Actor(id),
	CHECK (role IS NOT NULL AND role <> ''));

CREATE TABLE Review(
	name VARCHAR(20),
	time TIMESTAMP,
	mid INT,
	rating INT,
	comment VARCHAR(500),
	FOREIGN KEY (mid) REFERENCES Movie(id));

CREATE TABLE MaxPersonID(
	id INT,
	CHECK (id > '0' AND id IS NOT NULL));

CREATE TABLE MaxMovieID(
	id INT,
	CHECK (id > '0' AND id IS NOT NULL));

CREATE TRIGGER personID
	BEFORE INSERT ON Actor
	FOR EACH ROW
	BEGIN
		SET NEW.last = 'test';
	END;


