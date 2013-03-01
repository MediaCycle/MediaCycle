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


import java.util.*;

public class MyTimer {
  Timer timer;

  public MyTimer() {
    timer = new Timer();
    timer.scheduleAtFixedRate(new MyTimerTask(), 0, 1000);
  }

  class MyTimerTask extends TimerTask {
    public void run() {
      
        VBSServer.frame.invalidate();
        VBSServer.chart.invalidate();
        VBSServer.frame.validate();
        VBSServer.frame.repaint();
        
      //System.exit(0); //Stops the AWT thread (and everything else)
    }
  }
}