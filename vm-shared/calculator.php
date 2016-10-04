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

		// checking characters that shouldn't be in the expression
		if (preg_match('/[^0-9+\-\/*.\ ]/', $calc_input, $matches)) 
		{
			echo "Invalid Expression!";
			return;
		}

		$matches = [];

		// check for when a negative number has a space between 
		// its minus sign and the digits
		if (preg_match('/[+\-\/*]\s*-\s/', $calc_input, $matches)) 
		{
			echo "Invalid Expression!";
			return;
		}		

		$matches = [];

		// check for leading zeros
		if (preg_match('/0+[0-9]/', $calc_input, $matches)) 
		{
			echo "Invalid Expression!";
			return;
		}				

		// check for '--' , replace with '- -' for eval to work
		$calc_input = preg_replace('/-\s*-/', '- -', $calc_input);
		echo $calc_input . "<br>";

		// check for a divide by zero error
		if (preg_match("/\/[\s\-]*0/", $calc_input, $matches))
		{
			echo "Division by zero error!";
			return;
		}

		/* code for evaluating valid expression */
		$result = eval("return $calc_input;");
		$result = ($result == false) ? "Invalid expression!" : $result;
		echo $result;
	}

 ?>
</p>
</form>
</body>
</html>
