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
?>


</p>
</body>
</html>

