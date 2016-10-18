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

		// checking characters that shouldn't be in the expression
		if (preg_match('/[^0-9+\-\/*.\s]/', $calc_input)) 
		{
			echo "Invalid Expression!";
			return;
		}

		// check for when a negative number has a space between 
		// its minus sign and the digits
		if (preg_match('/(\A|[+\-\/*])\s*-\s/', $calc_input)) 
		{
			echo "Invalid Expression!";
			return;
		}		

		// check for leading zeros
		if (preg_match('/[+\-\/*\s]+0+[0-9]/', $calc_input)
			|| preg_match('/\A0[0-9]/', $calc_input)) 
		{
			echo "Invalid Expression!";
			return;
		}			

		// check for leading operator
		if (preg_match('/^\s*[+\/\*\.]/', $calc_input))
		{
			echo "Invalid Expression!";
			return;
		}	

		// check for '--' , replace with '- -' for eval to work
		$calc_input = preg_replace('/-\s*-/', '- -', $calc_input);
		//echo $calc_input . "<br>";

		// check for decimal without any number after it
		if (preg_match('/\.\s*[+\-\/*]?\Z/', $calc_input))
		{
			echo "Invalid expression!";
			return;
		}

		// check for decimal without any number before it
		if (preg_match('/[^0-9]\./', $calc_input))
		{
			echo "Invalid expression!";
			return;
		}

		// check for a divide by zero error
		// looks for /0 with or without a decimal after the zero and zeroes
		if (preg_match('/\/[\s-]*0\.?0*\s*\Z/', $calc_input))
		{
			echo "Division by zero error!";
			return;
		}

		// check for -0 or spaces before or after just 0
		// check for just 0
		if (preg_match('/^[\s]*[\-]?0[\s]*$/', $calc_input))
		{
			echo ($calc_input === "0") ? "" : 0;
			return;
		}

		/* code for evaluating valid expression */
		$result = eval("return $calc_input;");
		echo $result;
	}

 ?>
</p>
</form>
</body>
</html>
