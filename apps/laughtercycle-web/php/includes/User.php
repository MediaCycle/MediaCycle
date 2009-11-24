<?php
/**
 * @brief User.php
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
 * This class manage user's login/logout and preferences get/set.
 * It also extends the class Page, which means a user's information
 * can be displayed using : index.php?title=user
 * (each user can only see its own information on that page)
 *
  */

/*
 * Part of this code has been inspired by Mediawiki's User class which is GPL
 * This code follow the same idea/structure but has been over-simplified and adapted
 * e.g. function names are mainly the same, the loadFromSession function follows the line
 * The question is : "are we GPL'd ?" (I guess so)
 *
 * NB: many other user-login classes exists with less rectrictive licences
 * so this class could be replaced by any other similar class.
 * Normally this should mean that we only have to release User.php under GPL,
 * not the whole LaughterCycle code !!!
 *
 * NB2: all of this is to take into account if (and only if)
 * we start redistributing LaughterCycle.
 */
class User extends Page {
    private $id, $name, $password, $salt, $token, $email,
    $avatar, $description, $language, $regdate,
    $lastlogin,$admin;

    private $files, $groups, $friends;

    function __construct() {
        //anon until login
        $this->id = 0;
        //hopefully anon is not an admin
        $this->admin = false;
        $this->loadFromSession();
    }
	/**
	 * makes the "select * from users" SQL query using either user's name or user's ID
	 * if user exists, it fills the "$this->" private variables with the results
	 *
	 * @param string $condtype
	 * @param <type> $cond $cond is either an int (login using userid) or a string (login using username)
	 * @return bool
	 */
    private function fetchUserQuery($condtype, $cond = "") {
        global $gDB;

        $query = "SELECT * FROM users";
        switch($condtype) {
            case 'id':
                $query .= sprintf(" WHERE id=%d",intval($this->id));
                break;
            case 'name':
                if (cond === "") {
                    return false;
                }
                $cond = $gDB->cleanInputString($cond);//no sql injection
				
                $query .= sprintf(" WHERE name LIKE '%s'",$cond);
                break;
            default:
                return false;
        }

        $result = $gDB->query($query);

        if ($gDB->next_record()) {
            $this->id = $gDB->f("id");
            $this->name = $gDB->f("name");
            $this->password = $gDB->f("password");
            $this->salt = $gDB->f("salt");
            $this->token = $gDB->f("token");
            $this->email = $gDB->f("email");
            $this->avatar = $gDB->f("avatar");
            $this->description = $gDB->f("description");
            $this->language = $gDB->f("language");
            $this->regdate = $gDB->f("registration_date");
            $this->lastlogin = $gDB->f("last_login");
            $this->admin = ($gDB->f("is_admin") == 1) ? true : false;
            $gDB->free();

            return true;
        } else {
            return false;
        }
    }

    static public function createNewUser($name, $password, $email) {
        global $gDB;

        //no sql injection
        $name = $gDB->cleanInputString($name);
        $password = $gDB->cleanInputString($password);
        $email = $gDB->cleanInputString($email);

        if (self::userExists("name",$name)) {
            return false;
        }
        if (!self::isPasswordValid($password)) {
            return false;
        }
        if (!self::isEmailValid($email)) {
            return false;
        }

        //generate salt and token
        $salt = self::saltShaker();
        $token = self::saltShaker();
		//mixes password and salt using sha1 --> stores in the DB
		//it makes it virtually impossible to retrieve password (nowadays)
        $password = self::hashShaker($password, $salt);

        //SQL insertion
        $query = "INSERT INTO";
        $query .= " users ";
        $query .= "(`name`,`password`,`salt`,`token`,`email`,`registration_date`)";
        $query .= " VALUES ";
        $query .= "('" . $name . "','" . $password . "','" . $salt . "','" . $token . "','" . $email . "'," . gfGetTimeStamp() . ")";
        //get info (id, name, password, salt, token, ...)

		//create a new instance of the class User
		//(is there a diff in php between "new User" and "new User()" ???)
        $user = new User;

		//inserts into DB
        $gDB->query($query);

		//if it worked, get the id of the newly inserted user
        if ($gDB->affected_rows()) {
            $user->id = mysql_insert_id($gDB->link_id());
        } else {
            return false;
        }

		//user is an empty shell, fill it based on the newly created id
        if (!$user->loadFromId()) {
            return false;
        }

        //automatically logs the user;
        $user->setSessions();
        $user->setCookies();
		//save the user creation timestamp in database
        $user->saveLogin(UserLogin::CREATE);

        return $user;
    }

