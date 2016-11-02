<html>
<head>
</head>
<body>
<form action="query.php" method="get">
	<TEXTAREA NAME="query" rows="8" cols="60" placeholder="Enter your query here"></textarea>
	<br>
	<input type="submit"> 
</form>

<p>
<?php
	if ($_GET["query"] != NULL)
	{
		echo $_GET["query"];
	}


	try {
		$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
	} catch (PDOException $e) {
		echo 'Connection failed: ' . $e->getMessage();
		return;
	}

	//$query = "select * from Movie";
	$query = $_GET["query"];

	$test = $db->prepare($query);
	$test2 = $test->execute();

	$columnNames = [];
	$results = $test->fetchAll(PDO::FETCH_ASSOC);

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
		foreach($row as $value)
		{
			$resultsTable .= "<td>".$value."</td>";
		}
		$resultsTable .= "</tr>";
	}

	$resultsTable .= "</table>";

	echo $resultsTable;
	//var_dump($columnNames);
?>


</p>
</body>
</html>

