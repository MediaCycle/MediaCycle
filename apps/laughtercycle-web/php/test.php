<?php
/**
 * @brief test.php
 * @author Alexis Moinet
 * @date 29/05/2009
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
require_once 'includes/User.php';
include_once 'config.php';

global $wgUser;

echo "GET<br/>";
echo $_GET["action"] . "<br/>" ;
echo $_GET["filename"] . "<br/>" ;
echo "POST<br/>";
echo $_POST["action"] . "<br/>" ;
echo $_POST["filename"] . "<br/>" ;


$password = "password";
$salt = sha1(uniqid(mt_rand(), true));
$crypted = sha1($salt . '-' . sha1($password));
echo "bt : " . $crypted . "<br/>" . strlen($crypted);

$wgUser = new User();

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title></title>
    </head>
    <body>
        <?php
		echo "<br/>myid : "  .$wgUser->getId();
        // put your code here
        ?>
    </body>
</html>

<?php

?>