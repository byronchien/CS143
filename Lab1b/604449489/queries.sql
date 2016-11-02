#Give me the names of all the actors in the movie 'Die Another Day'. 
#Please also make sure actor names are in this format:  <firstname> <lastname>  
#(seperated by single space, **very important**).

SELECT first ' ' last
FROM MovieActor INNER JOIN actor
WHERE movie = 'Die Another Day';

#Give me the count of all the actors who acted in multiple movies.

SELECT COUNT(*)
FROM MovieActor C
WHERE C.aid = (	SELECT DISTINCT A.aid
				FROM MovieActor A, MovieActor B
				WHERE A.aid = B.aid AND A.mid > B.mid);

#another query that you came up with

#Give me the names of all actors who are also directors

SELECT A.first ' ' A.last
FROM Actor A, Director B
WHERE A.id = B.id