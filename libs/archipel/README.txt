First clean the HTML code of the albums with:
for html in *.html;do mv $html $html.old;sed -e 's/<br[^>]*>/<br\/>/g' -e 's/Content-type">/Content-type"\/>/g' -e 's/jpg">/jpg"\/>/g'  $html.old > $html;rm $html.old;done

To execute the archival HTML to XML converter, from the binary directory use:
java -jar Archipel_Html2Xml.jar [.../albums]


