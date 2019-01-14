<?php

/**
* @file
* islandora-digital-workflow-xsl-genre.tpl display template.
*
* Variables available:
* - $value string the value to set for the genre
*/
?><?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:copyrightMD="http://www.cdlib.org/inside/diglib/copyrightMD">
<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>

  <!-- IdentityTransform -->
  <xsl:template match="/ | @* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()" />
    </xsl:copy>
  </xsl:template>

  <!--  If the element exists, copy and change it -->
  <xsl:template match="/mods:mods/mods:genre">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:text><?php print $value; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
        <xsl:if test="not(mods:genre)">
            <genre xmlns="http://www.loc.gov/mods/v3"><?php print $value; ?></genre>
        </xsl:if>
        <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>