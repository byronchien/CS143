<html>
<head>
	<link rel="stylesheet" href="./style.css" type="text/css">
	<h1> Add Movie Information </h1>
</head>
<body>

<div id="nav-bar">
<!-- will be used on every page to navigate between functions -->
	<ul>
		<li><a href="add-person.php">
			Add Content</a></li>
		<li><a href="add-comment.php">Review Content</a></li>
		<li><a href="index.php">Search Database</a></li>
	</ul>
</div>


<!-- will be used to get input to add new Actor/Director -->
<form method="GET">
    <div class="form-group">
      <label for="title">Title</label>
      <input type="text" class="form-control" placeholder="Text input"  
      			name="title"/>
    </div>
    <div class="form-group">
      <label for="year">Year</label>
      <input type="text" class="form-control" placeholder="Year input" 
      			name="year">ie: 1996
    </div>
    <div class="form-group">
    	<label for="rating">Rating</label><br>
    	<select name="rating">
    		<option value="">Select...</option>
    		<option value="G">G</option>
    		<option value="PG">PG</option>
    		<option value="PG-13">PG-13</option>
    		<option value="R">R</option>
    		<option value="NC-17">NC-17</option>
    	</select>
    </div>
    <div class="form-group">
      <label for="company">Company</label>
      <input type="text" class="form-control" placeholder="Text input" 
      			name="company"><br>
    </div>
    <div class="form-group">
      <label for="genre">Genre</label>
      <input type="text" class="form-control" placeholder="Text input" 
      			name="genre"><br>
    </div>
    <button type="submit" class="btn btn-default">Add to Database</button>
</form>

<!-- will be used to add Movie Information to Movie and MovieGenre -->
<p>
<?php
	try {
		$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
	} catch (PDOException $e) {
		echo 'Connection failed: ' . $e->getMessage();
		return;
	}
	
	$valid_input = true;
	if ($_GET["title"] == NULL || $_GET["title"] == '') {
		echo "title is empty<br>";
		$valid_input = false;
	}
	if ($_GET["year"] == NULL || $_GET["year"] == '') {
		echo "year is empty<br>";
		$valid_input = false;
	}
	else if (!is_numeric($_GET["year"])) {
		echo "year is not numeric<br>";
		$valid_input = false;
	}
	if ($_GET["genre"] == NULL || $_GET["genre"] == '') {
		echo "genre is empty<br>";
		$valid_input = false;
	}
	if ($_GET["rating"] == '') {
		$_GET["rating"] == NULL;
	}
	if ($_GET["company"] == '') {
		$_GET["company"] == NULL;
	}
	if ($valid_input == false) {
		return;
	}

	//SELECT * FROM Movie WHERE id=( SELECT max(id) FROM Movie)

	// Update MaxMovieID
	$query = "update MaxMovieID set id=id+1";
	$rs = $db->query($query);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

	// Select ID from MaxPersonID
	$query = "select id from MaxMovieID";
	$rs = $db->query($query);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}
	$result = $rs->fetch(PDO::FETCH_ASSOC);
	$id = $result['id'];

	// insert into Movie
	$query = "insert into Movie values (?, ?, ?, ?, ?)";
	$vars = [$id, $_GET["title"], $_GET["year"], $_GET["rating"], 
		$_GET["company"]];
	$statement = $db->prepare($query);
	$rs = $statement->execute($vars);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

	// insert into MovieGenre
	$query = "insert into MovieGenre values (?, ?)";
	$vars = [$id, $_GET["genre"]];
	$statement = $db->prepare($query);
	$rs = $statement->execute($vars);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

	$db->close();
?>

</p>

</body>
</html>