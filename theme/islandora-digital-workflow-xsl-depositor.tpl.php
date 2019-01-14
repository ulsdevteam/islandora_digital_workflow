<?php

/**
* @file
* islandora-digital-workflow-xsl-depositor.tpl display template.
*
* Variables available:
* - $value string the value to set for the depositor
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

  <xsl:template match="/mods:mods/mods:name">
    <xsl:choose>
      <xsl:when test="./mods:role/mods:roleTerm[@text='depositor']">
      <!-- If the node existed, replace the value for the namePart based on what was passed -->
        <namePart xmlns="http://www.loc.gov/mods/v3">
          <xsl:text><?php print $value; ?></xsl:text>
        </namePart>
        <role xmlns="http://www.loc.gov/mods/v3">
          <roleTerm xmlns="http://www.loc.gov/mods/v3" type="text">depositor</roleTerm>
        </role>
      </xsl:when>
      <xsl:otherwise>
      <!-- If the node existed, insert the value for the namePart as well as
           the role/roleTerm[@text="depositor"] nodes based on what was passed -->
      <name xmlns="http://www.loc.gov/mods/v3">
        <namePart xmlns="http://www.loc.gov/mods/v3">
          <xsl:text><?php print $value; ?></xsl:text>
        </namePart>
        <role xmlns="http://www.loc.gov/mods/v3">
          <roleTerm xmlns="http://www.loc.gov/mods/v3" type="text">depositor</roleTerm>
        </role>
      </name>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>