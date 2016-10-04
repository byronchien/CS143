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
<<<<<<< HEAD

	if ($_POST["calc_input"] != NULL)
=======
	if ($_GET["calc_input"] != NULL)
>>>>>>> f24c4087bc5c9af223b4e2dd373c7736da4b831d
	{
		$calc_input = $_GET["calc_input"];
		$matches = [];
<<<<<<< HEAD

		// checks for any characters not allowed
		$check = preg_match("/[^0-9+\-\/*.\ ]+/", $calc_input, $matches);
		$check += preg_match("/[^1-9]
		
		$check += preg_match("/[0-9]+\ [0-9]+/", $calc_input, $matches);


		if($check)
=======
// code for checking for valid expressions?
		// check for alpha characters
		if (preg_match('/[^0-9+\-\/*.\ ]/', $calc_input, $matches)) 
>>>>>>> f24c4087bc5c9af223b4e2dd373c7736da4b831d
		{
			echo "Invalid Expression!";
			return;
		}

<<<<<<< HEAD
/* code for checking for valid expressions?
 /		preg_match("/([0-9]+[+-\/*])+([0-9]+)/",$calc_input, $matches);
 /		var_dump($matches);
 /		echo preg_last_error();
 */
=======
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
 		
 
>>>>>>> f24c4087bc5c9af223b4e2dd373c7736da4b831d

/* code for evaluating valid expression */
		$result = eval("return $calc_input;");
		echo $result;
	}

 ?>
</p>
</form>
</body>
</html>
