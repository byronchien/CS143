#breaks primary key constraint for Movie
INSERT INTO Movie
VALUES ( null, ‘title’, 2016’,’PG’, ‘CS143’);

#breaks primary key constraint for Director
INSERT INTO Director
VALUES ( null, ‘Last’, ‘First’, ‘2016-10-19’,’’);

#breaks primary key constraint for Actor
INSERT INTO Actor
VALUES ( null,‘Last’, ‘First’, ‘Male’, ‘2016-10-19’,’’);

#breaks Movie Check for null title
INSERT INTO Movie
VALUES (‘5000’,null,’2016’,’PG’,’CS143’);

#breaks foreign key constraint for MovieGenre
INSERT INTO MovieGenre
VALUES (NULL, 1) 

#breaks CHECK constraint for MovieGenre
INSERT INTO MovieGenre
VALUES (1, NULL) 

#breaks foreign key constraint for MovieDirector.mid
INSERT INTO MovieDirector
VALUES (NULL, 1)

#breaks foreign key constraint for MovieDirector.did
INSERT INTO MovieDirector
VALUES (1, NULL)

#breaks foreign key constraint for MovieActor.aid
INSERT INTO MovieActor
VALUES (1, NULL, ‘Doorman’)

#breaks foreign key constraint for MovieActor.mid
INSERT INTO MovieActor
VALUES (NULL, 1, ‘Doorman’)

#breaks CHECK constraint for MovieActor
INSERT INTO MovieActor
VALUES (1, 1, NULL)

#breaks foreign key constraint for Review.mid
INSERT INTO Review
VALUES (‘Nat’, ‘2016-01-01 01:01:01’, -1, 10, ‘nice’)


