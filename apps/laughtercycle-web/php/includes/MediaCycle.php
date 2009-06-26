<?php
/**
 * @brief MediaCycle.php
 * @author Alexis Moinet
 * @date 26/06/2009
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
	static public function addFile(LCFile $file) {
		global $gConfig;

		//$sc = new SocketClient($gConfig["mediacycle"]["ip"],$gConfig["mediacycle"]["port"]);
		$sc = new SocketClient("192.168.1.167","12345");
		//TODO : build $data (file infos)
		$sc->send($data);
		$sc->receive($answer);
		//TODO : check $answer
	}

	static public function getkNN(LCFile $file, $k) {
		global $gConfig;

		$result = array();
		//$sc = new SocketClient($gConfig["mediacycle"]["ip"],$gConfig["mediacycle"]["port"]);
		$sc = new SocketClient("192.168.1.167","12345");
		//TODO : build $data (file id + k)
		$sc->send($data);
		$sc->receive($answer);
		//TODO : check $answer + build $result

		return $result;
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

		if(!socket_connect($this->sock, $this->address, $this->port)) {
			$gOut->nl("socket could not connect" . socket_strerror(socket_last_error($this->sock)));
			$this->address = NULL;
			$this->port = NULL;
			return;
		}
	}

	public function  __destruct() {
		socket_close($this->socket);
	}

	public function send($data) {
		global $gOut;
		$result = socket_write($this->socket, $data, strlen($data));
		if (!$result || $result != strlen($data)) {
			$gOut->nl("Socket : send error");
			return false;
		}
		
		return true;
	}

	public function receive(&$answer) {
		$buflen = 2048;
		$answer = "";

		while ($out = socket_read($this->sock, $buflen)) {
			$answer .= $out;

			if (strlen($out) < $buflen) {
				break;
			}
		}
	}
}
?>
