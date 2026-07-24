<?php
include('global.inc');

$error = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
	$username = trim($_POST['username'] ?? '');
	$password = $_POST['password'] ?? '';
	$confirm = $_POST['confirm'] ?? '';

	if ($username === '' || strlen($username) < 3 || strlen($username) > 30 || !preg_match('/^[A-Za-z0-9_ ]+$/', $username))
	{
		$error = 'Username must be 3-30 characters (letters, numbers, spaces, and underscores only).';
	}
	elseif (strlen($password) < 6)
	{
		$error = 'Password must be at least 6 characters.';
	}
	elseif ($password !== $confirm)
	{
		$error = 'Passwords do not match.';
	}
	else
	{
		$check = $db->prepare("SELECT singer_id FROM singers WHERE username = :u");
		$check->execute(array(':u' => $username));
		if ($check->fetch())
		{
			$error = 'That username is already taken. Please choose another.';
		}
		else
		{
			$hash = password_hash($password, PASSWORD_DEFAULT);
			$ins = $db->prepare("INSERT INTO singers (username, password_hash) VALUES (:u, :p)");
			$ins->execute(array(':u' => $username, ':p' => $hash));
			$singerId = $db->lastInsertId();
			// Regenerate the session id on privilege change to avoid session fixation.
			session_regenerate_id(true);
			$_SESSION['singer_id'] = $singerId;
			$_SESSION['username'] = $username;
			header("Location: favorites.php?welcome=1");
			exit();
		}
	}
}

// Nothing above should have produced output yet if we're redirecting, so it's
// safe to start rendering the page now.
siteheader('Create Account');
navbar('index.php');

if ($error !== '')
{
	echo "<br><p class=error>" . htmlspecialchars($error) . "</p>";
}

$prefillUsername = htmlspecialchars($_POST['username'] ?? '');

echo "<br><form method=post action=register.php>
Username:<br><input type=text name=username autocomplete=username autofocus value=\"$prefillUsername\"><br>
Password:<br><input type=password name=password autocomplete=new-password><br>
Confirm password:<br><input type=password name=confirm autocomplete=new-password><br>
<input type=submit value=\"Create account\">
</form>
<p class=info>Save your favorite songs here and request them again with one tap next time.</p>
<p class=info>Already have an account? <a href=\"login.php\">Log in</a></p>";

sitefooter();
?>
