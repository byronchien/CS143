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
		<form action="index.php" method="GET">
			Search:<input name="search" type="text" placeholder="Search">
			<input type="submit" value="submit">
		</form>

		<?php
			if ($_GET["search"] == NULL)
			{
				return;
			}

			try {
				$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
			} catch (PDOException $e) {
				echo 'Connection failed: ' . $e->getMessage();
				return;
			}

			$keywords = explode(' ', $_GET["search"]);

			$actorSearch = "SELECT first,last,dob FROM Actor WHERE first LIKE '%".$keywords[0].
				"%' OR last LIKE '%".$keywords[0]."%'";

			for ($i = 1; $i < count($keywords); $i++)
			{
				$actorSearch .= " OR first LIKE '%".$keywords[$i]."%' OR ";
				$actorSearch .= "last LIKE '%".$keywords[$i]."%'";
			}

			$actorSearch .= " ORDER BY last ASC";

			$query = $db->prepare($actorSearch);
			$query->execute();
			$results = $query->fetchAll(PDO::FETCH_ASSOC);

			//var_dump($results);
			echo "<h3>Actors</h3>";
			$resultsTable = "<table><tr>";

			// retrieve column names
			$resultsTable .= "<th>Name</th><th>Date of Birth</th>";
			$resultsTable .= "</tr>";

			foreach($results as $row)
			{
				$resultsTable .= "<tr>";
				$resultsTable .= "<td>";
				$resultsTable .= "<a href='browseActor.php?first=".$row["first"]
					."&last=".$row["last"]."&dob=".$row["dob"]."'>";
				$resultsTable .= $row["first"]." ".$row["last"]."</a></td>";
				$resultsTable .= "<td>".$row["dob"]."</td>";

				$resultsTable .= "</tr>";
			}

			$resultsTable .= "</table><br><br>";
			echo $resultsTable;

			$movieSearch = "SELECT title,year FROM Movie WHERE title LIKE '%".$keywords[0]."%'";

			for ($i = 1; $i < count($keywords); $i++)
			{
				$movieSearch .= " OR title LIKE '%".$keywords[$i]."%'";
			}

			$movieSearch .= " ORDER BY title ASC";

			$query = $db->prepare($movieSearch);
			$query->execute();
			$results = $query->fetchAll(PDO::FETCH_ASSOC);

			echo "<h3>Movies</h3>";

			$resultsTable = "<table><tr>";

			// retrieve column names
			foreach($results[0] as $key => $value)
			{
				$columnNames[] = $key;
				$resultsTable .= "<th>".$key."</th>";
			}
			$resultsTable .= "</tr>";
			foreach($results as $row)
			{
				$resultsTable .= "<tr>";
				$resultsTable .="<td><a href='browseMovie.php?title=".$row["title"]."'>";
				$resultsTable .= $row["title"]."</a><td>".$row["year"]."</td>";
				$resultsTable .= "</tr>";
			}

			$resultsTable .= "</table><br><br>";
			echo $resultsTable;

		?>

	</div>
</div>

</body>
</html>