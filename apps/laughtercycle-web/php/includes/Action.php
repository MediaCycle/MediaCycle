<?php
/**
 * @brief Action.php
 * @author Alexis Moinet
 * @date 25/11/2009
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
 * Description of Action
 *
  */
class Action {
	/*
	 * This class parses url to see whether the user launched an action or a page
	 */
    function  __construct() {
    }

    static public function parse() {
        global $gOut;
        if (isset($_GET["action"])) {
            $gOut->nl("action");
            $action = $_GET["action"];
            switch ($action) {
                case 'file':
                    FileAction::parse();//user asked to do something with a file
                    break;
                case 'page':
                    PageAction::parse();//user asked to view a page (default)
                    break;
                case 'user':
                    UserAction::parse();//user asked to do something with is profile
                    break;
                default:
            }
        } elseif (isset($_GET["title"])) {
            PageAction::parse();//user asked to view a page (default)
        } else {
            PageAction::parse();//user asked to view a page (default)
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
                case 'login': //user asked to login by giving username/password
                    $name = $_POST["username"];
                    $password = $_POST["password"];
                    $gUser->login($name, $password);
                    break;
                case 'createuser': //anonymous user asked to create an account
                    $name = $_POST["username"];
                    $password = $_POST["password"];
                    $passwordconfirm = $_POST["passwordconfirm"];
                    $email = $_POST["email"];
                    if ($password === $passwordconfirm) {
                        $gUser = User::createNewUser($name, $password, $email);
                        if(!$gUser) {
                            $gOut->nl("problem");//KO : set debug info
                        } else {
                            $gOut->nl($gUser->toHtml());//OK : set debug info ($gUser->toHTML = present new User in an HTML <div>)
                        }
                    } else {
                        $gOut->nl("password error");//not the same password entered twice
                    }
                    break;
                case 'logout': //user asked to logout
                    $gUser->logout(true);
                    case 'save':
                        break;
                    default:
            }
        }
		//after action, redirect to index.php (homepage) and exit
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
                case 'new': //user recorded a file with the flash recorder
                    $recording = "live";//live recording vs. file upload
                    $type = "audio"; // $_POST["type"];//audio/video (audio only for now)
                    $title = $_POST["title"];//user can give a personnalized title to the file he recorded
                    $path = $_POST["path"];//UUID of the file wo/ extension
                    $file = LCFile::createNewFile($title,$path,$type,$recording);//"create" the file in the MySQL database
                    LCFile::convertFlvToWav($path);//flash records in flv, we want .wav to transmit to mediacycle

					//sends the file to mediacycle and get a thumbnail of the waveform in return (if Mediacycle is enabled in config.php)
                    MediaCycle::addFile($file);//send file to mediacycle
                    MediaCycle::getThumbnailXml($file);//get thumbnail from mediacycle
                    MediaCycle::saveLibrary();//ask mediacycle to save its current state
                    break;
                case 'upload': //users can upload files instead of recording them
                    //TODO non-logged in user can upload but in a tmp folder
                    if ($gUser->isLoggedIn()) {
                        $recording = "upload";//live recording vs. file upload
                        $type = "audio"; // $_POST["type"];//audio/video (audio only for now)
                        $title = $_POST["title"];
                        $path = UploadFile::getUploadedFile();//get the file (tmp->/var/www/LCimages), check extensions (mp3, flv)
                        if (!$path) {
                            $gOut->setContent('error',true);
                            $gOut->setContent('errormsg',"error moving file");
                            //header("Status: 200");
                            //header("Location: ./index.php?title=upload");
                        } else {
                            $file = LCFile::createNewFile($title,$path,$type,$recording);//add to MySQL
                            if ($file) {
                                LCFile::redirect($file->getId());//redirect url to new file ( http://myserver/title=file&id=123456789 )
                            }
                        }
                    } else {
                        $gOut->setContent('error',true);
                        $gOut->setContent('errormsg',"you must be logged in to upload");
                    }
                    break;
                case 'comment': //user submitted comment to file
                    $fileId = $_POST["fileId"];
                    $comment = $_POST["commenttext"];
                    $note = $_POST["note"];
                    Comments::addComment($fileId, $comment, $note);//add comment to mysql
                    LCFile::redirect($fileId); // redirect to file (in this case "refresh page")
                    break;
                case 'play': //user pressed a play button, the flash applet makes a php request to index.php?action=file&do=play
                    $path = $_POST["path"];
                    LCFile::logFilePlayed($path);//log in the mysql DB
                    break;
                case 'delete':
                    break;
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
            $gOut->loadPage($title);//this tries to load a page with $title as a key (see config.php)
        } else {
            $gOut->loadPage($gHomePage);// if no title is set (http://myserver/index.php, http://myserver/index.php?title=, http://myserver/index.php?truc)
        }
    }
}
?>