	/**
	 *
	 * @param <type> $name
	 * @param <type> $password
	 * @return <type> 
	 */
    public function login($name,$password) {
        if (!$this->fetchUserQuery("name",$name)) {
            $this->logout();
        }
        if (!$this->checkPassword($password)) {
            $this->logout();
        }

        $this->setSessions();
        $this->setCookies();
        $this->saveLogin(UserLogin::LOGIN);

        return true;
    }

    public function saveLogin($type = UserLogin::LOGIN) {
        global $gDB;

        if ($type !== UserLogin::LOGOUT) {
            $query = sprintf("UPDATE users SET last_login=%d WHERE id=%d", gfGetTimeStamp(), $this->id);
            $gDB->query($query);
        }

        $query = sprintf("INSERT INTO login (`user_id`,`type`,`time`) VALUES (%d, %d, %d)", $this->id, $type, gfGetTimeStamp());
        $gDB->query($query);
    }
    public function setSessions() {
        $_SESSION["userId"] = $this->id;
        $_SESSION["userName"] = $this->name;
        $_SESSION["userToken"] = $this->token;
    }

    public function setCookies() {
        $this->setCookie("userId",$this->id);
        $this->setCookie("userName",$this->name);
        $this->setCookie("userToken",$this->token);
    }
    public function logout($savelog = false) {
        if ($savelog) {
            $this->saveLogin(UserLogin::LOGOUT);
        }

        $this->admin = false;
        $this->id = 0;
        $this->name = "";
        $this->password = "";
        $this->salt = "";
        $this->token = "";
        $this->email = "";
        $this->avatar = "";
        $this->description = "";
        $this->language = "";
        $this->regdate = 0;
        $this->lastlogin = 0;

        $this->files = array();
        $this->groups = array();
        $this->friends = array();

        $this->setSessions();
        $this->setCookies();

    }

    protected function setCookie( $name, $val ) {
        setcookie( $name, $val, time() + 86400 );
    }
    protected function clearCookie( $name ) {
        setcookie( $name, $val, time() - 86400 );
    }

    public function loadFromSession() {

        if (isset($_SESSION["userId"])){//récupération des variable de session
            if (0 != $_SESSION["userId"]) {
                $id = $_SESSION["userId"];
            } else {
                $this->logout();
                return false;
            }
        } else if (isset($_COOKIE["userId"])){
            if (0 != $_SESSION["userId"]) {
                $id = intval($_COOKIE["userId"]);
                $_SESSION["userId"] = $id;
            } else {
                $this->logout();
                return false;
            }
        } else {
            $this->logout();
            return false;
        }

        if ( isset( $_SESSION['userName'] ) ) {
            $name = $_SESSION['userName'];
        } else if ( isset( $_COOKIE["userName"] ) ) {
            $name = $_COOKIE["userName"];
            $_SESSION['userName'] = $name;
        } else {
            $this->logout();
            return false;
        }

        $this->id = $id;

        if ( !$this->loadFromId() ) {
            //id doesn't exist --> go back to anon
            $this->logout();
            return false;
        }

        if ( isset( $_SESSION['userToken'] ) ) {
            $passwordCorrect = $this->token == $_SESSION['userToken'];
        } else if ( isset( $_COOKIE["userToken"] ) ) {
            $passwordCorrect = $this->token == $_COOKIE["userToken"];
        } else {
            $this->logout();
            return false;
        }

        if ( ( $name == $this->name ) && $passwordCorrect ) {
            $_SESSION['userToken'] = $this->token;
        } else {
            # Invalid credentials
            $this->logout();
            return false;
        }

        $this->loadUserGroups();
        $this->loadUserFriends();
        $this->loadUserFiles();

        return true;
    }
    public function loadFromId() {
        $this->id = intval($this->id);

        if (0 < $this->id) {
            return $this->fetchUserQuery("id");
        } else {
            return false;
        }
    }

    public function loadUserGroups() {
        global $gDB;

        $query = sprintf("SELECT groups.id id,groups.name name FROM usergroups, groups");
        $query .= sprintf(" WHERE usergroups.user_id=%d AND usergroups.group_id=groups.id",$this->id);

        $gDB->query($query);

        while ($gDB->next_record()) {
            //TODO : check if we're still in this group
            $this->groups[] = array("id" => $gDB->f("id"), "name" => $gDB->f("name")); //should be a UserGroup class instance
        }
    }
    public function loadUserFriends() {
        global $gDB;

        $query = sprintf("SELECT users.id id,users.name name FROM friends, users");
        $query .= sprintf(" WHERE friends.user_id=%d AND friends.friend_id=users.id",$this->id);

        $gDB->query($query);

        while ($gDB->next_record()) {
            //TODO: check if he's still our friend
            $this->friends[] = array("id" => $gDB->f("id"), "name" => $gDB->f("name")); //should be a UserFriend class instance
        }

    }
    public function loadUserFiles() {
        global $gDB;
        //TODO : put a limit in the SQL query
        $query = sprintf("SELECT files.id id,files.title title FROM files");
        $query .= sprintf(" WHERE files.uploader=%d",$this->id);

        $gDB->query($query);

        $this->files = array();

        while ($gDB->next_record()) {
            $this->files[] = array("id" => $gDB->f("id"), "title" => $gDB->f("title")); //should be a UserFile class instance
        }
    }

