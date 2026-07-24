<?php
include('global.inc');

$error = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
	$username = trim($_POST['username'] ?? '');
	$password = $_POST['password'] ?? '';

	$stmt = $db->prepare("SELECT singer_id, username, password_hash FROM singers WHERE username = :u");
	$stmt->execute(array(':u' => $username));
	$row = $stmt->fetch(PDO::FETCH_ASSOC);

	if ($row && password_verify($password, $row['password_hash']))
	{
		session_regenerate_id(true);
		$_SESSION['singer_id'] = $row['singer_id'];
		$_SESSION['username'] = $row['username'];
		$redirect = isset($_GET['redirect']) ? $_GET['redirect'] : 'favorites.php';
		// Only allow relative redirects within this app.
		if (preg_match('/^[a-zA-Z0-9_\-]+\.php(\?[a-zA-Z0-9_=&\-]*)?$/', $redirect))
		{
			header("Location: $redirect");
		}
		else
		{
			header("Location: favorites.php");
		}
		exit();
	}
	else
	{
		$error = 'Incorrect username or password.';
	}
}

siteheader('Log In');
navbar('index.php');

if ($error !== '')
{
	echo "<br><p class=error>" . htmlspecialchars($error) . "</p>";
}

$prefillUsername = htmlspecialchars($_POST['username'] ?? '');

echo "<br><form method=post action=login.php>
Username:<br><input type=text name=username autocomplete=username autofocus value=\"$prefillUsername\"><br>
Password:<br><input type=password name=password autocomplete=current-password><br>
<input type=submit value=\"Log in\">
</form>
<p class=info>New here? <a href=\"register.php\">Create an account</a></p>";

sitefooter();
?>
