<?php
/**
 * @brief SocketClient.php
 * @author Alexis Moinet
 * @date 18/06/2009
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

$sc = new SocketClient();

/**
 * Description of SocketClient
 *
  */
class SocketClient {
	public function __construct() {

		ob_implicit_flush(true);

		$maxbuflength = 2048;
		$connexiontype = 'udp';//'tcp'
		
		$address = gethostbyname('localhost');
		$port = 2345;

		//$address = "192.168.1.252";
		//$port = 12345;

		if ($connexiontype == 'tcp') {
			$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		} elseif ($connexiontype == 'udp') {
			$socket = socket_create(AF_INET, SOCK_DGRAM, 0);
		}
		
		if ($socket === false) {
			echo "socket_create() a échoué : raison :  " . socket_strerror(socket_last_error()) . "\n";
		} else {
			echo "socket_create() OK.\n";
		}

		if ($connexiontype == 'tcp') { echo "Essai de connexion à '$address' sur le port '$port'...";
			$result = socket_connect($socket, $address, $port);
			if ($socket === false) {
				echo "socket_connect() a échoué : raison : ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
			} else {
				echo "OK.\n";
			}
		}

		$in = "Hello World (the classic way) " . rand(1,1000) . " ";

		echo "<br/>Envoi de la requête ...";
		if ($connexiontype == 'tcp') {
			socket_write($socket, $in, strlen($in));
		} elseif ($connexiontype == 'udp') {
			$saddress = $address;
			$sport = $port;
			if(($ret = socket_sendto($socket, $in, strlen($in), 0, $saddress, $sport)) < 0) {
				echo "Transmission failure.";
				exit();
			}
			echo "<br/>socket_sendto() OK.\n";
		}

		echo "<br/>Lire la réponse : \n\n<br/>";
		//socket_set_option($socket,SOL_SOCKET, SO_RCVTIMEO, array("sec"=>1, "usec"=>100));
		//socket_getsockname($socket, &$myaddr, &$myport);
		//echo $myaddr .":". $myport . "<br/>";
		
		while ($out = socket_read($socket, $maxbuflength)) {
		//do {
			//$out = socket_read($socket, $maxbuflength);
			echo "out : " . $out . "<br/>\n";
			echo "length : " . strlen($out) . "<br/>\n";

			printf("last : %x<br/>\n", ord($out[strlen($out)-2]));

			if (strlen($out) < $maxbuflength) {
				break;
			}
			//if ($out == false) {
			//	echo "false <br/>\n";
			//}
			//$out = socket_read($socket, $maxbuflength);
			//echo $out . "\n";
		//} while (strlen($out)==$maxbuflength);
		}

		echo "Fermeture du socket...";
		socket_close($socket);
		echo "OK.\n\n";
	}
}
?>
