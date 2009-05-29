<?php
/**
 * @brief Output.php
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
 * Description of Output
 *
  */
class Output {
	private $out, $content;
	function  __construct() {
		$this->out = "";
		$this->content = array();
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
	static public function loadPage($pagename) {
		global $gPages,$gHomePage,$gOut;
		
		$classname = $gPages[$pagename];
		if (!class_exists($classname)) {
			//not found, try homepage
			$classname = $gPages[$gHomePage];
			if (!class_exists($classname)) {
				$gOut->setContent('pagename',"Page not found");
				$gOut->setContent('pagecontent', "Hello World");
				return false;
			}
		}
		
		$obj = call_user_func(array($classname, 'factory'));

		$gOut->setContent('pagename',$obj->getPageName());
		$gOut->setContent('pagecontent', $obj->toHtml());
		return true;
	}
	public function toHtml($name = "") {
		return $this->out;
	}
}
?>
