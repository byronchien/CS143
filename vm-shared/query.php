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

	$query = "select * from Movie";
	$test = $db->prepare($query);
	$test->execute();

	$results = $test->fetchAll();
	var_dump($results);

?>


</p>
</body>
</html>

