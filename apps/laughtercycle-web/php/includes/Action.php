<?php
/**
 * @brief Action.php
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
 * Description of Action
 *
  */
class Action {
	function  __construct() {
	}

	static public function parse() {
		global $gOut;
		if (isset($_GET["action"])) {
			$gOut->nl("action");
			$action = $_GET["action"];
			switch ($action) {
				case 'file':
					FileAction::parse();
					break;
				case 'page':
					PageAction::parse();
					break;
				case 'user':
					UserAction::parse();
					break;
				default:
			}
		} elseif (isset($_GET["title"])) {
			PageAction::parse();
		} else {
			PageAction::parse();
		}
	}
}

class UserAction extends Action {
	public function  __construct() {
		;
	}

	static public function parse() {
		global $gOut, $gUser;
		
		if (isset($_GET["do"])) {
			$gOut->nl("do");
			$do = $_GET["do"];
			switch ($do) {
				case 'login':
					$name = $_POST["username"];
					$password = $_POST["password"];
					$gUser->login($name, $password);
					break;
				case 'createuser':
					$name = $_POST["username"];
					$password = $_POST["password"];
					$passwordconfirm = $_POST["passwordconfirm"];
					$email = $_POST["email"];
					if ($password === $passwordconfirm) {
						$gUser = User::createNewUser($name, $password, $email);
						if(!$gUser) {
							$gOut->nl("problem");
						} else {
							$gOut->nl($gUser->toHtml());
						}
					} else {
						$gOut->nl("password error");
					}
					break;
				case 'logout':
					$gUser->logout(true);
				case 'save':
					break;
				default:
			}
		}
		header("Status: 200");
		header("Location: ./index.php");
		exit();
	}
}

class FileAction extends Action {
	public function  __construct() {
		;
	}

	static public function parse() {
		global $gOut, $gUser;

		if (isset($_GET["do"])) {
			$gOut->nl("do");
			$do = $_GET["do"];
			switch ($do) {
				default:
			}
		}
	}
}

class PageAction extends Action {
	public function  __construct() {
		;
	}

	static public function parse() {
		global $gOut, $gUser;

		if (isset($_GET["title"])) {
			$gOut->nl("title");
			$title = $_GET["title"];
			Output::loadPage($title);
		} else {
			Output::loadPage('home');
		}
	}
}
?>
