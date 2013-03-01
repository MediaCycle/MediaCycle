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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.JPanel;
 
public class ChartPanel extends JPanel {
  
  private static final long serialVersionUID = -2672479058214769573L;

  private double[] score;
 
  private String[] names;
 
  private String title;
  Color blue = new Color(0,0,255);
  Color black = new Color(0,0,0);
  Color gray = new Color(128,128,128);
  Color white = new Color(255,255,255);
  Color darkgray = new Color(30,30,30);
  Color yellow = new Color(255,255,0);
  Color green = new Color(0,255,0);
  Color red = new Color(255,0,0);
  //Color[] colors = new Color[VBSServer.TEAMS];
	Color[] colors = new Color[11];
  Font titleFont = new Font("Verdana", Font.BOLD, 20);
  Font labelFont1 = new Font("Verdana", Font.PLAIN, 16);
  Font labelFont2 = new Font("Arial Black", Font.PLAIN, 16);
  Font labelFont3 = new Font("Arial Black", Font.PLAIN, 14);
 
  public ChartPanel(double[] s, String[] n, String t) {
    names = n;
    score = s;
    title = t;
    
    colors[0] = new Color(255,0,0);
    colors[1] = new Color(255,128,0);
    colors[2] = new Color(240,240,0);
    colors[3] = new Color(128,255,0);
    colors[4] = new Color(0,255,0);
    colors[5] = new Color(0,255,128);
    /*colors[6] = new Color(0,255,255);
    colors[7] = new Color(0,127,255);
    colors[8] = new Color(0,0,255);
    colors[9] = new Color(128,0,255);
    colors[10] = new Color(255,0,255);
    colors[10] = new Color(255,0,128);
    //colors[11] = new Color(25,0,159);
     */
    
    this.setBackground(darkgray);
  }
 
  public void paintComponent(Graphics g) {
    
	super.paintComponent(g);
	    
    Dimension d = getSize();
    int clientWidth = d.width;
    
    
    FontMetrics titleFontMetrics = g.getFontMetrics(titleFont);    
    FontMetrics labelFont1Metrics = g.getFontMetrics(labelFont1);
    FontMetrics labelFont2Metrics = g.getFontMetrics(labelFont2);
 
    int titleWidth = titleFontMetrics.stringWidth(title);
    int y = titleFontMetrics.getAscent();
    int x = (clientWidth - titleWidth) / 2;
    
    g.setColor(white);
    g.setFont(titleFont);
    g.drawString(title, x, y);
    
    g.setColor(yellow);
    if (VBSServer.TASK_NUM < 1 || VBSServer.TASK_END_TIME < System.currentTimeMillis()) {
        g.drawString("No task active!", 10,20);
	} else {
	    int remaining = (int)(VBSServer.TASK_END_TIME - System.currentTimeMillis());
	    if (remaining >= 0) {
	        remaining /= 1000;
	        int secs = remaining % 60;
	        int mins = remaining / 60;
	        String Ssecs = "" + secs;
	        if (secs < 10) Ssecs = "0" + secs;
	        String Smins = "" + mins;
	        if (mins < 10) Smins = "0" + mins;
	        g.drawString("Task " + VBSServer.TASK_NUM + ": " + Smins + ":" + Ssecs, 10,20);
	    } else {
	        g.drawString("Task " + VBSServer.TASK_NUM + ": Time's up!", 10,20);   
	    }
	}
    
    //draw task numbers
    g.setFont(labelFont2);
    StringBuilder sb = new StringBuilder();
    for (int i=0; i < VBSServer.TASKS.size(); i++) {
    	String task = VBSServer.TASKS.elementAt(i);
    	sb.append(task);
    	sb.append("   ");
    }
    //CF no filename! g.drawString(sb.toString(), 14, 45);
	
	int teamWidth = (clientWidth - 20) / VBSServer.TEAMS;
	int posX = 10;
	int posY = 70; //TOP-MARGIN
	for (int i=0; i < VBSServer.TEAMS; i++) {
	
		if (i == 8) {
			g.setColor(white);
			g.drawLine(posX-1, posY+6, posX-1, posY-18);
		}
		
		//colored rectangle
		g.setColor(colors[i]);
		g.fillRect(posX, posY+10, teamWidth-2, 80);
		
		String teamNum = "Team " + (i+1);
		String teamName = names[i];
		int tnumWidth = labelFont1Metrics.stringWidth(teamNum);
		int tnWidth = labelFont2Metrics.stringWidth(teamName);
		
		//Team number (e.g. "Team 1") and name in next line (SHADOW)
		g.setColor(black);
		g.setFont(labelFont1);
		g.drawString(teamNum, posX + (teamWidth-tnumWidth)/2 +2, posY+2);
		g.setFont(labelFont2);
		g.drawString(names[i], posX + (teamWidth-tnWidth)/2 + 2, posY + 2 + 28);
		
		//Team number (e.g. "Team 1") and name in next line (FOREGROUND)
		g.setColor(white);
		g.setFont(labelFont1);
		g.drawString(teamNum, posX + (teamWidth-tnumWidth)/2, posY);
		g.setFont(labelFont2);
		g.drawString(names[i], posX + (teamWidth-tnWidth)/2, posY + 28);
		
		//current score
		g.setFont(titleFont);
		String sScore = "" + (int)score[i];
		int sWidth = titleFontMetrics.stringWidth(sScore);
		//current score (SHADOW)
		g.setColor(black);
		g.drawString(sScore, posX + (teamWidth-sWidth)/2 + 2, posY + 2 + 64);
		//current score (FOREGROUND)
		g.setColor(white);
		g.drawString(sScore, posX + (teamWidth-sWidth)/2, posY + 64);
		
		//images
		int Y = posY + 100;
		for (int k=0; k < VBSServer.SUBMISSION[i].size(); k++) {    	
			Submission sub = VBSServer.SUBMISSION[i].elementAt(k);
			if (sub.media.length() == 1)
				sub.media = "0" + sub.media;
			//String filename = sub.media + "small" + VBSServer.DELIM + "frame" + sub.start + ".jpg";
			String filename = sub.media;
			int barHeight = 60;

			if (sub.considered) {
				//correct or incorrect
				if (sub.correct)
					g.setColor(green);
				else
					g.setColor(red);
			}
			else 
				g.setColor(white);
			g.fillRect(posX, Y, teamWidth, barHeight+6);
			
			try {
				BufferedImage image = null;
				if (sub.media != null && sub.media.length() > 0) {
					image = ImageIO.read(new File(filename)); //resolution 80x54
				}
				
				if (image != null) {
					//image 
					int X = posX;
					if ((posX + (teamWidth-80)/2) > X)
						X = posX + (teamWidth-80)/2;
					g.drawImage(image, X, Y+3, null);
				}
				
			} catch (IOException e) {
				System.out.println("Exception: could not read file " + filename);
				sub.media = "";
			}
			
			//show score
			g.setColor(black);
			g.setFont(titleFont);
			g.drawString(""+sub.score, posX + 8 + /*1*/2, Y + 58 + /*1*/2);
			g.setColor(white);
			g.drawString(""+sub.score, posX + 8, Y + 58);
			
			Y += barHeight + 12;
			if (Y >= 1000)
				break;
		}
		
		posX += teamWidth;
		
	}
  }
}