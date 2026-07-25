<?php
include_once('settings.inc');
// Use the $db_file variable from settings.inc
$db = new PDO("sqlite:" . $db_file);

$db->exec("CREATE TABLE IF NOT EXISTS songdb (song_id integer PRIMARY KEY AUTOINCREMENT, artist text, title  TEXT, combined TEXT UNIQUE)"); 
$db->exec("CREATE TABLE IF NOT EXISTS state (accepting bool, serial integer NOT NULL)");
$db->exec("INSERT OR IGNORE INTO state (rowid,accepting,serial) VALUES(0,0,1)");
$db->exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_songstrings ON songdb(combined)");
$db->exec("CREATE TABLE IF NOT EXISTS requests (request_id integer PRIMARY KEY AUTOINCREMENT, artist TEXT, title TEXT, singer TEXT, request_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, key_change INTEGER DEFAULT 0)");
// Migration for databases created before key_change existed. Wrapped in
// try/catch because PDO's default error mode varies by PHP version (PHP 8.1+
// defaults to ERRMODE_EXCEPTION, older versions default to ERRMODE_SILENT),
// and this call is expected to fail harmlessly once the column already exists.
try {
	$db->exec("ALTER TABLE requests ADD COLUMN key_change INTEGER DEFAULT 0");
} catch (PDOException $e) {
	// Column already exists, nothing to do.
}

