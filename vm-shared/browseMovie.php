<html>
<head>
	<link rel="stylesheet" href="./style.css" type="text/css">
</head>
<body>
<div id="nav-bar">
<!-- will be used on every page to navigate between functions -->
	<ul>
		<li><a href="http://localhost:1438/~cs143/add-content.php">
			Add Content</a></li>
		<li><a href="">Review Content</a></li>
		<li><a href="">Search Database</a></li>
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
				
				$title = $_GET["title"];

				try {
					$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
				} catch (PDOException $e) {
					echo 'Connection failed: ' . $e->getMessage();
					return;
				}

				$movieInfo = "SELECT * FROM Movie WHERE title = '".$title."'";


				$query = $db->prepare($movieInfo);
				$query->execute();
				$results = $query->fetch(PDO::FETCH_ASSOC);

				echo "<h3>Movie Info</h3>";

				$resultsTable = "<table><tr><th>Title</th><th>Year</th>".
					"<th>Rating</th><th>Company</th></tr>";
				$resultsTable .= "<td>".$results["title"]."</td><td>".$results["year"];
				$resultsTable .= "</td><td>".$results["rating"]."</td><td>".$results["company"];
				$resultsTable .= "</td></tr></table><br><br><br>";

				echo $resultsTable;

				$Actors = "SELECT role, first, last, dob FROM MovieActor, Actor WHERE mid ='".
					$results["id"]."' AND MovieActor.aid = Actor.id";
				$query = $db->prepare($Actors);
				$query->execute();
				$results = $query->fetchAll(PDO::FETCH_ASSOC);

				echo "<h3>Actors in this Movie</h3>";

				$resultsTable = "<table><tr><th>Name</th><th>Role</th></tr>";
				foreach ($results as $row)
				{
					$resultsTable .= "<tr><td><a href='browseActor.php?".
						"first=".$row["first"].
						"&last=".$row["last"].
						"&dob=".$row["dob"]."'>";

					$resultsTable .= $row["first"]." ".$row["last"]."</a></td>";
					$resultsTable .= "<td>".$row["role"]."</td></tr>";
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