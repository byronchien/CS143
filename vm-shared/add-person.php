<html>
<head>
	<link rel="stylesheet" href="./style.css" type="text/css">
	<h1> Add new Actor/Director </h1>
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

<!-- will be used to get input to add new Actor/Director -->
<form method="GET">
   <label class="radio-inline">
        <input type="radio" checked="checked" name="identity" value="Actor"/>
        Actor
    </label>
    <label class="radio-inline">
        <input type="radio" name="identity" value="Director"/>Director
    </label>
    <div class="form-group">
      <label for="first">First Name</label>
      <input type="text" class="form-control" placeholder="Text input"  
      			name="first"/>
    </div>
    <div class="form-group">
      <label for="last">Last Name</label>
      <input type="text" class="form-control" placeholder="Text input" 
      			name="last"/>
    </div>
    <label class="radio-inline">
        <input type="radio" name="sex" checked="checked" value="male">Male
    </label>
    <label class="radio-inline">
        <input type="radio" name="sex" value="female">Female
    </label>
    <div class="form-group">
      <label for="dob">Date of Birth</label>
      <input type="text" class="form-control" placeholder="Text input" 
      			name="dob">ie: 1996-07-19<br>
    </div>
    <div class="form-group">
      <label for="dod">Date of Death</label>
      <input type="text" class="form-control" placeholder="Text input" 
      			name="dod">(If still alive, leave blank)<br>
    </div>
    <button type="submit" class="btn btn-default">Add to Database</button>
</form>

<!-- will be used to add new Actor/Director -->
<p>
<?php
	try {
		$db = new PDO('mysql:dbname=TEST;host=127.0.0.1', 'cs143', '');
	} catch (PDOException $e) {
		echo 'Connection failed: ' . $e->getMessage();
		return;
	}	
	
	$valid_input = true;
	if ($_GET["first"] == NULL || $_GET["first"] == '') {
		echo "first name is empty<br>";
		$valid_input = false;
	}
	if ($_GET["last"] == NULL || $_GET["last"] == '') {
		echo "last name is empty<br>";
		$valid_input = false;
	}
	if ($_GET["dob"] == NULL || $_GET["dob"] == '') {
		echo "date of birth is empty<br>";
		$valid_input = false;
	}
	if ($_GET["dod"] == '') {
		$_GET["dod"] = NULL;
	}
	if ($valid_input == false) {
		return;
	}
	

	//SELECT * FROM Actor WHERE id=( SELECT max(id) FROM Actor)
	
	// Update MaxPersonID
	$query = "update MaxPersonID set id=id+1";
	$rs = $db->query($query);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

	// Select ID from MaxPersonID
	$query = "select id from MaxPersonID";
	$rs = $db->query($query);
	if (!$rs) {
	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}
	$result = $rs->fetch(PDO::FETCH_ASSOC);
	$id = $result['id'];

	// Actor or Director
	$query = ($_GET["identity"] == 'Actor') ?
		"insert into Actor values (?, ?, ?, ?, ?, ?)" :
		"insert into Director values (?, ?, ?, ?, ?)";

	$vars = ($_GET["identity"] == 'Actor') ?
		[$id, $_GET["last"], $_GET["first"], $_GET["sex"], 
		$_GET["dob"], $_GET["dod"]] :
		[$id, $_GET["last"], $_GET["first"], 
		$_GET["dob"], $_GET["dod"]];

	$statement = $db->prepare($query);
	$rs = $statement->execute($vars);
	if (!$rs) {
		echo "test";
		// Update MaxPersonID
		$query = "update MaxPersonID set (id = id - 1)";
		$rs = $db->query($query);
		if (!$rs) {
		    echo "\nPDO::errorInfo():\n";
		    print_r($db->errorInfo());
			exit(1);
		}

	    echo "\nPDO::errorInfo():\n";
	    print_r($db->errorInfo());
		exit(1);
	}

	$db->close();
?>

</p>

</body>
</html>