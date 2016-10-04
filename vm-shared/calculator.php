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
		if (preg_match('/[a-z]/i', $calc_input, $matches)) 
		{
			echo "Invalid Expression!";
			return;
		}


 		//var_dump($matches);
 		//echo preg_last_error();
 		
 

/* code for evaluating valid expression */
		$result = eval("return $calc_input;");
		//echo $result;
	}

 ?>
</p>
</form>
</body>
</html>
