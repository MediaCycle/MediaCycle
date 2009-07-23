<?php
/**
 * @brief DB2MC.php
 * @author Alexis Moinet
 * @date 23/07/2009
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
 * This file is used to "transmit" to MediaCycle all the files referenced in the MySQL database.
 */

require_once '../config.php';
require_once '../setup.php';

//step 1 : get a list of the files (actually, only their id)
$query = "SELECT id FROM files";

$result = $gDB->query($query);

ob_implicit_flush(true);
echo "# : ". $gDB->nf() ."<br/>";

//put all the ids in an array
$ids = array();
while ($gDB->next_record()) {
    $ids[] = $gDB->f("id");
}

//step 2 : browse the list of id and do something
foreach ($ids as $id) {
    //create a file instance from an id
    if ($id > 0) {
        $file = new LCFile($id);

        echo $file->getId() . " : " . $file->getName() . "<br/>";

        //get the *.flv file (on the red5 server) and create the *.wav (wherever set in config.php, e.g. the apache server)
        LCFile::convertFlvToWav($file->getName());
        echo "MC : add file<br/>";
        //send the file to MediaCycle for analysis/integration in MediaCycle's DB
        MediaCycle::addFile($file);
        echo "MC : get thumbnail<br/>";
        //get the thumbnail of the file from MediaCycle
        MediaCycle::getThumbnailXml($file);
        echo "<br/>";
    }
}

echo "MC : save library<br/>";
//ask to MediaCycle to save it's current DB state
MediaCycle::saveLibrary();

$result = MediaCycle::getkNN(new LCFile(1),2);
print_r($result);

?>
