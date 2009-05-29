<?php
/**
 * @brief LCRecord.php
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
 * Description of LCRecord
 *
  */
class LCRecord {
    //put your code here
	private $id, $width, $height;

	function  __construct() {
		$this->id = sha1(uniqid(mt_rand(), true));
	}

	function getId() {
		return $this->id;
	}

	function getFileName() {
		return $this->id . ".flv";
	}

	function toHtml() {
		$out = "";
		$out .= '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"' . "\n";
		$out .=	'	id="LaughterCycle" width="702" height="569"' . "\n";
		$out .=	'	codebase="http://fpdownload.macromedia.com/get/flashplayer/current/swflash.cab">' . "\n";
		$out .=	'	<param name="movie" value="LaughterCycle.swf?input=' . $this->id . '.flv" >' . "\n";
		$out .=	'	<param name="quality" value="high" />' . "\n";
		$out .=	'	<param name="bgcolor" value="#869ca7" />' . "\n";
		$out .=	'	<param name="allowScriptAccess" value="sameDomain" />' . "\n";
		$out .=	'	<embed src="LaughterCycle.swf?input=' . $this->id . '.flv" quality="high" bgcolor="#869ca7"' . "\n";
		$out .=	'		width="702" height="569" name="LaughterCycle" align="middle"' . "\n";
		$out .=	'		play="true"' . "\n";
		$out .=	'		loop="false"' . "\n";
		$out .=	'		quality="high"' . "\n";
		$out .=	'		allowScriptAccess="sameDomain"' . "\n";
		$out .=	'		type="application/x-shockwave-flash"' . "\n";
		$out .=	'		pluginspage="http://www.adobe.com/go/getflashplayer">' . "\n";
		$out .=	'</embed>' . "\n";
	    $out .=	'</object>' . "\n";

		return $out;
	}
}
?>
