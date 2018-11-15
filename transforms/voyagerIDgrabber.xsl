<?xml version="1.0" encoding="UTF-8"?>
 <xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:marc="http://www.loc.gov/MARC21/slim" exclude-result-prefixes="marc">
<xsl:output method="text" indent="no" encoding="UTF-8" media-type="text/plain"/>
     <xsl:template match="/">
             <xsl:value-of select="normalize-space(//marc:controlfield[@tag=001])"/>
     </xsl:template>
 </xsl:stylesheet>
