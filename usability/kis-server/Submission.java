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

public class Submission {

	public int team;
	public String media;
	/*public String video;
	public int start;
	public int stop;*/
	public int score;
	public boolean correct;
	public boolean considered;
	public long time;
	
	public Submission(int team, String media) {
		this.team = team;
		this.media = media;
		this.correct = false;
		this.considered = false;
	}
	
	/*public Submission(int team, String video, int start, int stop) {
		this.team = team;
		this.video = video;
		this.start = start;
		this.stop = stop;
		this.correct = false;
		this.considered = false;
	}*/
}
