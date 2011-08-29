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
				out+="\t\t<path><![CDATA["+trackspath.get(i)+"]]></path>\n";
				out+="\t\t<title><![CDATA["+tracksname.get(i)+"]]></title>\n";
				out+="\t</track>\n";
			}
			out+="</tracks>\n";
		}
		out+="</archipel>";
		return out;
	}

}