    public function isInGroup($group) {
        $group = intval($group);
        foreach ($this->groups as $key => $val) {
            if ($val["id"] === $group) {
                return true;
            }
        }
        return false;
    }

    public function hasFriend($friend) {
        $friend = intval($friend);
        foreach ($this->friends as $key => $val) {
            if ($val["id"] === $friend) {
                return true;
            }
        }
        return false;
    }

    public function getId() {
        return $this->id;
    }
    public function getName() {
        return $this->name;
    }
    public function getGroups() {
        return $this->groups;
    }
    public function getFriends() {
        return $this->friends;
    }
    public function getFiles() {
        return $this->files;
    }
    public function checkPassword($password) {
        return self::hashShaker($password,$this->salt) === $this->password;
    }

    public function isAdmin() {
        return $this->admin;
    }
    public function isLoggedIn() {
        return 0 != $this->getId();
    }

    public function toHtml() {
        $out = "";
        $out .= "id = " . $this->getId();
        $out .= ", name = " . $this->getName();
        $out .= "<br/>";

        if (count($this->files) > 0) {
            $out .= '<div id="myfiles">My files :';
            $out .= '<ul>';
            foreach ($this->files as $file) {
                $file = new LCFile($file["id"]);
                $out .= '<li class="li-lastlaugh">';
                $out .= '<div class="lc-player">';
                $out .= '<a href="index.php?title=file&id=' . $file->getId() . '">' . $file->getTitle() . '</a>';
                $out .= LCPlayer::miniPlayer($file->getName());
                $out .= '</div></li>';
            }
            $out .= '</ul></div>';
        }
        $out .= Comments::getUserComments($this->getId());
        return $out;
    }
    public function getLinks() {
        //return login/logout + userpage link
        $link = "";
        if ($this->isLoggedIn()) {
            $link .= '<div id="userlinks-logged"><ul>';
            $link .= '<li id="userpage">';
            $link .= '<a href="index.php?title=user">' . $this->name . '</a>';
            $link .= '</li>';
            $link .= '<li id="logout">';
            $link .= UserLogin::logoutLink();
            $link .= '</li>';
            $link .= '</ul></div>';
        /*$link .= '<div id="userlinks-logged">';
        $link .= '<span id="userpage">';
        $link .= '<a href="index.php?title=user">' . $this->name . '</a>';
        $link .= '</span>';
        $link .= '<span id="logout">';
        $link .= UserLogin::logoutLink();
        $link .= '</span>';
        $link .= '</div>';*/
        } else {
            $link .= '<div id="userlinks-notlogged">';
            $link .= '<span id="login">';
            $link .= UserLogin::loginLink();
            $link .= '</span>';
            $link .= '</div>';
        }
        return $link;
    }

    static public function saltShaker() {
        return sha1(uniqid(mt_rand(), true));
    }
    static public function hashShaker($password,$salt) {
        return sha1($salt . sha1($password));
    }
    static public function userExists($condtype, $cond="") {
        global $gDB;

        $query = "SELECT * FROM users";
        switch($condtype) {
            case 'id':
                $query .= sprintf(" WHERE id=%d",intval($this->id));
                break;
            case 'name':
                if (cond === "") {
                    return false;
                }
                $cond = $gDB->cleanInputString($cond);//no sql injection
            /*if(get_magic_quotes_gpc()) {
                $cond = stripslashes($cond);
            }
            $cond = mysql_real_escape_string($cond,$gDB->link_id());//no sql injection*/
                $query .= sprintf(" WHERE name LIKE '%s'",$cond);
                break;
            default:
                return false;
        }

        $result = $gDB->query($query);

        if ($gDB->next_record()) {
            return true;
        } else {
            return false;
        }
    }
    static public function isPasswordValid($password) {
        global $gConfig;
        if (strlen($password) < $gConfig['minpasswordlength']) {
            return false;
        } else {
            return true;
        }
    }
    static public function isEmailValid($password) {
        return true;
    }

