<?php
/**
 * @brief setup.php
 * @author Alexis Moinet
 * @date 05/06/2009
 * @copyright (c) 2009 – UMONS - Numediart
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
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
require_once 'include.php';

global $gDB;
global $gOut;
global $gUser;
global $gConfig;

session_start();

$gDB = new DatabaseMySql();
$gDB->connect();

$gOut = new Output();
$gOut->setContent('sitename', $gConfig["sitename"]);
//$gOut->setContent('pagename', "Home");

$gOut->setContent('error', false);
$gOut->setContent('errormsg', "");

$gUser = new User();

//parse $_GET and make stuff
Action::parse();

//set all the content
$gOut->setContent('userlinks', $gUser->getLinks());

?>
