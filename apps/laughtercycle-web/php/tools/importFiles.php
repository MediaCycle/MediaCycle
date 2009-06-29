<?php
/**
 * @brief importFiles.php
 * @author Alexis Moinet
 * @date 29/06/2009
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
 * script to directly import a bunch of files in the DB
 * 
 */

require_once '../config.php';
require_once '../setup.php';

$dir = "/home/alexis/Bureau/aif/";

ob_implicit_flush(true);
echo "disabled - edit php code to use this";
exit();
if (is_dir($dir)) {
    if ( ($dh = opendir($dir)) ) {
        while (($file = readdir($dh)) !== false) {
			echo intval(is_file($dir . $file)) . " : " . $dir . $file . "<br/>";
			if (is_file($dir . $file) && $file != "." && $file != "..") {
				echo "fichier : $file : type : " . filetype($dir . $file) . "\n<br/>";
				$uuid = gfGetUUID();
				$parts = explode('.', $file);
				$ext = $parts[count($parts)-1];
				if ($ext == "wav" || $ext == "aif") {
					$filename = $uuid . '.' . "wav";
					if ($ext == "aif") {
						$command = "ffmpeg -i " . $dir . $file . " " . $gConfig["filepath"].$filename;
						$res = exec($command, $output);
					} else {
						if (copy($dir . $file, $gConfig["filepath"].$filename)) {
							$gOut->nl("The file ".  $dir . $file . " has been moved to " . $gConfig["filepath"].$filename);
						} else {
							$gOut->nl("There was an error uploading the file $file, please try again!");
						}
					}
					$lcfile = LCFile::createNewFile($parts[0], $uuid, 'audio', 'upload');
					LCFile::convertWavToFlv($uuid);
/*
					echo "MC : add file<br/>";
					MediaCycle::addFile($lcfile);
					echo "MC : get thumbnail<br/>";
					MediaCycle::getThumbnailXml($lcfile);
 
 */
				}
				
			}
        }
		echo "no more files\n";
        closedir($dh);
    } else {
		echo "could not open dir\n";
	}
} else {
	echo "not a dir\n";
}

echo $gOut->toHtml();

?>
