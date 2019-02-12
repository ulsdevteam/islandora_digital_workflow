<?php

/**
* @file
* islandora-digital-workflow-xsl-publication-status.tpl display template.
*
* Variables available:
* - $value string the value to set for the publication_status
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
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright/@publication.status">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:text><?php print $value; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!-- If the copyright exists, but the copyright.status attribute does not -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright[not(@publication.status)]">
    <xsl:copy>
      <xsl:attribute name="publication.status"><?php print $value; ?></xsl:attribute>
      <xsl:apply-templates select="@*|node()" />
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist at all, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
      <xsl:if test="not(mods:accessCondition/copyrightMD:copyright)">
        <accessCondition xmlns="http://www.loc.gov/mods/v3">
          <copyright xmlns="http://www.cdlib.org/inside/diglib/copyrightMD" publication.status="<?php print $value; ?>"/>
        </accessCondition>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>