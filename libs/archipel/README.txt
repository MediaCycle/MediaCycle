First clean the HTML code of the albums with this terminal command on the .../albums/ directory:
for html in *.html */*.html */*/*.html; do mv $html $html.old;sed -e 's/<br[^>]*>/<br\/>/g' -e 's/Content-type">/Content-type"\/>/g' -e 's/jpg">/jpg"\/>/g' -e 's/.png" border="0">/.png" border="0"\/>/g' $html.old > $html;rm $html.old;done

To execute the Archipel HTML to XML converter, from the project binary directory, use:
java -jar libs/archipel/archipelHtmlToXml.jar .../albums/
(adapt .../albums)

