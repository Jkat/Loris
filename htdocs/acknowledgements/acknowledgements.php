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
    AND (DATEDIFF(end_date,start_date) > 90 OR end_date='0000-00-00')
    ORDER BY ordering ASC",
    array('publication_date' => $publication_date)
);


// TODO: Transfer all this into a proper .tpl file

$tpl_data['baseurl']     = $config->getSetting('url');
$tpl_data['css']         = $config->getSetting('css');
$tpl_data['columns'] = $columns;
$tpl_data['results'] = $results;







if ($_SERVER['REQUEST_METHOD'] === 'POST') {

// Real CSV implementation
$CSVheaders = array();
$CSVdata = array();
foreach ($columns as $k => $v) {
    array_push($CSVheaders, $v);
}
foreach ($results as $k => $v) {
    array_push($CSVdata, $v);
}


$toCSV['headers']    = $CSVheaders;
$toCSV['data'] = $CSVdata;

$CSV = Utility::arrayToCSV($toCSV);

header("Content-Type: text/plain");
header('Content-Disposition: attachment; filename=data.csv');
header("Content-Length: " . strlen($CSV));
echo $CSV;
exit();
//readfile($CSV);

//print(Utility::arrayToCSV($toCSV));

} else {




/*
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



echo "<form action='' method='post'>";
echo "</div>";
echo "<input class='btn btn-primary' type='submit' value='Download as CSV'>";
echo "</div>";
echo "</form>";

//echo "<button type='button'>Download as CSV</button>";


echo "</html>";
*/

}





//Output template using Smarty
$smarty = new Smarty_neurodb;
$smarty->assign($tpl_data);
$smarty->display('acknowledgements.tpl');


?>
