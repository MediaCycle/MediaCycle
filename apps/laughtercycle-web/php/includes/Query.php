<?php
/**
 * @brief Query.php
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
 * LCQuery class generates the HTML to embed a Flash application that contains 2 sliders
 * these 2 sliders can be used to modify the importance given to respectivelly the pitch and the timbre
 * during a kNN request (k-nearest-neighbours, see MediaCycle.php)
 *
  */
class LCQuery implements PageInterface {
    //put your code here
    const WIDTH = 400;
    const HEIGHT = 90;
    const PITCH = 0.5;
    const TIMBRE = 0.5;

    private $fileId, $pitch, $timbre;

    public function  __construct() {
        global $gOut;

        $this->pitch = $_POST["pitchWeight"];
        $this->timbre = $_POST["timbreWeight"];
        $this->fileId = intval($_POST["file"]);
    }
    public static function factory() {
        return new LCQuery();
    }
    public function getPageName() {
        return "Query by example";
    }
    public function toHtml() {
        $out = "";
        $out .= LCFile::miniPlayer($this->fileId);
        $out .= self::sliders($this->fileId);

        return $out;
    }

    static public function sliders($fileId,$width=0, $height=0) {
        $out = "";

        if ($fileId > 0) {
            $width = ($width) ? intval($width) : self::WIDTH;
            $height = ($height) ? intval($height) : self::HEIGHT;
            $pitch = self::PITCH;
            $timbre = self::TIMBRE;

            if (isset ($_POST["pitchWeight"])) {
                $pitch = $_POST["pitchWeight"];
            }
            if (isset ($_POST["timbreWeight"])) {
                $timbre = $_POST["timbreWeight"];
            }

            $out .= '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553541234"' . "\n";
            $out .=	'   id="LaughterCycleMultiSlider" width="' . $width . '" height="' . $height . '"' . "\n";
            $out .=	'   codebase="http://fpdownload.macromedia.com/get/flashplayer/current/swflash.cab">' . "\n";
            $out .=	'   <param name="movie" value="LaughterCycleMultiSlider.swf?file=' . $fileId . '&pitch=' . $pitch . '&timbre=' . $timbre . '" >' . "\n";
            $out .=	'   <param name="quality" value="high" />' . "\n";
            $out .=	'   <param name="bgcolor" value="#869ca7" />' . "\n";
            $out .=	'   <param name="allowScriptAccess" value="sameDomain" />' . "\n";
            $out .=	'   <embed src="LaughterCycleMultiSlider.swf?file=' . $fileId . '&pitch=' . $pitch . '&timbre=' . $timbre . '" quality="high" bgcolor="#869ca7"' . "\n";
            $out .=	'       width="' . $width . '" height="' . $height . '" name="LaughterCycle" align="middle"' . "\n";
            $out .=	'       play="true"' . "\n";
            $out .=	'       loop="false"' . "\n";
            $out .=	'       quality="high"' . "\n";
            $out .=	'       allowScriptAccess="sameDomain"' . "\n";
            $out .=	'       type="application/x-shockwave-flash"' . "\n";
            $out .=	'       pluginspage="http://www.adobe.com/go/getflashplayer">' . "\n";
            $out .=	'</embed>' . "\n";
            $out .=	'</object>' . "\n";
        }

        return $out;
    }
}
?>
