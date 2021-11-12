<?php
/**
 * @brief config.php
 * @author Stéphane Dupont
 * @date 05/04/2011
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

/*
 * MySQL DB connexion information
 */
global $gDBhost, $gDBname, $gDBuser, $gDBpass;

$gDBhost = "localhost";
$gDBname = "numediart";
$gDBuser = "numediart";
$gDBpass = "numediart";

/*
 * Configuration of the website
 */
global $gConfig;

$gConfig["sitename"] = "LaughterCycle";
$gConfig["minpasswordlength"] = 6;
$gConfig["nlastfilesplayed"] = 7;//how many file do we display in the "last files played" column
$gConfig["nlastcomments"] = 5;//how many comments do we display in the "last comments" column
//$gConfig["filepath"] = "/var/www/LCimages/";//where are the waveform XML files (to display the audio waveforms in the flash player)
$gConfig["filepath"] = "/var/mediacycle/";//where are the waveform XML files (to display the audio waveforms in the flash player)
$gConfig["fileurl"] = $_SERVER['SERVER_ADDR'] . "/LCimages/";//where are they in a http://myserver/mypath fashion
$gConfig["fileextensions"] = array("mp3","flv","wav"); //we accept mp3 and flv at upload
$gConfig["streampath"] = "/opt/red5/red5/dist/webapps/oflaDemo/streams/"; // when recording with the flash application on the website, where are the recording stored ? There they're

/*
 * MEDIACYCLE CONFIG
 */
//if enabled, website will try to communicate with a MediaCycle server
//if no server exist, website will hang indefinitely (should put a timeout)
$gConfig["mediacycle"]["enable"] = true;

//ip address of a mediacycle server (in our case it is always local range (192.168.*.*), but it can be any IP
//$gConfig["mediacycle"]["ip"] = "192.168.1.153";
//$gConfig["mediacycle"]["ip"] = "169.254.49.254";
//$gConfig["mediacycle"]["ip"] = "192.168.3.168";
$gConfig["mediacycle"]["ip"] = "localhost";

//listening port of the mediacycle server
$gConfig["mediacycle"]["port"] = "12345";

//when a library of laughter is created, we can ask mediacycle to save it with a given name
$gConfig["mediacycle"]["libraryname"] = "LClib";

//how many knn (nearest-neighbours) do we want ?
$gConfig["mediacycle"]["knn"] = 5;

/*
 * list of the static pages of the website
 */
global $gPages,$gHomePage;

//$gPages is an array key=> value
// each key corresponds to an url of the kind http://myserver/index.php?title=key
// every value gives the corresponding php class to be loaded
// e.g. http://myserver/index.php?title=home will display the content generated by the class "Home" (located in the folder "includes")

$gPages = array("home"      => "Home",
                "file"      => "LCFile",
                "login"     => "UserLogin",
                "upload"    => "UploadFile",
                "user"      => "User",
                "query"     => "LCQuery");

$gHomePage= 'home';//website homepage (if url entered is http://myserver/index.php)
?>
