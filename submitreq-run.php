<?php 
include('global.inc');
#header("Refresh: 15; URL=");
siteheader("Song Submitted");



$songid = $_GET['songid'];
$singer = $_GET['singer'];
$keychange = isset($_GET['keychange']) ? (int)$_GET['keychange'] : 0;
if ($keychange < -6 || $keychange > 6) {
	$keychange = 0;
}
if ($singer == '') {
	navbar($_SERVER['HTTP_REFERER']);
	echo "<p>Sorry, you must input a singer name.  Please go back and try again.</p>";
	die();

}
navbar("index.php");
$entries = null;
$wherestring = null;
$artist = '';
$title = '';
$sql = "SELECT artist,title FROM songdb WHERE song_id = $songid";
foreach ($db->query($sql) as $row) {
	$artist = $row['artist'];
	$title = $row['title'];
}
$stmt = $db->prepare("INSERT INTO requests (singer,artist,title,key_change) VALUES(:singer, :artist, :title, :keychange)");
$stmt->execute(array(":singer" => $singer, ":artist" => $artist, ":title" => $title, ":keychange" => $keychange));
newSerial();
$keychangeLabel = $keychange == 0 ? "No change" : ($keychange > 0 ? "+$keychange" : $keychange);
echo "<p>Song: $artist - $title</p>
      <p>Submitted for singer: $singer</p>
      <p>Key change: $keychangeLabel</p>
	<br><p>Please press back to return to the main screen</p>
";

sitefooter();
?> 
