<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:marc="http://www.loc.gov/MARC21/slim" exclude-result-prefixes="marc">
    <xsl:output method="text" indent="no" encoding="UTF-8" media-type="text/plain"/>
    <xsl:template match="/">
        <xsl:text>id</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>name</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>voyager_id</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>enumeration</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>date_issued</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>copyright_status</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>publication_status</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>copyright_holder_name</xsl:text>
        <xsl:text>&#09;</xsl:text>
        <xsl:text>permission_notes</xsl:text>
        <xsl:text>&#10;</xsl:text>
        <xsl:for-each select="/marc:collection/marc:record">
            <!--             id        -->
            <xsl:value-of select="normalize-space(marc:datafield[@tag=980])"/>
            <xsl:text>&#09;</xsl:text>
            <!--             name       -->
            <xsl:for-each select="marc:datafield[@tag=245]">
                <xsl:call-template name="subfieldSelect">
                    <xsl:with-param name="codes">abc</xsl:with-param>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:text> </xsl:text>
            <xsl:value-of select="normalize-space(marc:datafield[@tag=983])"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="normalize-space(marc:datafield[@tag=984])"/>
            <xsl:text>&#09;</xsl:text>
            <!--             voyager ID       -->
            <xsl:value-of select="normalize-space(marc:controlfield[@tag=001])"/>
            <xsl:text>&#09;</xsl:text>
            <!--            enumeration        -->
            <xsl:value-of select="normalize-space(marc:datafield[@tag=983])"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="normalize-space(marc:datafield[@tag=984])"/>
            <xsl:text>&#09;</xsl:text>
            <!--            date_issued        -->
            <xsl:value-of select="normalize-space(marc:datafield[@tag=984])"/>
            <xsl:text>&#09;</xsl:text>
            <!--            copyright_status        -->
            <xsl:text>&#09;</xsl:text>
            <!--            publication_status        -->
            <xsl:text>&#09;</xsl:text>
            <!--            copyright_holder_name        -->
            <xsl:text>&#09;</xsl:text>
            <!--            permission_notes        -->
            <xsl:text>&#10;</xsl:text>
        </xsl:for-each>
    </xsl:template>
    <xsl:template name="subfieldSelect">
        <xsl:param name="codes">abcdefghijklmnopqrstuvwxyz</xsl:param>
        <xsl:param name="delimeter">
            <xsl:text/>
        </xsl:param>
        <xsl:variable name="str">
            <xsl:for-each select="marc:subfield">
                <xsl:if test="contains($codes, @code)">
                    <xsl:value-of select="text()"/>
                    <xsl:value-of select="$delimeter"/>
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="substring($str,1,string-length($str)-string-length($delimeter))"/>
    </xsl:template>
</xsl:stylesheet>

