<?php
/**
 * @brief File.php
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

/**
 * Description of File
 *
  */
class LCFile extends page {
	private $id, $title, $path , $type , $recording, $geotag, $uploader, $uploadDate, $quality, $meanNote, $nNotes;

	public function __construct($id=0) {
		$this->id = 0;
		if ($id > 0) {
			$this->loadFileFromId($id);
		}
	}

	public function loadFileFromId($id = 0) {
		if (0 < $id) {
			global $gDB;

			$query = "SELECT * FROM files";
			$query .= sprintf(" WHERE id=%d",intval($id));
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
			$this->id = 0;
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

	public function getComments() {
		if ($this->id > 0) {
			global $gDB;

			$query = "SELECT * FROM comments, users";
			$query .= sprintf(" WHERE file_id=%d AND users.id=comments.user_id",$this->id);
			$query .= " ORDER BY time DESC";
			
			$result = $gDB->query($query);

			$out .= '<div id="filecomments">Comments :';
			$out .= '<ul>';
			while ($gDB->next_record()) {
				$out .= '<li class="li-filecomment">';
				$out .= '<a href="index.php?title=user&id=' . $gDB->f("user_id") . '">' . $gDB->f("name") . '</a>';
				$out .= '<div class=filenote>'.$gDB->f("note").'</div>';
				$out .= '<div class=filecomment>'.$gDB->f("comment").'</div>';
				$out .= '</li>';
			}
			$out .= '</ul></div>';
			return $out;
		}
		return "";
	}

	public function getTags() {
		if ($this->id > 0) {
			global $gDB;

			$query = "SELECT * FROM filetags";
			$query .= sprintf(" WHERE file_id=%d",$this->id);
			$query .= " ORDER BY time DESC";

			$result = $gDB->query($query);

			while ($gDB->next_record()) {
				$this->tags[] = array($gDB->f(""));
			}
		}
	}

	public function updateMeanNote() {
		//non-secure solution:
		// $this->meanNote = ($this->meanNote*$this->nNotes + $newnote)/($this->nNotes+1);
		// (mismatch possible if two users save a note at the same time)

		//secure but more expansive solution :
		//should be called as little as possible
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
	public function getName() {
		return $this->path;
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
	public function fileExists() {
		if ($this->id == 0) return false;

		return true;
	}
	public function isCommentedBy($userId) {
		$userId = intval($userId);
		if ($this->id > 0 && $userId > 0) {
			global $gDB;

			$query = "SELECT * FROM comments";
			$query .= sprintf(" WHERE file_id=%d AND user_id=%d",$this->id,$userId);

			$result = $gDB->query($query);

			if ($gDB->next_record()) {
				return true;
			}
		}
		return false;
	}

	public function getPageName() {
		return "File" . $this->title;
	}
	public function toHtml() {
		global $gUser;
		$out = "";

		if ($this->fileExists()) {
		//insert file player here (*.swf object)
		//+file description (date/user)
		//+comments/notes & comment form
			$out .= $this->id . " : " . $this->title;
			
			$out .= LCPlayer::miniPlayer($this->getName());			
			$out .= $this->getComments();
			if (!$this->isCommentedBy($gUser->getId())) {
				//TODO comment/note form
				$out .= "<div>add comment form here</div>";
			}
		} else {
			$out .= "File not found";
		}
		return $out;
	}
	static public function factory() {
		$file = new LCFile();

		if (isset ($_GET["id"])) {
			$file->loadFileFromId(intval($_GET["id"]));
		} 

		return $file;
	}

	/*
	 * geotag and quality blob should also be arg for this function
	 */
	static public function createNewFile($title,$path,$type,$recording) {
		//SQL insertion
		global $gDB, $gUser;

		$title = $gDB->cleanInputString($title);
		$path = $gDB->cleanInputString($path);
		$type = $gDB->cleanInputString($type);
		$recording = $gDB->cleanInputString($recording);

		$query = "INSERT INTO";
		$query .= " files ";
		$query .= "(`title`,`path`,`type`,`recording`,`uploader`,`upload_date`)";
		$query .= " VALUES ";
		$query .= "('" . $title . "','" . $path . "','" . $type . "','" . $recording . "'," . $gUser->getId() . ",'" . Date::getDate() . "')";

		
		$gDB->query($query);

		if ($gDB->affected_rows()) {
				$id = mysql_insert_id($gDB->link_id());
		} else {
			return false;
		}

		$file = new LCFile($id);

		//TODO I guess there should be some DB insertion/analysis asked to laughtercycle here (or not ?)
		// we also could put a "find similar" button aside from the file player that would send an HTTP request to some php script ?

		return $file;
	}
	const N = 10;
	
	static public function displayLastNFilesPlayed($n) {
		$files_id = self::lastNFilesPlayed($n);
		$out = "";
		$out .= '<div id="lastlaugh">Currently laughing :';
		$out .= '<ul>';
		foreach ($files_id as $file_id) {
			$file = new LCFile($file_id);
			$out .= '<li class="li-lastlaugh">';
			$out .= '<div class="lc-player">';
			$out .= '<a href="index.php?title=file&id=' . $file->getId() . '">' . $file->getTitle() . '</a>';
			$out .= LCPlayer::miniPlayer($file->getName());
			$out .= '</div></li>';
		}
		$out .= '</ul></div>';

		return $out;
	}
	static public function lastNFilesPlayed($n = 0) {
		global $gDB, $gConfig;

		$n = ($n) ? intval($n) : $gConfig['nlastfilesplayed'];
		$nfiles = array();

		//$query = "SELECT * from history";
		$query = "SELECT DISTINCT(file_id) from history";
		$query .= " ORDER BY time DESC";
		$query .= sprintf(" LIMIT 0,%d",$n);

		$gDB->query($query);
		while ($gDB->next_record()) {
			$nfiles[] = intval($gDB->f("file_id"));
		}

		return $nfiles;
	}


	static public function lastNFilesRecorded($n = LCFile::N) {
		global $gDB;

		$n = intval($n);
		$nfiles = array();

		$query = "SELECT * from files";
		$query .= " ORDER BY time DESC";
		$query .= sprintf(" LIMIT 0,%d",$n);

		$gDB->query($query);
		while ($gDB->next_record()) {
			$nfiles[] = intval($gDB->f("id"));
		}

		return $nfiles;
	}
}


class UploadFile extends Page {

	public function  __construct() {
		;
	}

	static public function UploadForm() {
		$out = "";

		$out .= "<div id=\"uploadform\">\n";
		$out .= "<form name=\"uploadform\" action=\"index.php?action=file&do=upload\" method=\"POST\" enctype=\"multipart/form-data\">\n";
		$out .= "<table>\n";
		$out .= "<tr>\n";
		$out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"title\">Title : </label></td>\n";
		$out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"text\" name=\"title\" value=\"A very short file description\" size=\"20\"  onfocus='javascript:filename.value=\"\"' /></td>\n";
		$out .= "<tr/>\n";
		$out .= "<tr>\n";
		$out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"uploadedfile\">File : </label></td>\n";
		$out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"file\" name=\"uploadedfile\" value=\"\" size=\"20\" /></td>\n";
		$out .= "<tr/>\n";
		$out .= "<tr><td></td>\n";
		$out .= "<td class=\"submit\" style=\"text-align: left\"><input type=\"submit\" value=\"Upload\" name=\"ok\"/></td>\n";
		$out .= "<tr/>\n";
		$out .= "</table>\n";
		$out .= "</form>\n</div>\n";

		return $out;
	}

	static public function getUploadedFile() {
		global $gConfig, $gOut, $gUser;
		$gOut->nl(print_r($_FILES["uploadedfile"],true));
		//get the file an uuid and actually upload it from /tmp to some folder
		//TODO hash the folder into sub-folders (cf. mediawiki and al.)
		if (is_file($_FILES["uploadedfile"]["tmp_name"])) {
			$uuid = gfGetUUID();
			$parts = explode('.', $_FILES["uploadedfile"]["name"]);
			$ext = $parts[count($parts)-1];
			if (in_array($ext, $gConfig["fileextensions"])) {
				$filename = $uuid . '.' . $ext;
				if(move_uploaded_file($_FILES['uploadedfile']['tmp_name'], $gConfig["filepath"].$filename)) {
					$gOut->nl("The file ".  basename( $_FILES['uploadedfile']['name']) . " has been moved to " . $gConfig["filepath"].$filename);
					return $filename;
				} else{
					$gOut->nl("There was an error uploading the file, please try again!");
					return false;
				}

			} else {
				return false;
			}
		} else {
			return false;
		}
		
	}

	//implements PageInterface
	public function getPageName() {
		return "Upload file";
	}

	public function toHtml() {
		global $gUser;
		
		$out = "";
		$out .= self::UploadForm();

		return $out;
	}

	static public function factory() {
		return new UploadFile();
	}
	public static function getPageLink() {
		return '<a href="index.php?title=upload">Upload</a>';
	}
}
?>
