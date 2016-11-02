<html>
<head>
	<link rel="stylesheet" href="./style.css" type="text/css">
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

<div id="container">
<!-- will be used to display the content of each page -->
	<div id="main-content">
		<form>
			Actors:<input type="radio" value="actors" name="group" checked>
			Movies:<input type="radio" value="movies" name="group"><br>
			Search:<input type="text" placeholder="Search">
			<input type="button" value="submit">
		</form>


	</div>
</div>

</body>
</html>