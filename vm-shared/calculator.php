<html>
<head>
</head>
<body>

<h1> CALCULATOR </h1>
<form action="calculator.php" method="get">
<p> Input string: 
	<input type="text" name="calc_input">
	<input type="submit">
</p>

<p>
<?php
	if ($_GET["calc_input"] != NULL)
	{
		$calc_input = $_GET["calc_input"];
		$matches = [];
// code for checking for valid expressions?
		// check for alpha characters
		if (preg_match('/[^0-9+\-\/*.\ ]/', $calc_input, $matches)) 
		{
			echo "Invalid Expression!";
			return;
		}

		// check for when a negative number has a space between 
		// its minus sign and the digits
		if (preg_match('/[+\-\/*]\s*-\s/', $calc_input, $matches)) 
		{
			echo "Invalid Expression!";
			return;
		}		

		// check for '--' , replace with '- -' for eval to work
		$calc_input = preg_replace('/-\s*-/', '- -', $calc_input);
		echo $calc_input . "<br>";

 		//var_dump($matches);
 		//echo preg_last_error();
 		
 

/* code for evaluating valid expression */
		$result = eval("return $calc_input;");
		echo $result;
	}

 ?>
</p>
</form>
</body>
</html>
