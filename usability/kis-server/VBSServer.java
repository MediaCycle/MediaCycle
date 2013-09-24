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


import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.*;
import java.util.ArrayList;
import java.util.Vector;

import javax.swing.*;

import net.n3.nanoxml.IXMLParser;
import net.n3.nanoxml.IXMLReader;
import net.n3.nanoxml.StdXMLReader;
import net.n3.nanoxml.XMLElement;
import net.n3.nanoxml.XMLException;
import net.n3.nanoxml.XMLParserFactory;

import java.math.*;

public class VBSServer {

	static int PORT = 8080;
	public static int TEAMS = 1;
	//public static String[] TEAM_NAMES = {"Christian", "Stephane", "Thierry"};
    public static String[] TEAM_NAMES = {"You"};
	public static String LOGFILE = "VBSlogs.txt"; //verbose log file
	public static String LOGSUCCESSFILE = "log.csv"; //default log file in CSV format
	public static String LOGSTATS = "VBSlogs.txt"; //"stats.txt";
	public static String SCOREFILE = "scores.txt"; //used to store intermediate results for recovery in case of crash
	public static long TASK_START_TIME = -1;
	public static long TASK_END_TIME = -1;
	public static int TASK_NUM = 0;
	public static int OVERLAPPING = 125; //number of overlapping frames to accept
	
	public static int GTSTART; //ground truth segment start frame
	public static int GTSTOP; //ground truth segment stop frame
	public static String GTFILE; //target filename
	public static long MAX_TASK_SOLVE_TIME = 60000;//180000; //maximum task solve time
	
	public static int[] SUBMISSON_COUNT = new int[TEAMS]; //number of submission for this task
	public static boolean[] TEAM_FINISHED = new boolean[TEAMS]; //is team already finished (for this task)?
	public static long[] TASK_TIME = new long[TEAMS]; //task time
	public static double[] SCORE = new double[TEAMS]; //overall score
	public static Vector<Submission>[] SUBMISSION = new Vector[TEAMS];
	public static Vector<String> TASKS = new Vector<String>();
		
	public static ChartPanel chart;
	public static JFrame frame;
	
	//WINDOWS
	//public static String VLC = "C:\\Program Files (x86)\\VideoLAN\\VLC";
	//public static String VLCEXE = "\\vlc.exe "; 
	//public static String DELIM = "\\";
	
	//MAC OS
	public static String VLC = "/Applications/VLC.app/Contents/MacOS";
	public static String VLCEXE = "/VLC --play-and-exit ";
	public static String DELIM = "/";

	public static String CUESDIR = "cuesExpertRun";
	
	public static void main(String[] args) {
		System.out.println("VBS Server v2.1, by Klaus Schoeffmann");
		System.out.println("forked by Christian Frisson (UMONS)");
		System.out.println("Please make sure that the VLC player is installed in this directory: " + VLC);
		System.out.println("(this is only required for playback of the cue files; basically the server will also work without the VLC player)");
		System.out.println("**************************************");
		//System.out.println("cues directory set to " + CUESDIR + " (change with cues <dir> command)");
		
		for (int i=0; i < TEAMS; i++) {
			SUBMISSION[i] = new Vector<Submission>();
		}
		
        String library = "";
        
		if (args.length >= 1) {
			try {
				PORT = Integer.parseInt(args[0]);
			}
			catch (NumberFormatException nfe) {
				System.out.println("Please specify a valid port number!");
				System.exit(-1);
			}
			
			if (args.length == 2) {
			    //readGroundTruth(args[1]);
				VLC = args[1];
			}
            if (args.length >= 2) {
                library = args[1];
            }
            if (args.length == 3) {
				TEAM_NAMES[0] = args[2];
                log("\tTeam 1 is " + args[2]);
                SCOREFILE = "scores-" + args[2] + ".txt";
            }
			else 
				System.out.println("Warning: VLC path not specified!");
		} else 
			System.out.println("Warning: VLC path not specified!");
		initTask();

        
		
	    InputHandler input = new InputHandler(library);
	    input.start();
	    
	    frame = new JFrame("MediaCycle Known Item Search tasks");
	    frame.setPreferredSize(new Dimension(1000,550));
	    //frame.setSize(800,600);
	    //frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
	    chart = new ChartPanel(SCORE, TEAM_NAMES, "MediaCycle Known Item Search tasks");
	    frame.getContentPane().add(chart, BorderLayout.CENTER);
	    frame.pack();
	    
	    WindowListener wndCloser = new WindowAdapter() {
	      public void windowClosing(WindowEvent e) {
	        //System.exit(0);
	      }
	    };
	    frame.addWindowListener(wndCloser);
	    
	    frame.setVisible(true);
	    
	    MyTimer timer = new MyTimer();
	    
		Server server = new Server(PORT);
		server.open();
	}
	
