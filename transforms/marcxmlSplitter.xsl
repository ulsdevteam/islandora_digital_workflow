<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:marc="http://www.loc.gov/MARC21/slim" exclude-result-prefixes="marc">
    <xsl:output method="xml" indent="yes" encoding="utf-8"/>
    <xsl:template match="/">
        <xsl:for-each select="/marc:collection/marc:record">

            <xsl:variable name="number">
                <xsl:number value="position()" format="1" />
            </xsl:variable>
            <xsl:variable name="filename" select="concat(string($number), '.marcxml.xml')"/>
           <xsl:result-document href="$filename">
                <xsl:copy-of select="." />
            </xsl:result-document>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
