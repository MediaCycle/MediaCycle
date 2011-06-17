<?php
/**
 * @brief test-mediacycle.php
 * @author Alexis Moinet
 * @date 17/06/2011
 * @copyright (c) 2011 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/
?>

<?php
//everything starts here in "index.php"

require_once 'config.php';
require_once 'include.php';

global $gDB; //see config.php
global $gConfig; //see config.php
global $gOut;
global $gUser;

session_start();

//open mysql connexion
$gDB = new DatabaseMySql();
$gDB->connect();

//create an output class. Output contains all the page content that is common to all the page : side-column, header, footer, menu, ... (+debug)
$gOut = new Output();
//Ouptut works with getter and setter : set variable 'sitename' to the value in $gConfig["sitename"]
$gOut->setContent('sitename', $gConfig["sitename"]);
//$gOut->setContent('pagename', "Home");

//for debug (if any error occurs anywhere in the code, we should put this to true and set errormsg with some informing content)
//so this is one of the first thing to do (before anything happens)
$gOut->setContent('error', false);
$gOut->setContent('errormsg', "");

//create a User (loaded from cookies/session if logged in or 'anonymous' user if not logged in)
$gUser = new User();

$fileid = 41;
$file = new LCFile($fileid);

$result = Mediacycle::addFile($file);

$gOut->setContent('result',intval($result));

$gOut->printContent('result');

echo $gOut->toHtml();

?>
