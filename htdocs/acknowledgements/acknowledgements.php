<?php

/**
 * Publicly available generator for acknowledgements
 *
 * PHP Version 5
 *
 * @category Loris
 * @package  Behavioral
 * @author   Justin Kat <justin.kat@mail.mcgill.ca>
 * @license  http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link     https://github.com/aces/Loris
 */

require_once __DIR__ . "/../../vendor/autoload.php";

$client = new NDB_Client();
$client->makeCommandLine();
$client->initialize();

$config = NDB_Config::singleton();
$db     = Database::singleton();

$factory = NDB_Factory::singleton();
$baseurl = $factory->settings()->getBaseURL();

$css = $config->getSetting('css');

$publication_date = $_GET["date"];

$columns = array(
            full_name     => 'Full Name',
            citation_name => 'Citation Name',
            affiliations  => 'Affiliations',
            degrees       => 'Degrees',
            roles         => 'Roles',
            start_date    => 'Start Date',
            end_date      => 'End Date',
            present       => 'Present?',
           );

$keysAsString   = implode(', ', array_keys($columns));
$valuesAsString = implode('","', array_values($columns));
$valuesAsStringWithQuotes = '"' . $valuesAsString . '"';

$results = $db->pselect(
    "SELECT " . $keysAsString .
    " FROM acknowledgements
    WHERE start_date <= :publication_date
    ORDER BY ordering ASC",
    array('publication_date' => $publication_date)
);
echo "<html>";
echo "<link rel='stylesheet' href='{$baseurl}/../../{$css}' type='text/css' />";
echo "<link type='text/css' href='{$baseurl}/../../css/loris-jquery/jquery-ui-1.10.4.custom.min.css' rel='Stylesheet' />";
echo "<link rel='stylesheet' href='{$baseurl}/../../bootstrap/css/bootstrap.min.css'>";
echo "<link rel='stylesheet' href='{$baseurl}/../../bootstrap/css/custom-css.css'>";

echo '<div id="tabs" style="background: white">';
echo '<div class="tab-content">';
echo '<div class="tab-pane active">';
echo "<table class='table table-hover table-primary table-bordered table-unresolved-conflicts dynamictable' border='0'>";
echo "<thead><tr class='info'>";
foreach ($columns as $k => $v) {
     echo "<th>" . $v . "</th>";
}
echo "</tr></thead>";
foreach ($results as $k => $v) {
    echo "<tr>";
    foreach ($v as $key => $value) {
        echo "<td>" . ucwords(str_replace(",",", ",str_replace("_"," ",$value))) . "</td>";
    }
    echo "</tr>";
}
echo "</table>";

echo "<br>";
echo "CSV:";
echo "<br>";
echo $valuesAsStringWithQuotes;
echo "<br>";
foreach ($results as $k => $v) {
    $result = implode('","', array_values($v));
    echo '"' . $result . '"';
    echo "<br>";
}


echo "</html>";

?>
