<?php
/**
 * @brief Output.php
 * @author Alexis Moinet
 * @date 24/11/2009
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
		//this class stores to types of contents :
		// 1. a variable ($out)
		// 2. an array of values ($content)
		//I usually use $out to store debug info and $content to store the content of the current page
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
		//== new line + add()
        $this->out .= "<br/>" . $s;
    }
	/*
	 * this method dynamically loads page given a (key => value)
	 * $pagename is the key and the value is in $gPages (see config.php)
	 */
    public function loadPage($pagename) {
        global $gPages,$gHomePage;

        $classname = $gPages[$pagename];
        if (!class_exists($classname)) {
            //not found, try homepage
            //we might want to add a 'page' table in DB and fetch for it from here
            $classname = $gPages[$gHomePage];
            if (!class_exists($classname)) { // page not found --> display lipsum
                $this->setContent('pagename',"$pagename - Page not found");
                $this->setContent('pagecontent', self::lipsum());
                return false;
            }
        }

		//every class that implements a page must have a factory method
		//which can be called from here
        $obj = call_user_func(array($classname, 'factory'));

		//set pagename and pagecontent (what will be actually displayed to the user, see index.php)
        $this->setContent('pagename',$obj->getPageName());
        $this->setContent('pagecontent', $obj->toHtml());
        return true;
    }
    public function setRecorder() {
        $this->setContent('recorder',LCRecorder::Recorder(500,500));
    }
    public function setSideContent() {
        global $gConfig;

        //$this->setContent('sidecontent', self::lipsum());
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
		//returns the out variable (set with add() et nl())
        return $this->out;
    }

    static public function lipsum() {
        return "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin sed magna ac nunc aliquet posuere. Nullam vitae purus tellus. Aenean posuere, justo quis vestibulum porttitor, mi sem facilisis velit, tincidunt eleifend justo mi vitae velit. Curabitur sit amet ullamcorper nunc. Integer ac augue a risus viverra tincidunt. Proin ullamcorper urna nec dui consequat a ultrices mi mattis. Cras lorem est, interdum nec cursus ut, porta eu eros. Etiam cursus, sem eget viverra mollis, urna felis bibendum leo, aliquam aliquam neque turpis vel elit. Ut eu aliquet lacus. Vestibulum laoreet dictum tempus. Vestibulum rutrum urna sit amet urna placerat dictum. Fusce semper elementum massa, eu euismod augue venenatis eu.";
    }
}
?>
