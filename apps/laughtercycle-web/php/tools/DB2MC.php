<?php
/**
 * @brief DB2MC.php
 * @author Alexis Moinet
 * @date 30/06/2009
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

require_once '../config.php';
require_once '../setup.php';

$query = "SELECT id FROM files";

$gDB2 = new DatabaseMySql();
$gDB2->connect();
$gDB2->query($query);

ob_implicit_flush(true);
echo "# : ". $gDB2->nf() ."<br/>";
while ($gDB2->next_record()) {
    $id = $gDB2->f("id");
    $file = new LCFile($id);

    echo $file->getId() . " : " . $file->getName() . "<br/>";

    LCFile::convertFlvToWav($file->getName());
    echo "MC : add file<br/>";
    MediaCycle::addFile($file);
    echo "MC : get thumbnail<br/>";
    MediaCycle::getThumbnailXml($file);
    echo "<br/>";
}

echo "MC : save library<br/>";
MediaCycle::saveLibrary();

?>
