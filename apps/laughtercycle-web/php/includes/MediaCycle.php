<?php
/**
 * @brief MediaCycle.php
 * @author Alexis Moinet
 * @date 23/07/2009
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
 * This class connects to MediaCycle, send request and receive results
 * requests can be : adding a file or asking for kNN
 *
  */
class MediaCycle {
    /**
     * Construct a message following the format defined for communications with MediaCycle
     * @param string $command
     * @param string $params
     * @return string
     */
    static public function buildMessage($command,$params) {
        $message = "";
        $message .= $command;
        foreach ($params as $param) {
            $message .= " " . $param;
        }
        return $message;
    }
    /**
     *
     * @param string $command
     * @param string $params
     * @return string or bool
     */
    static public function askMediaCycle($command,$params) {
        global $gConfig;
        //is MediaCycle enabled in config.php ?
        if ($gConfig["mediacycle"]["enable"]) {
            $data = self::buildMessage($command, $params);
            $sc = new SocketClient($gConfig["mediacycle"]["ip"],$gConfig["mediacycle"]["port"]);
            //$sc = new SocketClient("192.168.1.167","12345");
            $sc->send($data);
            $sc->receive($answer);
            $sc->close();

            return $answer;
        } else {
            return false;
        }
    }
    /**
     * Sends a wave file to MediaCycle to be added to its library
     * @param LCFile $file an instance of a file created using the class LCfile (see File.php)
     * @return bool
     */
    static public function addFile(LCFile $file) {
        global $gConfig;
       
        //in this particular case (loading a whole *.wav file),
        //I think it's worth it to first check whether it's needed
        $raw = "";
        if ($gConfig["mediacycle"]["enable"]) {
            $filename = $gConfig["filepath"] . $file->getName() . ".wav";
            $raw = file_get_contents($filename, FILE_BINARY);
        }

        $command = "addfile";
        $params = array($file->getId(),$file->getName().".wav",$raw);

        $answer = self::askMediaCycle($command, $params);

        if ($answer) {
            $ans = explode(" ", $answer,3);
            if ($ans[0] == $command) {
                if (intval($ans[1]) == 1) {
                    return true;
                } else {
                    return false;
                }

            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    /**
     * Asks MediaCycle to save its current state in a file (so that it can be reloaded later)
     * @return bool
     */
    static public function saveLibrary(){
        global $gConfig;

        $command = "savelibrary";
        $params = array($gConfig["mediacycle"]["libraryname"]);

        $answer = self::askMediaCycle($command, $params);

        //$answer = "savelibrary 0/1"
        if ($answer) {
            $ans = explode(" ", $answer);
            if ($ans[0] == $command) {
                if (intval($ans[1]) == 1) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    static public function loadLibrary(){
        global $gConfig;

        $command = "loadlibrary";
        $params = array($gConfig["mediacycle"]["libraryname"]);

        $answer = self::askMediaCycle($command, $params);

        //$answer = "savelibrary 0/1"
        if ($answer) {
            $ans = explode(" ", $answer);
            if ($ans[0] == $command) {
                if (intval($ans[1]) == 1) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    static public function getkNN(LCFile $file, $k) {
        global $gConfig;

        $command = "getknn";
        $params = array($file->getId(),$k);

        $answer = self::askMediaCycle($command, $params);

        //$answer = getknn n id1 id2 .... (n = # of file found, <=$k)
        if ($answer) {
            $ans = explode(" ", $answer);
            if ($ans[0] == $command) {
                $n = intval($ans[1]);
                $result = array_slice($ans, 2);
                return $result;

            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    static public function getThumbnailXml(LCFile $file) {
        global $gConfig;

        $command = "getthumbnail";
        $params = array($file->getId());

        $answer = self::askMediaCycle($command, $params);

        //$answer = getthumbnail xml (xml = xml representation of waveform)
        if ($answer) {
            $ans = explode(" ", $answer,2);

            if ($ans[0] == $command) {
                if (file_put_contents($gConfig["filepath"] . $file->getName() . ".thml",$ans[1]))
                return true;
                else
                return false;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}

class SocketClient {
    public $socket = NULL, $address = NULL, $port = NULL;

    public function __construct($address, $port) {
        global $gOut;

        $this->address = $address;
        $this->port = $port;
        $this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

        if (!$this->socket) {
            $gOut->nl("socket could not be created");
            $this->address = NULL;
            $this->port = NULL;
            return;
        }

        if(!socket_connect($this->socket, $this->address, $this->port)) {
            $gOut->nl("socket could not connect" . socket_strerror(socket_last_error($this->socket)));
            $this->address = NULL;
            $this->port = NULL;
            return;
        }
    }

    public function  __destruct() {
        //socket_close($this->socket);
    }

    public function send($data) {
        global $gOut;
        $result = socket_write($this->socket, $data, strlen($data));
        if (!$result || $result != strlen($data)) {
            $gOut->nl("Socket : send error : $result/" . strlen($data));
            return false;
        }

        return true;
    }

    public function receive(&$answer) {
        $buflen = 2048;
        $answer = "";

        while ($out = socket_read($this->socket, $buflen)) {
            $answer .= $out;
            //echo "<br/> out : " . strlen($out);
            /*if (strlen($out) < $buflen) {
                break;
            }*/
            if ($out == "") {
                break;
            }
        }
    }

    public function close() {
        socket_close($this->socket);
    }
}
?>
