package htmlparser;

import java.util.ArrayList;

public class ArchipelData {

	public ArrayList<String> description = new ArrayList<String>();
	public ArrayList<String> islandkeys = new ArrayList<String>();
	public ArrayList<String> glossarykeys = new ArrayList<String>();
	public String artist = "";
	boolean anthology = false;
	public String ref = "";
	public String title = "";
	public String year = "";
	public String producer = "";
	public String thumb = "";
	public String link = "";
	public String extra = "";
	public ArrayList<String> tracksfiles = new ArrayList<String>(); // not printed in XML!
	public ArrayList<String> trackspath = new ArrayList<String>();
	public ArrayList<String> tracksartist = new ArrayList<String>();
	public ArrayList<String> tracksname = new ArrayList<String>();

	public ArchipelData() {}

	public String trackNumber(String ref, int i) {
		i += 1;
		int counter = 0;
		String out = "";
		
	
		// Double CDs
		if (ref.equals("XO739I")){//XO739I 101-128 201-208
			System.out.println("Reference '"+ref+"'");
			if(i>=1 && i<=28)
				counter = 100+i; 
			else if (i>28 && i<=28+8)
				counter = 200+i-28;
			out += counter;
		}
		else if (ref.equals("HB4755")){//HB4755 301-309 401-410
			if(i>=1 && i<=9)
				counter = 300+i; 
			else if (i>9 && i<=9+10)
				counter = 400+i-9;
			out += counter;
		}
		else if (ref.equals("XM665D")){//XM665D 101-112 201-211
			if(i>=1 && i<=12)
				counter = 100+i; 
			/*else if (i>12 && i<=12+11)
				counter = 200+i-12;*/
			else if (i>12 && i<=12+11)
				counter = 200+i-12;
			else if (i==24)//missing track, ugly hack...
				counter = 101;
			out += counter;
		}
		else {
			if( i<10)
				out += "0"+i;
			else
				out += i;
		}
		return out;
	}
	
	public String printXML() {
		String out = "<?xml version='1.0' encoding=\"UTF-8\"?>\n";
		out+="<archipel reference=\""+ref+"\">\n";
		if (islandkeys.size() > 0 || glossarykeys.size() > 0) {
			out+="<keywords>\n";
			if (islandkeys.size() > 0) {
				out+="<ilot>\n";
				for (int i=0; i<islandkeys.size(); i++) {
					out+="\t<keyword><![CDATA["+islandkeys.get(i)+"]]></keyword>\n";
				}
				out+="</ilot>\n";
			}
			if (glossarykeys.size() > 0) {
				out+="<glossaire>\n";
				for (int i=0; i<glossarykeys.size(); i++) {
					out+="\t<keyword><![CDATA["+glossarykeys.get(i)+"]]></keyword>\n";
				}
				out+="</glossaire>\n";
			}
			out+="</keywords>\n";
		}
		if (description.size()>0) {
			out+="<description>\n";
			for (int i=0; i<description.size(); i++) {
				out+="\t<p><![CDATA["+description.get(i)+"]]></p>\n";
			}
			out+="</description>\n";
		}
		out+="<info>\n";
			out+="\t<artist><![CDATA["+artist+"]]></artist>\n";
			out+="\t<type>";
			if (anthology) { out+="ANTHOLOGY"; }
			else { out+="SOLO"; }
			out+="</type>\n";
			out+="\t<title><![CDATA["+title+"]]></title>\n";
			out+="\t<reference><![CDATA["+ref+"]]></reference>\n";
			out+="\t<year>"+year+"</year>\n";
			out+="\t<producer><![CDATA["+producer+"]]></producer>\n";
			out+="\t<thumb><![CDATA["+thumb+"]]></thumb>\n";
			out+="\t<link><![CDATA["+link+"]]></link>\n";
			out+="\t<extra><![CDATA["+extra+"]]></extra>\n";
		out+="</info>\n";
		if (tracksname.size() > 0) {
			out+="<tracks>\n";
			for (int i=0; i<tracksname.size(); i++) {
				out+="\t<track position=\""+i+"\">\n";
				out+="\t\t<artist><![CDATA["+tracksartist.get(i)+"]]></artist>\n";
				//out+="\t\t<path><![CDATA["+trackspath.get(i)+"]]></path>\n";
				out+="\t\t<path><![CDATA["+ref+"/audio/"+ref+"-"+trackNumber(ref,i)+".ogg]]></path>\n";
				out+="\t\t<title><![CDATA["+tracksname.get(i)+"]]></title>\n";
				out+="\t</track>\n";
			}
			out+="</tracks>\n";
		}
		out+="</archipel>";
		return out;
	}

}
