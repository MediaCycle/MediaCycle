package htmlparser;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Vector;

import net.n3.nanoxml.IXMLParser;
import net.n3.nanoxml.IXMLReader;
import net.n3.nanoxml.StdXMLReader;
import net.n3.nanoxml.XMLElement;
import net.n3.nanoxml.XMLException;
import net.n3.nanoxml.XMLParserFactory;

public class HtmlParser {

	private static final int CONTEXT_ALBUM = 0;
	private static final int CONTEXT_TRACKS = 1;
	private static final int CONTEXT_TRACK = 2;
	private ArchipelData archipeld;
	private int context;
	private boolean printfirstparsing = false;

	private final ArrayList<String> globalilotkeywords;
	private final ArrayList<String> globalglossairekeywords;

	public HtmlParser(String[] args) {

		double startmillis = System.currentTimeMillis();

		globalilotkeywords = new ArrayList<String>();
		globalglossairekeywords = new ArrayList<String>();

		String initfolderpath="html/albums/";
		String folderpath="html/albums/";
		if (args.length >= 1) {
			initfolderpath = args[0];
			folderpath = args[0];
			System.out.println("folder path is '"+args[0]+"'");
		} else {
			System.out.println("no args -> 'html/albums/' will be used as folder path");
		}

		File dir = new File(folderpath);
		String[] files = dir.list();

		for (int i=0; i<files.length; i++) {
			String filepath = files[i];
			XMLElement xml = openHTML(folderpath+filepath);
			if (xml != null) {
				context = CONTEXT_ALBUM;
				archipeld = new ArchipelData();
				String tfolder = filepath.substring(0,filepath.indexOf("."));
				// if (tfolder.equals("XL012V")) { printfirstparsing = true; }
				Vector<XMLElement> ms = xml.getChildrenNamed("body");
				if (ms.size() > 0) {
					if (printfirstparsing) { System.err.println("PARSING ALBUM"); }
					recursiveNodeParsing(ms.get(0), null);
					// getting the tracks
					// getting folder
					context = CONTEXT_TRACKS;
					if (printfirstparsing) { System.err.println("PARSING TRACKS"); }
					XMLElement tracks = openHTML(folderpath+tfolder+"/tracks.html");
					if (tracks != null) {
						ms = tracks.getChildrenNamed("body");
						if (ms.size() > 0) { recursiveNodeParsing(ms.get(0), null); }
						// all tracks are in, now getting all tracks details
						if (printfirstparsing) { System.err.println("PARSING TRACK BY TRACK"); }
						context = CONTEXT_TRACK;
						for (int t=0; t<archipeld.tracksfiles.size(); t++) {
							XMLElement track = openHTML(folderpath+tfolder+"/"+archipeld.tracksfiles.get(t));
							if (track != null) {
								ms = track.getChildrenNamed("body");
								if (ms.size() > 0) { recursiveNodeParsing(ms.get(0), null); }
							}
						}
					}

				}
				if (printfirstparsing) {
					System.out.println(archipeld.printXML());
					printfirstparsing = false;
				}

				if (archipeld.anthology) { System.out.println(tfolder + " // "+archipeld.tracksfiles.size()); }

				File f = new File(folderpath+tfolder+".xml");
				try {
					FileWriter fw = new FileWriter(f);
					fw.write(archipeld.printXML());
					fw.flush();
					fw.close();
				} catch (IOException e) {
					e.printStackTrace();
				}

			}

		}

		if (globalglossairekeywords.size() > 0 || globalilotkeywords.size() > 0) {
			File f = new File(initfolderpath+"_keywords.xml");
			try {
				FileWriter fw = new FileWriter(f);
				fw.write("<?xml version='1.0' encoding=\"UTF-8\"?>\n");
				fw.write("<keywords>\n");
				if (globalilotkeywords.size() > 0) {
					fw.write("\t<ilot>\n");
					for (int i=0; i<globalilotkeywords.size(); i++) {
						fw.write("\t\t<keyword><![CDATA["+globalilotkeywords.get(i)+"]]></keyword>\n");
					}
					fw.write("\t</ilot>\n");
				}
				fw.flush();
				if (globalglossairekeywords.size() > 0) {
					fw.write("\t<glossaire>\n");
					for (int i=0; i<globalglossairekeywords.size(); i++) {
						fw.write("\t\t<keyword><![CDATA["+globalglossairekeywords.get(i)+"]]></keyword>\n");
					}
					fw.write("\t</glossaire>\n");
				}
				fw.write("</keywords>\n");
				fw.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		double laps = System.currentTimeMillis() - startmillis;
		System.out.println("processed in "+(laps/1000)+" seconds");

	}

	private XMLElement openHTML(String fpath) {
		XMLElement xml = null;
		if (fpath.indexOf(".html") != -1) {
			try {
				IXMLParser parser = XMLParserFactory.createDefaultXMLParser();
				IXMLReader reader = StdXMLReader.fileReader(fpath);
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
			System.out.println("unable to open '"+fpath+"'");
		}
		if (xml != null) {
			System.out.println("starting to parse '"+fpath+"'");
		}
		return xml;
	}

	private boolean searchContentFor(XMLElement xe, String needle) {
		String c = xe.getContent();
		if (c != null && c.indexOf(needle) > -1) { return true; }
		Vector<XMLElement> ms = xe.getChildren();
		for (int i=0; i<ms.size(); i++) {
			c = ms.get(i).getContent();
			if (c != null && c.indexOf(needle) > -1) { return true; }
		}
		return false;
	}

	private boolean isGlossaire(XMLElement xe) {
		return searchContentFor(xe, "Glossaire:");
	}

	private boolean isIlot(XMLElement xe) {
		return searchContentFor(xe, "Îlots:");
	}

	private String collectAllContent(XMLElement xe) {
		String out = "";
		if (xe.getContent() != null) { out += xe.getContent() +" "; }
		Vector<XMLElement> ms = xe.getChildren();
		for (int i=0; i<ms.size(); i++) {
			out += collectAllContent(ms.get(i));
		}
		return out;
	}

	private void recursiveNodeParsing(XMLElement xe, XMLElement parente) {
		String c = xe.getContent();
		if (c!=null) { c = c.trim(); }
		String parenttagName = "?";
		if (parente != null) { parenttagName = parente.getName(); }
		String tagName = xe.getName();
		if (printfirstparsing) {
			System.out.println("-------------------------------------");
			System.out.println("name of parent tag: "+parenttagName);
			System.out.println("name of tag: "+tagName);
			System.out.println("content: "+c);
		}
		if (printfirstparsing && xe.getAttributes() != null) {
			System.out.println("attributes: "+xe.getAttributeCount());
			if (xe.getAttribute("id", null) != null) { System.out.println("\tid:"+xe.getAttribute("id", null)); }
			if (xe.getAttribute("src", null) != null) { System.out.println("\tsrc:"+xe.getAttribute("src", null)); }
			if (xe.getAttribute("href", null) != null) { System.out.println("\thref:"+xe.getAttribute("href", null)); }
			if (xe.getAttribute("target", null) != null) { System.out.println("\ttarget:"+xe.getAttribute("target", null)); }
			if (xe.getAttribute("class", null) != null) { System.out.println("\tclass:"+xe.getAttribute("class", null)); }
			if (xe.getAttribute("style", null) != null) { System.out.println("\tstyle:"+xe.getAttribute("style", null)); }
			if (xe.getAttribute("data-album", null) != null) { System.out.println("\tdata-album:"+xe.getAttribute("data-album", null)); }
		}
		if (context == CONTEXT_ALBUM) {
			if (c != null && c.length() > 2 && tagName != null && tagName.equals("a") && isIlot(parente)) {
				String k = c.toLowerCase();
				archipeld.islandkeys.add(k);
				if(!globalilotkeywords.contains(k)) { globalilotkeywords.add(k); }
			} else if (c != null && c.length() > 2 && tagName != null && tagName.equals("a") && isGlossaire(parente)) {
				String k = c.toLowerCase();
				archipeld.glossarykeys.add(c.toLowerCase());
				if(!globalglossairekeywords.contains(k)) { globalglossairekeywords.add(k); }
			} else if (c != null && c.length() > 2 && tagName != null && tagName.equals("a")) {
				String thref = xe.getAttribute("href", null);
				if (thref != null && thref.indexOf("www.lamediatheque.be") != -1) { archipeld.link = thref; }
			} else if (c != null && c.length() > 2) {
				if (tagName != null && tagName.equals("h2")) {
					archipeld.artist = c;
					if (c.equals("ANTHOLOGIE")) { archipeld.anthology = true; }
				} else if (tagName == null && parenttagName != null && parenttagName.equals("div")) {
					if (c.indexOf(archipeld.ref) != -1 && c.indexOf(")") != -1) {
						archipeld.extra = c;
						String tmp = c.replaceAll(archipeld.ref, "");
						tmp = tmp.substring(tmp.indexOf(")")+1).trim();
						archipeld.producer = tmp.substring(0, tmp.indexOf("(")).trim();
						archipeld.year = tmp.substring(tmp.indexOf("(")+1, tmp.indexOf(")")).trim();
					}
				} else if (tagName == null && parenttagName != null && parenttagName.equals("li")) { archipeld.title = c; }
			}
			if (tagName != null && tagName.equals("li") && xe.getAttribute("data-album", null) != null) {
				archipeld.ref = xe.getAttribute("data-album", null);
			} else if (tagName != null && tagName.equals("img") && xe.getAttribute("src", null) != null) {
				archipeld.thumb = xe.getAttribute("src", null);
			}
			if (tagName != null && tagName.equals("p")) {
				String tmp = collectAllContent(xe);
				if (tmp != null && tmp.length() > 1) { archipeld.description.add(tmp); }
			}
		} else if (context == CONTEXT_TRACKS) {
			if (
				c != null && c.length() > 2 &&
				tagName != null && tagName.equals("a") &&
				xe.getAttribute("href", null) != null &&
				xe.getAttribute("class", null) != null && xe.getAttribute("class", null).equals("playtrack")) {
				archipeld.tracksfiles.add(xe.getAttribute("href", null));
			}
		} else if (context == CONTEXT_TRACK) {
			if (
				c != null && c.length() > 2 &&
				tagName != null && tagName.equals("div") &&
				xe.getAttribute("class", null) != null && xe.getAttribute("class", null).equals("album_artist")) {
				archipeld.tracksartist.add(c);
			} else if (
				c != null && c.length() > 2 &&
				tagName != null && tagName.equals("div") &&
				xe.getAttribute("class", null) != null && xe.getAttribute("class", null).equals("track_title")) {
				archipeld.tracksname.add(c);
			} else if (
				c != null && c.equals("audio") &&
				tagName != null && tagName.equals("a") &&
				xe.getAttribute("href", null) != null
				) {
				archipeld.trackspath.add(xe.getAttribute("href", null));
			}
		}
		Vector<XMLElement> ms = xe.getChildren();
		for (int i=0; i<ms.size(); i++) { recursiveNodeParsing(ms.get(i), xe); }
	}

	public static final void main(String[] args) throws IOException {
		HtmlParser p = new HtmlParser(args);
	}


}
