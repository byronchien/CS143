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

<!-- will be used to get input to add to MovieDirector table -->
<form method="GET">

<!-- will be used to add to MovieDirector table -->
<p>
<?php
	try {
		$db = new PDO('mysql:dbname=CS143;host=127.0.0.1', 'cs143', '');
	} catch (PDOException $e) {
		echo 'Connection failed: ' . $e->getMessage();
		return;
	}	

	// Get Director ID
  	$rs = $db->query("select id, first, last from Director");
 	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

    echo "<div class='form-group'>
    	<label for='did'>Director</br></label>
    	<select name='did'>";

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

	$query = "insert into MovieDirector values (?, ?)";
	$vars = [$_GET["mid"], $_GET["did"]];

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