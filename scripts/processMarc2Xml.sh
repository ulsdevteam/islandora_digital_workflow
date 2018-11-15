#!/bin/bash

if [ -z $1 ]
    then
    echo "Please supply filename of Marc records to process"
    exit 0
fi

### not used:  STARTDIR=`pwd`
MARCSPLIT_XSL="../transforms/marcxmlSplitter.xsl"
VOYAGERID_XSL="../transforms/voyagerIDgrabber.xsl"
MARC2DB_XSL="../transforms/marc2batchSpreadSheet.xsl"
SAXON="java -jar /usr/local/dlxs/prep/bin/saxon/saxon8.jar"
BASENAME=`basename $1`
TEMP_XML="/tmp/marc.xml"

######################################################
# transform the MARC into MARCXML
######################################################

echo "transforming MARC -> XML ..."
/usr/bin/marc2xml $1 > $TEMP_XML
echo "done.\n";

######################################################
# perform the XSLT transformation to split MARC records
######################################################

echo "splitting MARC records..."
$SAXON -s "$TEMP_XML" $MARCSPLIT_XSL


######################################################
# rename split marc files with Voyager ID
######################################################

for i in split/*.xml
do
VOYAGERID=`$SAXON -s $i $VOYAGERID_XSL`
cp $i /usr/local/dlxs/prep/m/marcxml/$VOYAGERID.marcxml.xml
done

######################################################
# create checkin spreadsheet
######################################################

echo "making check-in..."
$SAXON -s "$TEMP_XML" -o $BASENAME.txt $MARC2DB_XSL

######################################################
# remove the temporary marc-xml file.
######################################################

echo "cleaning up..."
rm $TEMP_XML
rm -r split
echo "all done."


