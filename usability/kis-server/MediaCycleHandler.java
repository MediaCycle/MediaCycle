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

// import java.util.Scanner;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.IOException;


public class MediaCycleHandler extends Thread {
    
    public MediaCycleHandler(String _application, String _library) {
        //setDaemon(true);
        application = _application;
        library = _library;
    }
    
    public Process p;
    
    public void run() {

        if(application!=""){
            System.out.println("Loading MediaCycle with application " + application);
            try {
                ProcessBuilder pb = new ProcessBuilder(application,library);
                Process process = pb.start();
                
                BufferedInputStream in = new BufferedInputStream(process.getInputStream());

                byte[] bytes = new byte[4096];

                while (in.read(bytes) != -1) {}

                // wait for completion
                process.waitFor();

                System.out.println("\tloading mediacycle with " + application);
            } catch (Exception e) {
                System.out.println("can't load mediacycle with: " + application);
            }
        }
        
    }
    public String application;
        public String library;
}