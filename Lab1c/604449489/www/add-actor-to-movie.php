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


<!-- will be used to get input to add to MovieActor table -->
<form method="GET">
    <div class="form-group">
      <label for="role">Role</label>
      <input type="text" class="form-control" placeholder="Text input"  
      			name="role"/>
    </div>

<!-- will be used to add to MovieActor table -->
<p>
<?php
	try {
		$db = new PDO('mysql:dbname=CS143;host=127.0.0.1', 'cs143', '');
	} catch (PDOException $e) {
		echo 'Connection failed: ' . $e->getMessage();
		return;
	}	

	// Get Actor ID
  	$rs = $db->query("select id, first, last from Actor");
 	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

    echo "<div class='form-group'>
    	<label for='aid'>Actor</br></label>
    	<select name='aid'>";

    while ($row = $rs->fetch(PDO::FETCH_ASSOC)) {

                  unset($id, $first, $last);
                  $id = $row['id'];
                  $first = $row['first']; 
                  $last = $row['last']; 
                  echo '<option value="'.$id.'">'.$first.' '.$last.'</option>';
                 
	}

    echo "</select><br></div>";

	// Get Movie ID
  	$rs = $db->query("select id, title from Movie");
 	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

    echo "<div class='form-group'>
    	<label for='mid'>Movie</br></label>
    	<select name='mid'>";

    while ($row = $rs->fetch(PDO::FETCH_ASSOC)) {

                  unset($id, $title);
                  $id = $row['id'];
                  $title = $row['title']; 
                  echo '<option value="'.$id.'">'.$title.'</option>';
                 
	}

    echo "</select><br></div>";
    echo "<button type='submit' class='btn btn-default'>
    		Add to Database</button></form>";

	$valid_input = true;
	if ($_GET["role"] == NULL || $_GET["role"] == '') {
		echo "role is empty<br>";
		$valid_input = false;
	}

	if ($valid_input == false) {
		return;
	}

	$query = "insert into MovieActor values (?, ?, ?)";
	$vars = [$_GET["mid"], $_GET["aid"], $_GET["role"]];

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