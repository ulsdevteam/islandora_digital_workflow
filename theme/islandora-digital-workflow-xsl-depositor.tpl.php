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

  <!--  If the element exists, do what you want to do -->
  <xsl:template match="/mods:mods/mods:name/mods:role/mods:roleTerm[@type='text']">
    <xsl:copy><?php print $value; ?></xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
      <xsl:if test="not(mods:name/mods:role/mods:roleTerm[@type='text'])">
        <name xmlns="http://www.loc.gov/mods/v3">
          <namePart xmlns="http://www.loc.gov/mods/v3"><?php print $value; ?></namePart>
          <role xmlns="http://www.loc.gov/mods/v3">
            <roleTerm xmlns="http://www.loc.gov/mods/v3" type="text">depositor</roleTerm>
          </role>
        </name>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>