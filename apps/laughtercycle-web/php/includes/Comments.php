<?php
/**
 * @brief Comments.php
 * @author Alexis Moinet
 * @date 30/06/2009
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
 * Description of Comments
 *
  */

class Comments {
    static public function exists($fileId,$userId) {
        $fileId = intval($fileId);
        $userId = intval($userId);

        if ($fileId > 0 && $userId > 0) {
            global $gDB;

            $query = "SELECT * FROM comments";
            $query .= sprintf(" WHERE file_id=%d AND user_id=%d",$fileId,$userId);

            $result = $gDB->query($query);

            if ($gDB->next_record()) {
                return true;
            }
        }
        return false;
    }
    static public function getFileComments($fileId) {
        $fileId = intval($fileId);
        if ($fileId > 0) {
            global $gDB;

            $query = "SELECT * FROM comments, users";
            $query .= sprintf(" WHERE file_id=%d AND users.id=comments.user_id",$fileId);
            $query .= " ORDER BY time DESC";

            $out = "";

            $result = $gDB->query($query);
            if ($gDB->nf() > 0) {
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
            }
            return $out;
        }
        return "";
    }

    static public function getUserComments($userId) {
        $userId = intval($userId);
        if ($userId > 0) {
            global $gDB;

            $query = "SELECT * FROM comments, files";
            $query .= sprintf(" WHERE user_id=%d AND files.id=comments.file_id",$userId);
            $query .= " ORDER BY time DESC";

            $result = $gDB->query($query);

            $out = "";
            if ($gDB->nf() > 0) {
                global $gUser;
                if ($gUser->getId() == $userId) {
                    $out .= '<div id="filecomments">My comments :';
                } else {
                    $out .= '<div id="filecomments">His comments :';
                }
                $out .= '<ul>';
                while ($gDB->next_record()) {
                    $out .= '<li class="li-filecomment">';
                    $out .= '<a href="index.php?title=file&id=' . $gDB->f("file_id") . '">' . $gDB->f("title") . '</a>';
                    $out .= '<div class=filenote>'.$gDB->f("note").'</div>';
                    $out .= '<div class=filecomment>'.$gDB->f("comment").'</div>';
                    $out .= '</li>';
                }
                $out .= '</ul></div>';
            }
            return $out;
        }
        return "";
    }

    static public function getNLastComments($n=0) {
        global $gConfig, $gDB;

        if ($n<=0) {
            $n = $gConfig["nlastcomments"];
        }

        $query = "SELECT * FROM comments, files, users";
        $query .= " WHERE files.id=comments.file_id AND users.id=comments.user_id";
        $query .= sprintf(" ORDER BY time DESC LIMIT 0, %d",$n);

        $result = $gDB->query($query);

        $out = "";
        if ($gDB->nf() > 0) {
            global $gUser;

            $out .= '<div id="filecomments">Last comments :';
            $out .= '<ul>';
            while ($gDB->next_record()) {
                $out .= '<li class="li-filecomment">';
                $out .= '<a href="index.php?title=file&id=' . $gDB->f("file_id") . '">' . $gDB->f("title") . '</a>';
                $out .= '<div class=filecomment>'.$gDB->f("comment").'</div>';
                $out .= '</li>';
            }
            $out .= '</ul></div>';
        }
        return $out;
    }

    static public function form($fileId=0) {
        $fileId = intval($fileId);

        if ($fileId > 0) {
            $out = "";

            $out .= "<div id=\"commentform\">\n";
            $out .= "<form name=\"commentform\" action=\"index.php?action=file&do=comment\" method=\"POST\">\n";
            $out .= sprintf("<input type=hidden name=\"fileId\" value=\"%d\">",$fileId);
            $out .= "<label for=\"note\">Your note: </label><br/>";
            $out .= "<input type=hidden id=\"noteId\" name=\"note\" value=\"\">";
            $out .= '<div class="code">
                        <script type="text/javascript" language="javascript" src="ratingscript.js"></script>
                        <div id="rateMe" title="Rate Me..." >
                            <a onclick="rateIt(this,1)" id="_1" title="ehh..." onmouseover="rating(this)"></a>
                            <a onclick="rateIt(this,2)" id="_2" title="Not Bad" onmouseover="rating(this)"></a>
                            <a onclick="rateIt(this,3)" id="_3" title="Pretty Good" onmouseover="rating(this)"></a>
                            <a onclick="rateIt(this,4)" id="_4" title="Out Standing" onmouseover="rating(this)"></a>
                            <a onclick="rateIt(this,5)" id="_5" title="Freaking Awesome!" onmouseover="rating(this)"></a>
                        </div>
                    </div>';

            $out .= "<label for=\"commenttext\">Your comment : </label><br/>";
            $out .= "<textarea name=\"commenttext\" rows=\"8\" cols=\"80\">\n";
            $out .= "</textarea><br/>\n";
            $out .= "<input type=\"submit\" value=\"Send comment\" name=\"ok\" />";
            $out .= "</form>\n</div>\n";
        }
        return $out;
    }

    static public function addComment($fileId, $comment, $note) {
        global $gDB, $gUser;

        $fileId = intval($fileId);
        $comment = $gDB->cleanInputString($comment);
        $note = ($note == "") ? "NULL" : "'" . intval($note) . "'";

        if (LCFile::exists($fileId)) {
            $query = "INSERT INTO";
            $query .= " comments ";
            $query .= "(`file_id`,`user_id`,`comment`,`note`,`time`)";
            $query .= " VALUES ";
            $query .= "(" . $fileId . "," . $gUser->getId() . ",'" . $comment . "'," . $note . "," . gfGetTimeStamp() . ")";

            $gDB->query($query);
        }
    }
}
?>