    public function getPageName() {
        return $this->name;
    }
    static public function factory() {
        global $gUser;
        return $gUser;
    }
}

class UserLogin extends Page {
    //static public LOGIN = ;
    const CREATE = 0;
    const LOGIN = 1;
    const LOGOUT = 2;

    //put your code here
    public function  __construct() {
        ;
    }

    static public function loginLink() {
        return '<a href="index.php?title=login">Login</a>';
    }
    static public function createUserLink() {
        return '<a href="index.php?title=login&do=createuser">Create new User</a>';
    }
    static public function logoutLink() {
        return '<a href="index.php?action=user&do=logout">log&nbsp;out</a>';
    }
    static public function loginForm() {
        global $gUser;

        $out = "";
        if (!$gUser->isLoggedIn()) {
            $out .= "<div id=\"loginform\">\n";
            $out .= "<form name=\"loginform\" action=\"index.php?action=user&do=login\" method=\"POST\">\n";
            $out .= "<table>\n";
            $out .= "<tr>\n";
            $out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"username\">User name : </label></td>\n";
            $out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"text\" name=\"username\" value=\"Your name\" size=\"20\"  onfocus='javascript:username.value=\"\"' /></td>\n";
            $out .= "<tr/>\n";
            $out .= "<tr>\n";
            $out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"password\">Password : </label></td>\n";
            $out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"password\" name=\"password\" value=\"\" size=\"20\" /></td>\n";
            $out .= "<tr/>\n";
            $out .= "<tr><td></td>\n";
            $out .= "<td class=\"submit\" style=\"text-align: left\"><input type=\"submit\" value=\"Connexion\" name=\"ok\"/></td>\n";
            $out .= "<tr/>\n";
            $out .= "</table>\n";
            $out .= "</form>\n</div>\n";
            $out .= '<div id="createaccount">';
            $out .= UserLogin::createUserLink();
            $out .= '</div>';
        } else {
            $out .= "<div>Already connected</div>";
        }
        return $out;
    }

    public static function createUserForm() {
        global $gUser;

        $out = "";
        if (!$gUser->isLoggedIn()) {
            $out .= "<div id=\"loginform\">\n";
            $out .= "<form name=\"loginform\" action=\"index.php?action=user&do=createuser\" method=\"POST\">\n";
            $out .= "<table>\n";
            $out .= "<tr>\n";
            $out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"username\">User name : </label></td>\n";
            $out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"text\" name=\"username\" value=\"Your name\" size=\"20\"  onfocus='javascript:username.value=\"\"' /></td>\n";
            $out .= "<tr/>\n";
            $out .= "<tr>\n";
            $out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"password\">Password : </label></td>\n";
            $out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"password\" name=\"password\" value=\"\" size=\"20\" /></td>\n";
            $out .= "<tr/>\n";
            $out .= "<tr>\n";
            $out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"passwordconfirm\">Confirm Password : </label></td>\n";
            $out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"password\" name=\"passwordconfirm\" value=\"\" size=\"20\" /></td>\n";
            $out .= "<tr/>\n";
            $out .= "<tr>\n";
            $out .= "<td class=\"label\" style=\"text-align: right\"><label for=\"email\">e-mail : </label></td>\n";
            $out .= "<td class=\"input\" style=\"text-align: left\"><input type=\"text\" name=\"email\" value=\"\" size=\"20\" /></td>\n";
            $out .= "<tr/>\n";
            $out .= "<tr><td></td>\n";
            $out .= "<td class=\"submit\" style=\"text-align: left\"><input type=\"submit\" value=\"Create\" name=\"ok\"/></td>\n";
            $out .= "<tr/>\n";
            $out .= "</table>\n";
            $out .= "</form>\n</div>\n";
        } else {
            $out .= "<div>First you need to log out</div>";
        }
        return $out;
    }


    public static function logout() {
        $out = "";
        $out .= "<div id=\"logout\">\n";
        $out .= self::logoutLink();
        $out .= "</div>\n";

        return $out;
    }

    public function getPageName() {
        return "Login";
    }
    public function toHtml() {
        global $gUser;
        $out = "";

        if ($gUser->isLoggedIn()) {
            $out .= UserLogin::logout();
        } elseif (isset($_GET["do"])) {
            $do = $_GET["do"];
            switch ($do) {
                case 'login':
                    $out.= self::loginForm();
                    break;
                case 'createuser':
                    $out .= self::createUserForm();
                    break;
            }
        } else {
            $out .= self::loginForm();
        }

        return $out;
    }
    static public function factory() {
        return new UserLogin();
    }
}
?>
