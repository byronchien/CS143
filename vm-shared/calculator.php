<html>
<head>
</head>
<body>

<h1> CALCULATOR </h1>
<form action="calculator.php" method="post">
<p> Input string: 
	<input type="text" name="calc_input">
	<input type="submit">
</p>

<p>
<?php
	if ($_POST["calc_input"] != NULL)
	{
		$calc_input = $_POST["calc_input"];
		$matches = [];
		preg_match("/([0-9]+[+-\/*])+([0-9]+)/",$calc_input, $matches);
		var_dump($matches);
		echo preg_last_error();
	}

 ?>
</p>
</form>
</body>
</html>