	public static void initTask() {
	    for (int i=0; i < TEAMS; i++) {
	        VBSServer.SUBMISSON_COUNT[i] = 0;
	        VBSServer.TEAM_FINISHED[i] = false;
	        VBSServer.TASK_TIME[i] = 0;
	        VBSServer.SUBMISSION[i].clear();
	    }
	    TASK_START_TIME = System.currentTimeMillis();
	    TASK_END_TIME = System.currentTimeMillis() + MAX_TASK_SOLVE_TIME;
	}
	
	public static void readGroundTruth(String filepath) {
	    BufferedReader reader = null;
	    
        try {
        	System.out.println("\tloading task from: " + filepath);
            //FileReader file = new FileReader(VBSServer.CUESDIR + "/" +filepath);
			/*FileReader file = new FileReader(filepath);
            reader = new BufferedReader(file);
            GTSTART = Integer.parseInt(reader.readLine());
            GTSTOP = Integer.parseInt(reader.readLine());
            MAX_TASK_SOLVE_TIME = Long.parseLong(reader.readLine());*/
			//MAX_TASK_SOLVE_TIME = 180000;
			GTFILE = filepath; //CF
            TASK_NUM++;
            //reader.close();
            //file.close();
            TASKS.add(filepath.substring(0, filepath.indexOf('.')));
            System.out.println("\tTask successfully loaded from " + filepath);
            log("\tTask successfully loaded from " + filepath);
            stats(false);
            initTask();
        } catch (Exception e) {
            System.out.println("Task could NOT be loaded from " + filepath + "!");
            e.printStackTrace();
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e1) { }
            }
        }
	}
	
	public static void loadLibrary(String filepath) {
		//filepath = "/Volumes/data/Datasets/xml/OLPC.xml";
        
        System.out.println("\trestoring scores");
        
        restoreScore();
        
		System.out.println("\tloading library: " + filepath);
		       
		XMLElement xml = null;
		if (filepath.indexOf(".xml") != -1) {
			try {
				IXMLParser parser = XMLParserFactory.createDefaultXMLParser();
				IXMLReader reader = StdXMLReader.fileReader(filepath);
		        parser.setReader(reader);
		        xml = (XMLElement) parser.parse();
			} catch (FileNotFoundException e) {
				System.out.println("Error: file not found.");
			} catch (IOException e) {
				System.out.println("Error: impossible to read the file.");
			} catch (ClassNotFoundException e) {
				System.out.println("Error: impossible to read the file\nClassNotFoundException: "+e.toString()+"");
			} catch (InstantiationException e) {
				System.out.println("Error: impossible to read the file\nInstantiationException: "+e.toString()+"");
			} catch (IllegalAccessException e) {
				System.out.println("Error: impossible to read the file\nIllegalAccessException: "+e.toString()+"");
			} catch (XMLException e) {
				System.out.println("Error: XML structure is not valid\nXMLException: "+e.toString()+"");
			}
		} else {
			System.out.println("unable to open '"+filepath+"'");
			return;
		}
		if (xml == null) {
			System.out.println("unable to open '"+filepath+"'");
			return;
		}
		System.out.println("parsing '"+filepath+"'");
			
		Vector<XMLElement> mc = xml.getChildrenNamed("Medias");
		if (mc.size()!=1) {
			System.out.println("xml file '"+filepath+"' is not a MediaCycle library, it doesn't contain a <Medias> node");
			return;
		}
					
		Vector<XMLElement> medias = mc.get(0).getChildrenNamed("Media");
		int number_of_medias = medias.size();
		
		if (number_of_medias<1) {
			System.out.println("xml file '"+filepath+"' is doesn't contain any <Media> node");
			return;
		}
		
		System.out.println("xml file '"+filepath+"' contains " + number_of_medias + " medias");
				
		
		while(true){
			double a = Math.random();
			System.out.println("random '"+a+"'");
			int b = (int)(a*(number_of_medias-1));
			System.out.println("loading media '"+b+"'");
				
			if (medias.get(b).getAttribute("FileName", null) != null)
			{ 
			
				String filename = medias.get(b).getAttribute("FileName", null);
				System.out.println("Filename:"+filename);
			
				int samplerate = 1;
				int samples = 1;
				if (medias.get(b).getAttribute("SamplingRate", null) != null)
					samplerate = Integer.parseInt(medias.get(b).getAttribute("SamplingRate", null));
				if (medias.get(b).getAttribute("SampleEnd", null) != null)
					samples = Integer.parseInt(medias.get(b).getAttribute("SampleEnd", null));
				int wait = (int)((float)samples/(float)samplerate*1000);
				System.out.println("SamplingRate:"+samplerate);
				System.out.println("SampleEnd:"+samples);
				System.out.println("wait:"+wait);
		
				play(filename,wait);
			}
		}
		
	}
	
	public static void restoreScore()
	{
		BufferedReader reader = null;
		try {
			String line = null;
			FileReader file = new FileReader(VBSServer.SCOREFILE);
			reader = new BufferedReader(file);
			while ((line = reader.readLine()) != null)
			{
				for (int i=0; i < VBSServer.TEAMS; i++)
				{
					VBSServer.SCORE[i] = Double.parseDouble(reader.readLine());
				}
			}
			System.out.println("Restored scores:");
			for (int i=0; i < VBSServer.TEAMS; i++)
				System.out.println("Team " + (i+1) + ": " + VBSServer.SCORE[i]);
			reader.close();
			file.close();
			
			VBSServer.frame.invalidate();
	        VBSServer.chart.invalidate();
	        VBSServer.frame.validate();
	        VBSServer.frame.repaint();
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void saveScore()
	{
		FileWriter file = null;
		try {
			file = new FileWriter(VBSServer.SCOREFILE,true);
			PrintWriter out = new PrintWriter(file);
			out.println(System.currentTimeMillis());
			for (int i=0; i < VBSServer.TEAMS; i++)
			{
				out.println(VBSServer.SCORE[i]);
			}
			out.close();
		}
		catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void stats(boolean console) {		
		FileWriter file = null;
		try {
            file = new FileWriter(VBSServer.LOGSTATS, true);
            PrintWriter out = new PrintWriter(file);
            for (int i=0; i < VBSServer.TEAMS; i++) {
            	out.println(System.currentTimeMillis() + "\tTeam=" + (i+1) + "\tCurrentscore=" + SCORE[i]);
	            if (console)
	            	System.out.println("Team=" + (i+1) + "\tCurrentscore=" + SCORE[i]);
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	
	public static void log(String info) {
		FileWriter file = null;
		try {
            file = new FileWriter(VBSServer.LOGFILE, true);
            PrintWriter out = new PrintWriter(file);
            out.println(info);
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	
	public static void logSuccess(Submission sub, String inputLine) {
		
		FileWriter file = null;
		try {
            file = new FileWriter(VBSServer.LOGSUCCESSFILE, true);
            PrintWriter out = new PrintWriter(file);
            
            StringBuilder sb = new StringBuilder("1;");
            sb.append(System.currentTimeMillis()); sb.append(";");
            sb.append(sub.team); sb.append(";");
            sb.append(sub.media); sb.append(";");
			//sb.append(sub.video); sb.append(";");
            sb.append(sub.time); sb.append(";");
            /*sb.append(sub.start); sb.append(";");
            sb.append(sub.stop); sb.append(";");*/
            sb.append(sub.score); sb.append(";");
            sb.append(inputLine);
            
            out.println(sb.toString());
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	

	public static void logNoSuccess(Submission sub, String inputLine) {
		
		FileWriter file = null;
		try {
            file = new FileWriter(VBSServer.LOGSUCCESSFILE, true);
            PrintWriter out = new PrintWriter(file);
            
            StringBuilder sb = new StringBuilder("0;");
            sb.append(System.currentTimeMillis()); sb.append(";");
            sb.append(sub.team); sb.append(";");
            sb.append(sub.media); sb.append(";");
			//sb.append(sub.video); sb.append(";");
            sb.append(sub.time); sb.append(";");
            /*sb.append(sub.start); sb.append(";");
            sb.append(sub.stop); sb.append(";");*/
            sb.append(sub.score); sb.append(";");
            sb.append(inputLine);
            
            out.println(sb.toString());
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	
	public static void play(String filename, int sleepTime) {
		//String command = VLC + VLCEXE + VBSServer.CUESDIR + "/" + filename + "cue.mp4";
		try {
			//Process p = Runtime.getRuntime().exec(command);
			Process p = new ProcessBuilder("/opt/local/bin/ffplay","-autoexit","-nodisp",filename).start();
			//Process p = new ProcessBuilder("/usr/bin/afplay",filename).start();
			//System.out.println("\tplaying " + command);
            System.out.println("\tplaying " + filename);
			//Thread.sleep(22000);
			Thread.sleep(sleepTime);
			//VBSServer.readGroundTruth(filename + ".txt");
			VBSServer.readGroundTruth(filename);
			p.destroy();
			//p.waitFor();
			Thread.sleep((int)(MAX_TASK_SOLVE_TIME));
		} catch (IOException e) {
			//System.out.println("CANNOT EXECUTE: " + command);
            System.out.println("can't play: " + filename);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
}
