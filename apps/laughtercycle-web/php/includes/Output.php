<?php
/**
 * @brief Output.php
 * @author Alexis Moinet
 * @date 18/06/2009
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
 * Description of Output
 *
  */
class Output {
	private $out, $content;
	function  __construct() {
		$this->out = "";
		$this->content = array();
		
		$this->setHeader();
		$this->setMenu();
		$this->setRecorder();
		$this->setSideContent();
	}

	public function setContent($name,$content) {
		$this->content[$name] = $content;
	}
	public function getContent($name) {
		return $this->content[$name];
	}
	public function printContent($name) {
		echo $this->content[$name];
	}
	public function add($s = "") {
		$this->out .= $s;
	}
	public function nl($s = "") {
		$this->out .= "<br/>" . $s;
	}
	public function loadPage($pagename) {
		global $gPages,$gHomePage;
		
		$classname = $gPages[$pagename];
		if (!class_exists($classname)) {
			//not found, try homepage
			//we might want to add a 'page' table in DB and fetch for it from here
			$classname = $gPages[$gHomePage];
			if (!class_exists($classname)) {
				$this->setContent('pagename',"$pagename - Page not found");
				$this->setContent('pagecontent', self::lipsum());
				return false;
			}
		}
		
		$obj = call_user_func(array($classname, 'factory'));

		$this->setContent('pagename',$obj->getPageName());
		$this->setContent('pagecontent', $obj->toHtml());
		return true;
	}
	public function setRecorder() {
		$this->setContent('recorder',LCRecorder::Recorder(500,500));
	}
	public function setSideContent() {
		global $gConfig;
		
		$this->setContent('sidecontent', self::lipsum());
		$this->setContent('last-laugh',LCFile::displayLastNFilesPlayed($gConfig["nlastfilesplayed"]));
		$this->setContent('last-comments', Comments::getNLastComments());
	}
	public function setMenu() {
		$this->setContent('menu', Home::getPageLink() . " " . UploadFile::getPageLink() . " " .  "Forum");
	}
	public function setHeader() {
		$this->setContent('header', "Header");
	}

	public function toHtml($name = "") {
		return $this->out;
	}

	static public function lipsum() {
		return "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin sed magna ac nunc aliquet posuere. Nullam vitae purus tellus. Aenean posuere, justo quis vestibulum porttitor, mi sem facilisis velit, tincidunt eleifend justo mi vitae velit. Curabitur sit amet ullamcorper nunc. Integer ac augue a risus viverra tincidunt. Proin ullamcorper urna nec dui consequat a ultrices mi mattis. Cras lorem est, interdum nec cursus ut, porta eu eros. Etiam cursus, sem eget viverra mollis, urna felis bibendum leo, aliquam aliquam neque turpis vel elit. Ut eu aliquet lacus. Vestibulum laoreet dictum tempus. Vestibulum rutrum urna sit amet urna placerat dictum. Fusce semper elementum massa, eu euismod augue venenatis eu.";
	}
}
?>
