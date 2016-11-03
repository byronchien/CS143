<html>
<head>
	<link rel="stylesheet" href="./style.css" type="text/css">
	<h1> Add Comments to Movies </h1>
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

<!-- will be used to get input to add comments to Review table -->
<form method="GET">
    <div class="form-group">
      <label for="name">Reviewer name</label>
      <input type="text" class="form-control" placeholder="Text input"  
      			name="name"/>
    </div>
    <div class="form-group">
      <label for="rating">Rating</label>
      <input type="text" class="form-control" placeholder="Text input" 
      			name="rating"/>
    </div>
    <div class="form-group">
      <label for="comment">Comment</br></label>
      <textarea class="form-control" placeholder="Text input" 
      			name="comment" rows="10" cols="30"></textarea>
    </div>
    <button type="submit" class="btn btn-default">Add to Database</button>
</form>

<!-- will be used to add comments to Review table -->
<p>
<?php
	try {
		$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
	} catch (PDOException $e) {
		echo 'Connection failed: ' . $e->getMessage();
		return;
	}	
	
	// Need movie ID
	$mid = 2;

	$valid_input = true;
	if ($_GET["name"] == NULL || $_GET["name"] == '') {
		echo "name is empty<br>";
		$valid_input = false;
	}
	if ($_GET["rating"] == NULL || $_GET["rating"] == '') {
		echo "rating is empty<br>";
		$valid_input = false;
	}
	else if (!is_numeric($_GET["rating"])) {
		echo "rating is not numeric<br>";
		$valid_input = false;
	}
	if ($_GET["comment"] == NULL || $_GET["comment"] == '') {
		echo "comment is empty<br>";
		$valid_input = false;
	}

	if ($valid_input == false) {
		return;
	}
	
	// Get Timestamp
	$query = "select now()";
	$rs = $db->query($query);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}
	$result = $rs->fetch(PDO::FETCH_ASSOC);
	$timestamp = $result['now()'];

	$query = "insert into Review values (?, ?, ?, ?, ?)";
	$vars = [$_GET["name"], $timestamp, $mid, $_GET["rating"], 
			$_GET["comment"]];
			
	$statement = $db->prepare($query);
	$rs = $statement->execute($vars);
	if (!$rs) {
		echo "hi";
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

	$db->close();
?>

</p>

</body>
</html>