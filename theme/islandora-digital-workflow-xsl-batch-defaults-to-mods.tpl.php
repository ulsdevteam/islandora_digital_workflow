<?php

/**
* @file
* islandora-digital-workflow-xsl-batch-defaults-to-mods.tpl display template.
*
* Variables available:
*  'depositor' => '',
*  'genre' => '',
*  'type_of_resource' => '',
*  'copyright_status' => '',
*  'publication_status' => '',
*  'rights_holder' => '',
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

<?php if ($depositor): ?>
  <!--  If the element exists, do what you want to do -->
  <xsl:template match="/mods:mods/mods:name/mods:role/mods:roleTerm[@type='text']">
    <xsl:copy><?php print $depositor; ?></xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
      <xsl:if test="not(mods:name/mods:role/mods:roleTerm[@type='text'])">
        <name xmlns="http://www.loc.gov/mods/v3">
          <namePart xmlns="http://www.loc.gov/mods/v3"><?php print $depositor; ?></namePart>
          <role xmlns="http://www.loc.gov/mods/v3">
            <roleTerm xmlns="http://www.loc.gov/mods/v3" type="text">depositor</roleTerm>
          </role>
        </name>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
<?php endif; ?>

<?php if ($genre): ?>
  <!--  If the element exists, copy and change it -->
  <xsl:template match="/mods:mods/mods:genre">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:text><?php print $genre; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
        <xsl:if test="not(mods:genre)">
            <genre xmlns="http://www.loc.gov/mods/v3"><?php print $genre; ?></genre>
        </xsl:if>
        <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
<?php endif; ?>

<?php if ($copyright_status): ?>
  <!--  If the element exists, copy and change it -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright/@copyright.status">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:text><?php print $copyright_status; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!-- If the copyright exists, but the copyright.status attribute does not -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright[not(@copyright.status)]">
    <xsl:copy>
      <xsl:attribute name="copyright.status"><?php print $copyright_status; ?></xsl:attribute>
      <xsl:apply-templates select="@*|node()" />
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist at all, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
      <xsl:if test="not(mods:accessCondition/copyrightMD:copyright)">
        <accessCondition xmlns="http://www.loc.gov/mods/v3">
          <copyright xmlns="http://www.cdlib.org/inside/diglib/copyrightMD" copyright.status="<?php print $copyright_status; ?>"/>
        </accessCondition>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
<?php endif; ?>

<?php if ($publication_status): ?>
  <!--  If the element exists, copy and change it -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright/@publication.status">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:text><?php print $publication_status; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!-- If the copyright exists, but the copyright.status attribute does not -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright[not(@publication.status)]">
    <xsl:copy>
      <xsl:attribute name="publication.status"><?php print $publication_status; ?></xsl:attribute>
      <xsl:apply-templates select="@*|node()" />
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist at all, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
      <xsl:if test="not(mods:accessCondition/copyrightMD:copyright)">
        <accessCondition xmlns="http://www.loc.gov/mods/v3">
          <copyright xmlns="http://www.cdlib.org/inside/diglib/copyrightMD" publication.status="<?php print $publication_status; ?>"/>
        </accessCondition>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
<?php endif; ?>

<?php if ($rights_holder): ?>
  <!--  If the element exists, copy and change it -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright/copyrightMD:rights.holder/copyrightMD:name">
    <xsl:copy>
      <xsl:copy-of select="@*"/>

      <xsl:text><?php print $rights_holder; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!-- If the copyright exists, but the copyright.status attribute does not -->
  <xsl:template match="/mods:mods/mods:accessCondition/copyrightMD:copyright[not(copyrightMD:rights.holder)]">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <rights.holder xmlns="http://www.cdlib.org/inside/diglib/copyrightMD">
        <name xmlns="http://www.cdlib.org/inside/diglib/copyrightMD"><?php print $rights_holder; ?></name>
        <contact xmlns="http://www.cdlib.org/inside/diglib/copyrightMD"/>
      </rights.holder>
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist at all, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
      <xsl:if test="not(mods:accessCondition/copyrightMD:copyright)">
        <accessCondition xmlns="http://www.loc.gov/mods/v3">
          <copyright xmlns="http://www.cdlib.org/inside/diglib/copyrightMD">
            <rights.holder xmlns="http://www.cdlib.org/inside/diglib/copyrightMD">
              <name xmlns="http://www.cdlib.org/inside/diglib/copyrightMD"><?php print $rights_holder; ?></name>
              <contact xmlns="http://www.cdlib.org/inside/diglib/copyrightMD"/>
            </rights.holder>
          </copyright>
        </accessCondition>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
<?php endif; ?>

<?php if ($type_of_resource): ?>
  <!--  If the element exists, copy and change it -->
  <xsl:template match="/mods:mods/mods:typeOfResource">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:text><?php print $type_of_resource; ?></xsl:text>
    </xsl:copy>
  </xsl:template>

  <!--  If the element doesn't exist, add it -->
  <xsl:template match="/mods:mods">
    <xsl:copy>
        <xsl:if test="not(mods:typeOfResource)">
            <typeOfResource xmlns="http://www.loc.gov/mods/v3"><?php print $type_of_resource; ?></typeOfResource>
        </xsl:if>
        <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
<?php endif; ?>

</xsl:stylesheet>