<?php
/**
 * @brief File.php
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
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of File
 *
  */
class LCFile {
	private $id, $title, $path , $type , $recording, $geotag, $uploader, $uploadDate, $quality, $meanNote, $nNotes;

	public function __construct($id) {
		$this->id = 0;
		if (!loadFileFromId($id)) {}
	}

	public function loadFileFromId($id) {
		$this->id = intval($id);

		if (0 < $this->id) {
			global $gDB;

			$query = "SELECT * FROM users";
			$query .= sprintf(" WHERE id=%d",$this->id);
			$result = $gDB->query($query);

			if ($gDB->next_record()) {
				$this->id = $gDB->f("id");//useless
				$this->title = $gDB->f("title");
				$this->path = $gDB->f("path");
				$this->type = $gDB->f("type");
				$this->recording = $gDB->f("recording");
				$this->geotag = $gDB->f("geotag");
				$this->uploader = $gDB->f("uploader");
				$this->uploadDate = $gDB->f("uploadDate");
				$this->quality = $gDB->f("quality");//blob
				$this->meanNote = $gDB->f("meanNote");
				$gDB->free();

				$this->loadNumberOfNotes();

				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	public function loadNumberOfNotes() {
		$this->nNotes = 0;
		if ($this->id > 0) {
			global $gDB;

			$query = "SELECT COUNT(*) as n FROM comments";
			$query .= sprintf(" WHERE file_id=%d",$this->id);
			$result = $gDB->query($query);

			if ($gDB->next_record()) {
				$this->nNotes = $gDB->f("n");
			} 
		}
	}
	public function updateMeanNote() {
		//non-secure (mismatch possible if two users save a note at the same time)
		//$this->meanNote = ($this->meanNote*$this->nNotes + $newnote)/($this->nNotes+1);

		global $gDB;

		$query = "SELECT note FROM comments";
		$query .= sprintf(" WHERE file_id=%d",$this->id);
		$result = $gDB->query($query);

		$n = 0;
		$note = 0;

		while ($gDB->next_record()) {
			$note += $gDB->f("note");
			$n++;
		}

		$note/$n;
		//SQL update
		$query = sprintf("UPDATE files SET mean_note=%f WHERE id=%d", $note/$n, $this->id);
		$gDB->query($query);
	}

	public function getId() {
		return $this->id;
	}
	public function getTitle() {
		return $this->title;
	}
	public function getUploader() {
		return $this->uploader;
	}
	public function getUploadDate() {
		return $this->uploadDate;
	}
	public function getMeanNote() {
		return $this->meanNote;
	}

	public function toHtml() {
		$out = "";
		//insert file player here (*.swf object)
		return $out;
	}

	/*
	 * geotag and quality blob should also be arg for this function
	 */
	static public function createNewFile($title,$path,$type,$recording,$userId) {
		//SQL insertion
		$query = "INSERT INTO";
		$query .= " files ";
		$query .= "(`title`,`path`,`type`,`recording`,`uploader`,`upload_date`)";
		$query .= " VALUES ";
		$query .= "('" . $title . "','" . $path . "','" . $type . "','" . $recording . "'," . $userId . ",'" . Date::getDate() . "')";

		global $gDB;
		$gDB->query($query);

		if ($gDB->affected_rows()) {
				$id = mysql_insert_id($gDB->link_id());
		} else {
			return false;
		}

		$file = new File($id);
		return $file;
	}
}
?>
