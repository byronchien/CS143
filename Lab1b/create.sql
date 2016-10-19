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
	CHECK (dob IS NOT NULL));

CREATE TABLE Director(
	id INT,
	last VARCHAR(20),
	first VARCHAR(20),
	dob DATE,
	dod DATE,
	PRIMARY KEY (id),
	CHECK (dob IS NOT NULL));
