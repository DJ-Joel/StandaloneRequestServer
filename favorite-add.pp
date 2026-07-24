<?php
include('global.inc');
requireLogin('login.php');

$songId = isset($_GET['id']) ? (int)$_GET['id'] : 0;

$stmt = $db->prepare("SELECT artist,title FROM songdb WHERE song_id = :id");
$stmt->execute(array(':id' => $songId));
$row = $stmt->fetch(PDO::FETCH_ASSOC);

if ($row)
{
	// Favorites store the artist/title text rather than song_id so they keep
	// working even if the KJ reloads or renumbers their song database later.
	$ins = $db->prepare("INSERT OR IGNORE INTO favorites (singer_id, artist, title) VALUES (:sid, :artist, :title)");
	$ins->execute(array(':sid' => currentSingerId(), ':artist' => $row['artist'], ':title' => $row['title']));
}

header("Location: favorites.php?added=1");
exit();
?>
