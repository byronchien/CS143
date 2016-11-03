<html>
<head>
	<link rel="stylesheet" href="./style.css" type="text/css">
</head>
<body>
<div id="nav-bar">
<!-- will be used on every page to navigate between functions -->
	<ul>
		<li><a href="add-person.php">
			Add Person</a></li>
		<li><a href="add-actor-to-movie.php">
			Add Actor to Movie Relation</a></li>
		<li><a href="add-director-to-movie.php">
			Add Director to Movie Relation</a></li>
		<li><a href="add-movie-info.php">
			Add Movie Info</a></li>
		<li><a href="add-comment.php">
			Review Content</a></li>
		<li><a href="index.php">
			Search Database</a></li>
	</ul>
</div>

<div id="container">
<!-- will be used to display the content of each page -->
	<div id="main-content">
		<?php
			if ($_GET == null)
			{
				echo "<h3>Search for an Actor or Movie</h3>";
			}
			else
			{
				$first = $_GET["first"];
				$last = $_GET["last"];
				$dob = $_GET["dob"];

				try {
					$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
				} catch (PDOException $e) {
					echo 'Connection failed: ' . $e->getMessage();
					return;
				}

				$ActorInfo = "SELECT * FROM Actor WHERE first='".$first."' AND last='"
							.$last."' AND dob='".$dob."'";

				$query = $db->prepare($ActorInfo);
				$query->execute();
				$results = $query->fetch(PDO::FETCH_ASSOC);

				$results["dod"] = ($results["dod"] == null) ?
					 "Still Alive" : $results["dod"];

				echo "<h3>Actor Information</h3>";

				$resultsTable = "<table><tr><th>Name</th><th>Sex</th>".
					"<th>Date of Birth</th><th>Date of Death</th></tr>";
				$resultsTable .= "<td>".$results["first"]." ".$results["last"]."</td><td>".$results["sex"];
				$resultsTable .= "</td><td>".$results["dob"]."</td><td>".$results["dod"];
				$resultsTable .= "</td></tr></table><br><br><br>";

				echo $resultsTable;

				$ActorMovies = "SELECT role, title FROM MovieActor, Movie WHERE aid = '".
					$results["id"]."' AND Movie.id=MovieActor.mid";

				$query = $db->prepare($ActorMovies);
				$query->execute();
				$results = $query->fetchAll(PDO::FETCH_ASSOC);

				echo "<h3>Movie Information</h3>";

				$resultsTable = "<table><tr><th>Role</th><th>Title</th></tr>";
				foreach($results as $row)
				{
					$resultsTable .= "<tr><td>".$row["role"]."</td>";
					$resultsTable .= "<td><a href='browseMovie.php?".
						"title=".$row["title"]."'>";
					$resultsTable .= $row["title"]."</a></td></tr>";
				}

				$resultsTable .= "</table><br><br><br>";

				echo $resultsTable;


			}
		?>


		<form action="index.php" method="GET">
			Search:<input name="search" type="text" placeholder="Search">
			<input type="submit" value="submit">
		</form>
	</div>
</div>

</body>
</html>