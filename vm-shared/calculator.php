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

		// checks for any characters not allowed
		$check = preg_match("/[^0-9+\-\/*.\ ]+/", $calc_input, $matches);
		$check += preg_match("/[^1-9]
		
		$check += preg_match("/[0-9]+\ [0-9]+/", $calc_input, $matches);


		if($check)
		{
			echo "Invalid Expression!";
			return;
		}

/* code for checking for valid expressions?
 /		preg_match("/([0-9]+[+-\/*])+([0-9]+)/",$calc_input, $matches);
 /		var_dump($matches);
 /		echo preg_last_error();
 */

/* code for evaluating valid expression */
		$result = eval("return $calc_input;");
		echo $result;
	}

 ?>
</p>
</form>
</body>
</html>
