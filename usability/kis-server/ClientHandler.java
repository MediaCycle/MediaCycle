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

import java.io.*;
import java.net.Socket;
import java.net.SocketAddress;

public class ClientHandler extends Thread {
	
	Socket clientSock;
	int team=-1;
	String media=null;
	/*String video=null;
	int start=-1;
	int stop=-1;*/
	
	public ClientHandler(Socket clientSock) {
		this.clientSock = clientSock;
	}
	
	void error(String info, PrintWriter out, String inputLine, String sockAddressS) {
		out.write("HTTP/1.1 400 " + info + "\r\n");
		out.close();
		System.out.println("wrong format: " + inputLine + " #IP=" + sockAddressS);
		VBSServer.log(info + " " + inputLine + " #IP=" + sockAddressS);
	}

	public void run() {
		
		
		//SUBMISSION FORMAT:
		//http://localhost:8080/team=4/file=.../a.wav
		
		try {
			PrintWriter out = new PrintWriter(clientSock.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(clientSock.getInputStream()));
			String inputLine;
			
			SocketAddress sockAddress = clientSock.getRemoteSocketAddress();
			String sockAddressS = "";
			if (sockAddress != null)
				sockAddressS = sockAddress.toString();
			
			inputLine = in.readLine();
			
			//check format
			int idxGET = inputLine.indexOf("GET /");
			int idxTeam = inputLine.indexOf("team=");
			int idxMedia = inputLine.indexOf("file=");
			/*int idxVideo = inputLine.indexOf("video=");
			int idxSegStart = inputLine.indexOf("segstart=");
			int idxSegStop = inputLine.indexOf("segstop=");*/
					
			if (idxGET == -1 || idxTeam == -1 || idxMedia == -1 /*idxSegStart == -1 || idxSegStop == -1 || idxVideo == -1 || 
				idxTeam > idxVideo || idxVideo > idxSegStart || idxSegStart > idxSegStop*/) {
					error("WRONG FORMAT", out, inputLine, sockAddressS);
					return;
			}
			
			//parse header values
			try {
				team = Integer.parseInt(inputLine.substring(idxTeam+5, inputLine.indexOf("/", idxTeam)));
				///media =  inputLine.substring(idxMedia+5, inputLine.indexOf(" ", idxMedia));
				media =  inputLine.substring(idxMedia+5, inputLine.indexOf(" HTTP", idxMedia));
				/*video = inputLine.substring(idxVideo+6, inputLine.indexOf("/", idxVideo));
				start = Integer.parseInt(inputLine.substring(idxSegStart+9, inputLine.indexOf("/", idxSegStart))); 
				stop = Integer.parseInt(inputLine.substring(idxSegStop+8, inputLine.indexOf(" ", idxSegStop)));*/
			}
			catch (NumberFormatException nfe) {
				error("WRONG FORMAT LINE", out, inputLine, sockAddressS);
				return;
			}
			
			//check team number
			/*
			 * 2013:
			 * 1 DCU (IR, Dublin)
			 * 2 Joanneum Research (AT, Graz)
			 * 3 FRTRD & Beijing University
			 * 4 NII-UIT-VBS (Japan/Vietnam)  - National Institute of Informatics (Japan) and The University of Information Technology (Vietnam)
			 * 5 VideoCycle (Belgium)
			 * 6 AAU Video Browser 
			 */
			if (team < 1 || team > VBSServer.TEAMS) {
				error("WRONG FORMAT TEAM NUMBER", out, inputLine, sockAddressS);
				return;
			}
			
			/*//check segment
			if (start > stop || start < 1 || stop < 1) { // || start > 210000 || stop > 210000) {
				error("WRONG FORMAT", out, inputLine, sockAddressS);
				return;
			}*/
			
			
			//check for correctness only if TEAM NOT YET FINISHED
			if (!VBSServer.TEAM_FINISHED[team-1]) {
			    			    
			    //Submission sub = new Submission(team, video, start, stop);
				Submission sub = new Submission(team, media);
			    
			    //only count when task active
			    if (VBSServer.TASK_NUM >= 1 && VBSServer.TASK_END_TIME > System.currentTimeMillis()) {
	    			
			    	//count submission
			    	VBSServer.SUBMISSON_COUNT[team-1]++;
			    	sub.considered = true;
			    	
			    	//check for scoring
			    	//if (start >= VBSServer.GTSTART-VBSServer.OVERLAPPING && stop <= VBSServer.GTSTOP+VBSServer.OVERLAPPING) {
					
					System.out.println("\tTarget " + VBSServer.GTFILE);
					System.out.println("\tSubmission " + sub.media);
					if (sub.media.equals(VBSServer.GTFILE)) {
	    			//    //correct
	    				
	    				sub.correct = true;
	    				
	    			    VBSServer.TEAM_FINISHED[team-1] = true;
	    			    long time = VBSServer.TASK_TIME[team-1] = System.currentTimeMillis()-VBSServer.TASK_START_TIME;
	    			    sub.time = time;
	    			    
	    			    //scoring!
	    			    if (time <= VBSServer.MAX_TASK_SOLVE_TIME) {
	    			        
	    			    	int submissionCount = VBSServer.SUBMISSON_COUNT[team-1];
	    			    	int penaltyDiv = 1;
	    			    	if (submissionCount > 1)
	    			    		penaltyDiv = submissionCount - 1;
	    			    	double score = (100 - (double)time / VBSServer.MAX_TASK_SOLVE_TIME * 50) / penaltyDiv;
	    			    		
	    			        sub.score = (int)Math.ceil(score);
	    			        VBSServer.SCORE[team-1] += sub.score;
	    			        VBSServer.saveScore();
	    			        VBSServer.chart.invalidate(); //refresh
	    			        System.out.println("Correct answer from team " + team + " after " + time + "ms" + "! Got score: " + sub.score);
	    		
	    			        VBSServer.logSuccess(sub, inputLine + " #IP=" + sockAddressS);
							
	    			    }
	    			} else {
	    				VBSServer.logNoSuccess(sub, inputLine + " #IP=" + sockAddressS);
					}
			    	
			    }
    			
			    //add submission and repaint
    			VBSServer.SUBMISSION[team-1].add(0,sub);
    			VBSServer.frame.invalidate();
    	        VBSServer.chart.invalidate();
    	        VBSServer.frame.validate();
    	        VBSServer.frame.repaint();
    			
			}
			
			//String loginfo = team + ":" + video + ":" + start + ":" + stop + ":" + (System.currentTimeMillis()-VBSServer.TASK_START_TIME) + ":" + VBSServer.SUBMISSON_COUNT[team-1];
			String loginfo = team + ":" + media + ":" + (System.currentTimeMillis()-VBSServer.TASK_START_TIME) + ":" + VBSServer.SUBMISSON_COUNT[team-1];
			String responseInfo = "<html><body>" + loginfo + "</body></html>";
			//VBSServer.log(System.currentTimeMillis()+ ": "  + inputLine + " | " + loginfo + " //GTSTART=" + VBSServer.GTSTART + " GTSTOP=" + VBSServer.GTSTOP  + " #IP=" + sockAddressS);
			//VBSServer.log(System.currentTimeMillis()+ ": "  + inputLine + " | " + loginfo + " //file=" + media + " #IP=" + sockAddressS);
			VBSServer.log((System.currentTimeMillis()-VBSServer.TASK_START_TIME)+ ";" + media);
			
			out.write("HTTP/1.1 200 OK\r\n");
			out.write("Content-Type: text/html\r\n");
			out.write("Content-Length: " + responseInfo.length() + "\r\n");
			out.write("\r\n");
			out.write(responseInfo);
			out.close();
			
			
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
