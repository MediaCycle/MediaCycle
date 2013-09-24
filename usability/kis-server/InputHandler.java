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


import java.util.Scanner;

public class InputHandler extends Thread {
    
    public InputHandler(String _library) {
        setDaemon(true);
        library = _library;
    }
    
    public void run() {
        
    	System.out.println();
    	System.out.println("To load a task please enter 'start <filename without extension>' (e.g.: start 040) or enter 'exit' to quit.");
        System.out.println("To load a library please enter 'lib <path to xml file>' (e.g.: lib /Volumes/data/Datasets/xml/library.xml) or enter 'exit' to quit.");
        
        if(library!=""){
            System.out.println("Loading library " + library);
            VBSServer.loadLibrary(library);
        }
    	
        while (true) {
            try {
	            Scanner sc = new Scanner(System.in);
	            String filepath = sc.nextLine().trim();
	            if (filepath.toLowerCase().equals("exit") || filepath.toLowerCase().equals("quit")) {
	                System.exit(1);
	            }
	            else if (filepath.toLowerCase().equals("stats")) {
	            	VBSServer.stats(true);
	            }
	            else if (filepath.toLowerCase().startsWith("load ")) {
	            	VBSServer.readGroundTruth(filepath.substring(5) + ".txt");
	            }
				else if (filepath.toLowerCase().startsWith("lib ")) {
	            	VBSServer.loadLibrary(filepath.substring(4));
	            }
	            else if (filepath.startsWith("start ") || filepath.startsWith("play ")) {
	            	VBSServer.play(filepath.substring(5).trim(),1000);
	            }
	            else if (filepath.toLowerCase().startsWith("cues ")) {
	            	VBSServer.CUESDIR = filepath.substring(5);
	            	System.out.println("cues directory set to " + VBSServer.CUESDIR);
	            }
	            else if (filepath.toLowerCase().startsWith("restore")) {
	            	VBSServer.restoreScore();
	            }
	            else 
	            {
	            	System.out.println("Invalid command: " + filepath);
	            }
            }
            catch (Exception e) {}
        }
        
    }
    public String library;
}