$db->exec("CREATE TABLE IF NOT EXISTS singers (singer_id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE NOT NULL, password_hash TEXT NOT NULL, created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
$db->exec("CREATE TABLE IF NOT EXISTS favorites (favorite_id INTEGER PRIMARY KEY AUTOINCREMENT, singer_id INTEGER NOT NULL, artist TEXT NOT NULL, title TEXT NOT NULL, added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, UNIQUE(singer_id, artist, title))");
$db->exec("CREATE INDEX IF NOT EXISTS idx_favorites_singer ON favorites(singer_id)");

$db->exec("CREATE TABLE IF NOT EXISTS rotation (rotation_id INTEGER PRIMARY KEY AUTOINCREMENT, singer_id INTEGER, name TEXT, position INTEGER, regular INTEGER DEFAULT 0, is_current INTEGER DEFAULT 0)");
// Rotation gets its own serial, separate from the main "requests/songdb changed"
// serial, so that pushing a rotation update doesn't trigger OpenKJ's own poll
// loop into needlessly re-fetching requests and venues.
try {
	$db->exec("ALTER TABLE state ADD COLUMN rotation_serial INTEGER NOT NULL DEFAULT 1");
} catch (PDOException $e) {
	// Column already exists, nothing to do.
}

// Singer accounts are only used by this web songbook, on top of the shared
// browser session. Sessions are how we know which singer is "logged in".
if (session_status() === PHP_SESSION_NONE) {
	session_start();
}

$path = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
$songdbtable = "songdb";
$requeststable = "requests";

if (isset($_SERVER['REFERER'])) $referer = $_SERVER['REFERER']; 
function siteheader($title) 
{
	global $venueName;
	global $screensize;
	echo "<html><head>
		<link href='https://fonts.googleapis.com/css?family=Audiowide' rel='stylesheet'>
		<link href='https://fonts.googleapis.com/css?family=Roboto' rel='stylesheet'>
		<title>$venueName Karaoke Songbook</title>
		<link rel=stylesheet type=\"text/css\" href=venuestyle.css />
		<script type=\"text/javascript\">
    			function submitreq(varid){
        			window.location = \"./submitreq.php?id=\" + varid;
    			}
		</script>
	      </head><body>";
}

function sitefooter() {
	echo "</div></body></html>";
}

function navbar($backurl) 
{
	if ($backurl == "")
		$backurl = "index.php";
	global $screensize;
	echo "<div class=navbar>
	<span class=title>OpenKJ Songbook</span>
	</div><div class=mainbody><span class=backbtn><a class=button href=\"$backurl\" class=navbar id=backlink>Back</a> <a class=button href=\"rotation.php\" class=navbar id=rotationlink>Who's Up Next</a></span><span class=acctwrap>";
	accountLinks();
	echo "</span>";
}

function setAccepting($accepting)
{
	global $db;
	if ($accepting == 1)
	{
		echo("setting accepting to 1");
		$db->exec("UPDATE state SET accepting=1");
	}
	else
	{
		echo("setting accepting to 0");
		$db->exec("UPDATE state SET accepting=0");
	}
}

function getAccepting()
{
	global $db;
	$accepting = false;
        foreach ($db->query("SELECT accepting FROM state LIMIT 1") as $row)
        {
                $accepting = $row['accepting'];
        }
	return $accepting;
}

function searchform() 
{
	global $db;
	global $venue_id;
	if (!getAccepting())
	{
		echo "<br><br><h2>Sorry, requests are not being accepted at this time</h2>";
	}
	else
	{
		global $url_name;
		global $screensize;
		echo "<br><p><form method=get action=search.php>Song search: <input type=text name=q autofocus autocomplete=off><input type=submit value=Search></form></p>";
		echo '<p class=info>You may enter any part of the artist and/or title of the song.  Partial words are allowed.</p>
			<p class=info>For example "pai bla stone" would match "Rolling Stones, The - Paint it black".</p>';
		if (isLoggedIn())
		{
			echo '<p class=info>Tap the &#9734; next to a search result to save it to <a href="favorites.php">My Favorites</a>.</p>';
		}
		else
		{
			echo '<p class=info><a href="register.php">Create a free account</a> to save your favorite songs and request them again in one tap.</p>';
		}
	}
}



function getSerial()
{
	global $db;
	$serial = -1;
	foreach ($db->query("SELECT serial FROM state LIMIT 1") as $row)
        {
        	$serial = (int)$row['serial'];
        }
	return $serial;
}

function newSerial()
{
        global $db;
        $serial = getSerial();
        $newSerial = mt_rand(0,99999);
        while ($newSerial == $serial)
        {
                $newSerial = mt_rand(0,99999);
        }
        $db->exec("UPDATE state SET serial=$newSerial");
        return $newSerial;
}

function getRotationSerial()
{
	global $db;
	$serial = -1;
	foreach ($db->query("SELECT rotation_serial FROM state LIMIT 1") as $row)
        {
        	$serial = (int)$row['rotation_serial'];
        }
	return $serial;
}

function newRotationSerial()
{
        global $db;
        $serial = getRotationSerial();
        $newSerial = mt_rand(0,99999);
        while ($newSerial == $serial)
        {
                $newSerial = mt_rand(0,99999);
        }
        $db->exec("UPDATE state SET rotation_serial=$newSerial");
        return $newSerial;
}

function getVenue()
{
	// We don't really do multiple venues in standalone, just fake it
        global $db;
	global $venueName;
        $serial = -1;
	$venue = array();
	$venue['venue_id'] = $venue_id;
        $venue['accepting'] = getAccepting();
	$venue['name'] = $venueName;
	$venue['url_name'] = "none";
	return $venue;
}

function getVenues()
{
	// We don't really do multiple venues in standalone, just fake it
	global $db;
	global $venueName;
	$venues = array();
	$venue['venue_id'] = 0;
        $venue['accepting'] = getAccepting();
        $venue['name'] = $venueName;
	$venue['url_name'] = "none";
	$venues['venues'][] = $venue;
	return $venues;
}

function isLoggedIn()
{
	return isset($_SESSION['singer_id']);
}

function currentSingerId()
{
	return isset($_SESSION['singer_id']) ? (int)$_SESSION['singer_id'] : null;
}

function currentUsername()
{
	return $_SESSION['username'] ?? null;
}

// Sends the browser to $redirectTo and stops execution if nobody is logged
// in. Must be called before any HTML has been echoed on the calling page.
function requireLogin($redirectTo = 'login.php')
{
	if (!isLoggedIn())
	{
		header("Location: $redirectTo");
		exit();
	}
}

function accountLinks()
{
	if (isLoggedIn())
	{
		$username = htmlspecialchars(currentUsername());
		echo "<span class=accountbar>Signed in as $username &middot; <a class=navbar href=\"favorites.php\">My Favorites</a> &middot; <a class=navbar href=\"logout.php\">Log out</a></span>";
	}
	else
	{
		echo "<span class=accountbar><a class=navbar href=\"login.php\">Log in</a> &middot; <a class=navbar href=\"register.php\">Create account</a></span>";
	}
}

function keyChangeSelect()
{
	echo "Key change (optional): <select name=keychange>";
	for ($i = 6; $i >= -6; $i--)
	{
		$label = $i == 0 ? "No change" : ($i > 0 ? "+$i" : $i);
		$selected = $i == 0 ? " selected" : "";
		echo "<option value=\"$i\"$selected>$label</option>";
	}
	echo "</select><br>";
}

function getRequests()
{
        global $db;
        $requests = array();
        $result = $db->query("SELECT request_id,artist,title,singer,key_change,strftime('%s', request_time) AS unixtime FROM requests");
	if ($result)
	{
        	foreach ($result as $row)
        	{
        	        $request['request_id'] = (int)$row['request_id'];
        	        $request['artist'] = $row['artist'];
        	        $request['title'] = $row['title'];
        	        $request['singer'] = $row['singer'];
        	        $request['request_time'] = (int)$row['unixtime'];
        	        $request['key_change'] = (int)$row['key_change'];
        	        $requests['requests'][] = $request;
        	}
	}
        return $requests;
}

function getRotation()
{
        global $db;
        $rotation = array();
        $result = $db->query("SELECT rotation_id,singer_id,name,position,regular,is_current FROM rotation ORDER BY position ASC");
	if ($result)
	{
        	foreach ($result as $row)
        	{
        	        $singer['singer_id'] = (int)$row['singer_id'];
        	        $singer['name'] = $row['name'];
        	        $singer['position'] = (int)$row['position'];
        	        $singer['regular'] = (bool)$row['regular'];
        	        $singer['is_current'] = (bool)$row['is_current'];
        	        $rotation['rotation'][] = $singer;
        	}
	}
        return $rotation;
}

?>
