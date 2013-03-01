/******************************************************
 * VIDEO BROWSER SHOWDOWN Server 2011-2013            *
 * Klaus Schoeffmann, Klagenfurt University, Austria  *
 * ks@itec.uni-klu.ac.at                              *
 * 2011-2013                                          *
 *                                                    *
 * Feel free to use or improve this code.             *
 * When using it for purposes outside the VBS, please *
 * mention Klagenfurt University                      *
 * (Creative Commons License, Attribution             *
 *****************************************************/


import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;


public class Server {

	int port;

	public Server(int port) {
		this.port = port;
	}

	public void open() {

		ServerSocket serverSock=null;
		try {
			serverSock = new ServerSocket(port);
			System.out.println("Server running on port " + port + "...");
		} catch (IOException e) {
			System.out.println("Port " + port + " not available!");
			System.exit(-1);
		}

		while (true) {
			Socket clientSock=null;
			try {
				clientSock = serverSock.accept();
				ClientHandler ch = new ClientHandler(clientSock);
				ch.start();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

	}
}
