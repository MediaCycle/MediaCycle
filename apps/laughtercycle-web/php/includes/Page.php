<?php
/**
 * @brief Page.php
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
/**
 * The Page class is the parent class of all classes that can be displayed
 * using index.php?title=pagename (see $gPages explanation in config.php)
 *
  */
interface PageInterface {
    public function getPageName();
    public function toHtml();
    static public function factory();
}

/*
 * shouldn't this class have a "implement PageInterface" ?
 * (need to read doc about php's interface - same as java's ?)
 */
class Page {
	/**
	 * returns the name of the page
	 * @return string
	 */
    public function getPageName() {
        return "";
    }
	/**
	 * Function used to generate content when calling index.php?title=pagekey
	 * (see config.php about pagekey and Output.php for usage)
	 * @return string
	 */
    public function toHtml() {
        return "";
    }
	/**
	 * factory of the class.
	 * this is called in Output.php
	 *
	 * @return Home
	 */
    static public function factory() {
        return new Page();
    }
}
?>
