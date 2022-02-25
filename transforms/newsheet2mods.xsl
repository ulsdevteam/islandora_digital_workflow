<?xml version="1.0" encoding="ISO-8859-1"?><xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:copyrightMD="http://www.cdlib.org/inside/diglib/copyrightMD" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.loc.gov/mods/v3 http://www.loc.gov/standards/mods/v3/mods-3-7.xsd">

<xsl:output method="xml" indent="yes"/>

        <xsl:template match="/sheet">
        <mods:mods>
		
                <!-- MODS:TITLE -->

                <!-- There MUST always be a title in the source XML, so there is no logic here.  Wrap it in the
                     right tags and render the selected `title` value -->
                <mods:titleInfo>
                        <mods:title>
                                <xsl:value-of select="title" />
                        </mods:title>
                </mods:titleInfo>
		
                <!-- MODS:ABSTRACT -->

		<xsl:if test="count(abstract) &gt; 0">
                        <mods:abstract>
                                <xsl:value-of select="abstract" />
                        </mods:abstract>
                </xsl:if>

		<xsl:if test="count(description) &gt; 0">
                        <mods:abstract>
                                <xsl:value-of select="description" />
                        </mods:abstract>
                </xsl:if>

                <!-- MODS:ACCESS CONDITION -->

                <xsl:if test="count(rights_holder|copyright_status|publication_status) &gt; 0">
                        <mods:accessCondition>
                                <copyrightMD:copyright>
                                        <xsl:attribute name="copyright.status">
                                                <xsl:value-of select="copyright_status" />
                                        </xsl:attribute>
                                        <xsl:attribute name="publication.status">
                                                <xsl:value-of select="publication_status" />
                                        </xsl:attribute>
                                        <xsl:if test="count(rights_holder) &gt; 0">
                                                <xsl:for-each select="rights_holder">
                                                        <xsl:call-template name="split_rights_holder">
                                                                <xsl:with-param name="text" select="."/>
                                                        </xsl:call-template>
                                                </xsl:for-each>
                                        </xsl:if>
                                </copyrightMD:copyright>
                        </mods:accessCondition>
                </xsl:if>

                <!-- MODS:GENRE -->

                <xsl:if test="count(genre) &gt; 0">
                        <xsl:for-each select="genre">
                                <xsl:call-template name="split_genre">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <!-- MODS:IDENTIFIER -->

                <xsl:if test="count(identifier) &gt; 0">
                        <mods:identifier type="pitt">
                                <xsl:value-of select="identifier" />
                        </mods:identifier>
                </xsl:if>

                <xsl:if test="count(source_id|source_identifier) &gt; 0">
                        <xsl:for-each select="source_id">
                                <xsl:call-template name="split_identifier">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                        <xsl:for-each select="source_identifier">
                                <xsl:call-template name="split_identifier">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <!-- MODS:LANGUAGE -->

                <xsl:if test="count(language) &gt; 0">
                        <xsl:for-each select="language">
                                <xsl:call-template name="split_language">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <!-- MODS:NAME -->

                <xsl:if test="count(contributor) &gt; 0">
                        <xsl:for-each select="contributor">
                                <xsl:call-template name="split_contributor">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(creator) &gt; 0">
                        <xsl:for-each select="creator">
                                <xsl:call-template name="split_creator">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(depositor) &gt; 0">
                        <xsl:for-each select="depositor">
                                <xsl:call-template name="split_depositor">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(interviewee) &gt; 0">
                        <xsl:for-each select="interviewee">
                                <xsl:call-template name="split_interviewee">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(interviewer) &gt; 0">
                        <xsl:for-each select="interviewer">
                                <xsl:call-template name="split_interviewer">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <!-- MODS:NOTE -->

                <xsl:if test="count(address|gift_of) &gt; 0">
                        <xsl:for-each select="address">
                                <xsl:call-template name="split_address">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                        <xsl:for-each select="gift_of">
                                <xsl:call-template name="split_gift_of">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
                        </xsl:for-each>
                </xsl:if>

                <!-- MODS:ORIGIN INFORMATION -->

                <xsl:if test="count(date_digitized|normalized_date|normalized_date_qualifier|date|display_date|sort_date|pub_place|publisher) &gt; 0">
                        <mods:originInfo>
                                <xsl:if test="count(date_digitized) &gt; 0">
                                        <mods:dateCaptured>
                                                <xsl:value-of select="date_digitized" />
                                        </mods:dateCaptured>
                                </xsl:if>
                                <xsl:if test="count(normalized_date|normalized_date_qualifier) &gt; 0">
                                        <mods:dateCreated>
                                                <xsl:attribute name="encoding">iso8601</xsl:attribute>
                                                <xsl:attribute name="keyDate">yes</xsl:attribute>
                                                <xsl:if test="count(normalized_date_qualifier) &gt; 0">
                                                        <xsl:attribute name="qualifier">approximate</xsl:attribute>
                                                </xsl:if>
                                                <xsl:value-of select="normalized_date" />
                                        </mods:dateCreated>
                                </xsl:if>
                                <xsl:for-each select="date">
                                        <xsl:call-template name="split_date">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="display_date">
                                        <xsl:call-template name="split_display_date">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="sort_date">
                                        <mods:dateOther type="sort">
                                                <xsl:value-of select="." />
                                        </mods:dateOther>
                                </xsl:for-each>
                                <xsl:for-each select="publisher">
                                        <xsl:call-template name="split_publisher">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="pub_place">
                                        <xsl:call-template name="split_pub_place">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                        </mods:originInfo>
                </xsl:if>

                <!-- MODS:PHYSICAL DESCRIPTION -->

                <xsl:if test="count(dimension|format|extent) &gt; 0">
                        <mods:physicalDescription>
                                <xsl:if test="count(dimension) &gt; 0">
                                        <xsl:for-each select="dimension">
                                                <xsl:call-template name="split_dimension">
                                                        <xsl:with-param name="text" select="."/>
                                                </xsl:call-template>
                                        </xsl:for-each>
                                </xsl:if>
                                <xsl:if test="count(format) &gt; 0">
                                        <xsl:for-each select="format">
                                                <xsl:call-template name="split_format">
                                                        <xsl:with-param name="text" select="."/>
                                                </xsl:call-template>
                                        </xsl:for-each>
                                </xsl:if>
                                <xsl:if test="count(extent) &gt; 0">
                                        <xsl:for-each select="extent">
                                                <xsl:call-template name="split_extent">
                                                        <xsl:with-param name="text" select="."/>
                                                </xsl:call-template>
                                        </xsl:for-each>
                                </xsl:if>
                        </mods:physicalDescription>
                </xsl:if>

                <!-- MODS:RELATED ITEM -->

                <xsl:if test="count(source_collection_id|source_container|source_ownership|source_collection_date|source_collection|source_citation|source_other_level|parent_id|parent_identifier|source_series|source_subseries) &gt; 0">
                        <mods:relatedItem>
                                <xsl:for-each select="source_citation">
                                        <mods:note type="prefercite">
                                                <xsl:value-of select="." />
                                        </mods:note>
                                </xsl:for-each>
                        </mods:relatedItem> 
		        <mods:relatedItem type="host">
				 <xsl:for-each select="source_collection">
                                        <mods:titleInfo>
                                                <mods:title>
                                                        <xsl:value-of select="." />
                                                </mods:title>
                                        </mods:titleInfo>
				</xsl:for-each>
                                <xsl:for-each select="source_collection_id">
                                        <xsl:call-template name="split_source_collection_id">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
			        <xsl:for-each select="source_container">
                                        <xsl:call-template name="split_source_container">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="source_ownership">
                                        <xsl:call-template name="split_source_ownership">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="source_collection_date">
                                        <xsl:call-template name="split_source_collection_date">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="source_other_level">
                                        <xsl:call-template name="split_source_other_level">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="parent_id">
                                        <xsl:call-template name="split_source_parent_id">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="parent_identifier">
                                        <xsl:call-template name="split_source_parent_identifier">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="source_series">
                                        <mods:note type="series">
                                                <xsl:value-of select="." />
                                        </mods:note>
                                </xsl:for-each>
                                <xsl:for-each select="source_subseries">
                                        <mods:note type="subseries">
                                                <xsl:value-of select="." />
                                        </mods:note>
                                </xsl:for-each>
                        </mods:relatedItem>
                </xsl:if>

                <!-- MODS:SUBJECT -->

                <xsl:if test="count(subject_geographic|subject_location|subject_temporal|subject_topic|subject_lcsh|subject_name|subject_topic_local|subject_local|subject|scale) &gt; 0">
				<xsl:for-each select="subject_geographic">
                                        <xsl:call-template name="split_subject_geographic">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_location">
                                        <xsl:call-template name="split_subject_location">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_temporal">
                                        <xsl:call-template name="split_subject_temporal">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_topic">
                                        <xsl:call-template name="split_subject_topic">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_lcsh">
                                        <xsl:call-template name="split_subject_lcsh">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_name">
                                        <xsl:call-template name="split_subject_name">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_topic_local">
                                        <xsl:call-template name="split_subject_topic_local">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
                                <xsl:for-each select="subject_local">
                                        <xsl:call-template name="split_subject_local">
                                                <xsl:with-param name="text" select="."/>
                                        </xsl:call-template>
                                </xsl:for-each>
				<xsl:for-each select="subject">
                                	<xsl:call-template name="split_subjects">
                                        	<xsl:with-param name="text" select="."/>
                                	</xsl:call-template>
                        	</xsl:for-each>
                        	<xsl:for-each select="scale">
                                	<xsl:call-template name="split_scale">
                                        	<xsl:with-param name="text" select="."/>
                                	</xsl:call-template>
                        	</xsl:for-each>
                </xsl:if>

                <!-- MODS:TYPE OF RESOURCE -->

                <xsl:if test="count(type_of_resource) &gt; 0">
                        <xsl:for-each select="type_of_resource">
                        	<xsl:call-template name="split_type_of_resource">
                                        <xsl:with-param name="text" select="."/>
                                </xsl:call-template>
			</xsl:for-each>
                </xsl:if>

        </mods:mods>
        </xsl:template>


        <!-- SPLIT CHARACTERS FOR REPEATABLE FIELDS -->

        <!-- ACCESS CONDITION -->

        <xsl:template match="text/text()" name="split_rights_holder">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <copyrightMD:rights.holder>
                                        <copyrightMD:name>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </copyrightMD:name>
                                </copyrightMD:rights.holder>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_rights_holder">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_rights_holder">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>
        
	<!-- GENRE -->

        <xsl:template match="text/text()" name="split_genre">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:genre>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:genre>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_genre">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_genre">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <!-- IDENTIFIER -->

        <xsl:template match="text/text()" name="split_identifier">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:identifier type="source">
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:identifier>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_identifier">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_identifier">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <!-- LANGUAGE -->

        <xsl:template match="text/text()" name="split_language">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:language>
                                        <mods:languageTerm type="code" authority="iso639-2b">
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:languageTerm>
                                </mods:language>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_language">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_language">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <!-- NAME -->

        <xsl:template match="text/text()" name="split_contributor">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:name>
                                        <mods:namePart>
						<xsl:value-of select="normalize-space($text)"/>
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">contributor</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_contributor">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_contributor">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_creator">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:name>
                                        <mods:namePart>
						<xsl:value-of select="normalize-space($text)"/>
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">creator</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_creator">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_creator">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_depositor">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">depositor</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_depositor">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_depositor">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_interviewee">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">interviewee</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_interviewee">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_interviewee">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_interviewer">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">interviewer</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_interviewer">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_interviewer">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <!-- NOTE -->

        <xsl:template match="text/text()" name="split_address">
         <xsl:param name="text" select="."/>
         <xsl:param name="separator" select="';'"/>
                 <xsl:choose>
                         <xsl:when test="not(contains($text, $separator))">
                                 <mods:note type="address">
                                         <xsl:value-of select="normalize-space($text)"/>
                                 </mods:note>
                         </xsl:when>
                         <xsl:otherwise>
                                 <xsl:call-template name="split_address">
                                         <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                 </xsl:call-template>
                                 <xsl:call-template name="split_address">
                                         <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                 </xsl:call-template>
                         </xsl:otherwise>
                 </xsl:choose>
         </xsl:template>

         <xsl:template match="text/text()" name="split_gift_of">
         <xsl:param name="text" select="."/>
         <xsl:param name="separator" select="';'"/>
                 <xsl:choose>
                         <xsl:when test="not(contains($text, $separator))">
                                 <mods:note type="donor">
                                         <xsl:value-of select="normalize-space($text)"/>
                                 </mods:note>
                         </xsl:when>
                         <xsl:otherwise>
                                 <xsl:call-template name="split_gift_of">
                                         <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                 </xsl:call-template>
                                 <xsl:call-template name="split_gift_of">
                                         <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                 </xsl:call-template>
                         </xsl:otherwise>
                 </xsl:choose>
         </xsl:template>

         <!-- ORIGIN INFORMATION -->

         <xsl:template match="text/text()" name="split_date">
         <xsl:param name="text" select="."/>
         <xsl:param name="separator" select="';'"/>
                 <xsl:choose>
                         <xsl:when test="not(contains($text, $separator))">
                                <mods:dateOther type="display">
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:dateOther>
                         </xsl:when>
                         <xsl:otherwise>
                                 <xsl:call-template name="split_date">
                                         <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                 </xsl:call-template>
                                 <xsl:call-template name="split_date">
                                         <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                 </xsl:call-template>
                         </xsl:otherwise>
                 </xsl:choose>
         </xsl:template>

         <xsl:template match="text/text()" name="split_display_date">
         <xsl:param name="text" select="."/>
         <xsl:param name="separator" select="';'"/>
                 <xsl:choose>
                         <xsl:when test="not(contains($text, $separator))">
                                <mods:dateOther type="display">
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:dateOther>
                         </xsl:when>
                         <xsl:otherwise>
                                 <xsl:call-template name="split_display_date">
                                         <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                 </xsl:call-template>
                                 <xsl:call-template name="split_display_date">
                                         <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                 </xsl:call-template>
                         </xsl:otherwise>
                 </xsl:choose>
         </xsl:template>

         <xsl:template match="text/text()" name="split_publisher">
         <xsl:param name="text" select="."/>
         <xsl:param name="separator" select="';'"/>
                 <xsl:choose>
                         <xsl:when test="not(contains($text, $separator))">
                                <mods:publisher>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:publisher>
                         </xsl:when>
                         <xsl:otherwise>
                                 <xsl:call-template name="split_publisher">
                                         <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                 </xsl:call-template>
                                 <xsl:call-template name="split_publisher">
                                         <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                 </xsl:call-template>
                         </xsl:otherwise>
                 </xsl:choose>
         </xsl:template>

         <xsl:template match="text/text()" name="split_pub_place">
         <xsl:param name="text" select="."/>
         <xsl:param name="separator" select="';'"/>
                 <xsl:choose>
                         <xsl:when test="not(contains($text, $separator))">
                                <mods:place>
                                        <mods:placeTerm type="text">
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:placeTerm>
                                </mods:place>
                         </xsl:when>
                         <xsl:otherwise>
                                 <xsl:call-template name="split_pub_place">
                                         <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                 </xsl:call-template>
                                 <xsl:call-template name="split_pub_place">
                                         <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                 </xsl:call-template>
                         </xsl:otherwise>
                 </xsl:choose>
         </xsl:template>

        <!-- PHYSICAL DESCRIPTION -->

        <xsl:template match="text/text()" name="split_dimension">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:extent>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:extent>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_dimension">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_dimension">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_format">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:form>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:form>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_format">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_format">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_extent">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:extent>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:extent>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_extent">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_extent">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <!-- RELATED ITEM -->

        <xsl:template match="text/text()" name="split_source_collection_id">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:identifier>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:identifier>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_collection_id">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_collection_id">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_source_container">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:note type="container">
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:note>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_container">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_container">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_source_ownership">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:note type="ownership">
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:note>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_ownership">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_ownership">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_source_collection_date">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:originInfo>
                                                <mods:dateCreated>
                                                        <xsl:value-of select="normalize-space($text)"/>
                                                </mods:dateCreated>
                                        </mods:originInfo>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_collection_date">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_collection_date">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_source_other_level">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:note type="otherlevel">
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:note>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_other_level">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_other_level">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_source_parent_id">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:identifier>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:identifier>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_parent_id">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_parent_id">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_source_parent_identifier">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                        <mods:identifier>
                                                <xsl:value-of select="normalize-space($text)"/>
                                        </mods:identifier>
                        </xsl:when>
                        <xsl:otherwise>
                                <xsl:call-template name="split_source_parent_identifier">
                                        <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                                </xsl:call-template>
                                <xsl:call-template name="split_source_parent_identifier">
                                        <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

        <!-- SUBJECT -->

        <xsl:template match="text/text()" name="split_subject_geographic">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject authority="lcsh">
				<xsl:call-template name="split_subject_geographic_dash">
					<xsl:with-param name="text" select="$text"/>
				</xsl:call-template>
			</mods:subject>
		</xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_geographic">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_geographic">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

	<xsl:template match="text/text()" name="split_subject_geographic_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
              		<mods:geographic>
                       		<xsl:value-of select="normalize-space($text)"/>
                        </mods:geographic>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_geographic_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_geographic_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_location">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject authority="lcsh">
			        <xsl:call-template name="split_subject_location_dash">
                                        <xsl:with-param name="text" select="$text"/>
                                </xsl:call-template>
			</mods:subject>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_location">
                		<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_location">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

	<xsl:template match="text/text()" name="split_subject_location_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
                	<mods:geographic>
                        	<xsl:value-of select="normalize-space($text)"/>
                        </mods:geographic>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_location_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_location_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
               	</xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_temporal">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject authority="lcsh">
                        	<xsl:call-template name="split_subject_temporal_dash">
                        		<xsl:with-param name="text" select="$text"/>
                        	</xsl:call-template>
			</mods:subject>
		</xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_temporal">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_temporal">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_temporal_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
                	<mods:temporal>
                        	<xsl:value-of select="normalize-space($text)"/>
                        </mods:temporal>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_temporal_dash">
  				<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_temporal_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_topic">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject authority="lcsh">
                        	<xsl:call-template name="split_subject_topic_dash">
                                	<xsl:with-param name="text" select="$text"/>
                                </xsl:call-template>
                        </mods:subject>
		</xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_topic">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_topic">
                		<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
	</xsl:choose>
        </xsl:template>

	<xsl:template match="text/text()" name="split_subject_topic_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
                	<mods:topic>
                        	<xsl:value-of select="normalize-space($text)"/>
                        </mods:topic>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_topic_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_topic_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_lcsh">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject authority="lcsh">
				<xsl:call-template name="split_subject_lcsh_dash">
					<xsl:with-param name="text" select="$text"/>
				</xsl:call-template>
			</mods:subject>
		</xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_lcsh">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_lcsh">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
	</xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_lcsh_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
        		<mods:topic>
                        	<xsl:value-of select="normalize-space($text)"/>
                        </mods:topic>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_lcsh_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_lcsh_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_name">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
                	<mods:subject authority="lcsh">
                        	<xsl:call-template name="split_subject_name_dash">
                                	<xsl:with-param name="text" select="$text"/>
                                </xsl:call-template>
                        </mods:subject>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_name">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_name">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_name_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
                	<mods:name>
                        	<mods:namePart>
                                	<xsl:value-of select="normalize-space($text)"/>
                                </mods:namePart>
                        </mods:name>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_name_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_name_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_topic_local">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject authority="local">
				<xsl:call-template name="split_subject_topic_local_dash">
                                	<xsl:with-param name="text" select="$text"/>
                                </xsl:call-template>
			</mods:subject>
		</xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_topic_local">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                       	<xsl:call-template name="split_subject_topic_local">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
		</xsl:otherwise>
        </xsl:choose>
        </xsl:template>

	<xsl:template match="text/text()" name="split_subject_topic_local_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
			<mods:topic>
				<xsl:value-of select="normalize-space($text)"/>
			</mods:topic>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_topic_local_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_topic_local_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_local">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
		<xsl:when test="not(contains($text, $separator))">
                	<mods:subject authority="local">
                        	<xsl:call-template name="split_subject_local_dash">
                                	<xsl:with-param name="text" select="$text"/>
                                </xsl:call-template>
                        </mods:subject>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_local">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                	</xsl:call-template>
                        <xsl:call-template name="split_subject_local">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subject_local_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
                	<mods:topic>
                        	<xsl:value-of select="normalize-space($text)"/>
                        </mods:topic>
                </xsl:when>
                <xsl:otherwise>
                	<xsl:call-template name="split_subject_local_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subject_local_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_subjects">
       	<xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator))">
			<mods:subject>
		        	<xsl:call-template name="split_subjects_dash">
                        		<xsl:with-param name="text" select="$text"/>
                        	</xsl:call-template>
			</mods:subject>
		</xsl:when>
		<xsl:otherwise>
                	<xsl:call-template name="split_subjects">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subjects">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

	<xsl:template match="text/text()" name="split_subjects_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
			<mods:topic>
                        	<xsl:value-of select="normalize-space($text)"/>
                        </mods:topic>
                </xsl:when>
                <xsl:otherwise>
	                <xsl:call-template name="split_subjects_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_subjects_dash">
                        	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

        <xsl:template match="text/text()" name="split_scale">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
	<xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
                <xsl:when test="not(contains($text, $separator))">
                	<mods:subject>
			        <xsl:call-template name="split_scale_dash">
                                        <xsl:with-param name="text" select="$text"/>
                                </xsl:call-template>
			</mods:subject>
		</xsl:when>
                <xsl:otherwise>
                        <xsl:call-template name="split_scale">
                                <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_scale">
                                <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>
        
        <xsl:template match="text/text()" name="split_scale_dash">
        <xsl:param name="text" select="."/>
        <xsl:param name="separator" select="';'"/>
        <xsl:param name="separator2" select="'--'"/>
        <xsl:choose>
        	<xsl:when test="not(contains($text, $separator2))">
                	<mods:cartographics>
                		<mods:scale>
                                	<xsl:value-of select="normalize-space($text)"/>
                                </mods:scale>
                        </mods:cartographics>
                </xsl:when>
                <xsl:otherwise>
                        <xsl:call-template name="split_scale_dash">
                                <xsl:with-param name="text" select="normalize-space(substring-before($text, $separator2))"/>
                        </xsl:call-template>
                        <xsl:call-template name="split_scale_dash">
                                <xsl:with-param name="text" select="normalize-space(substring-after($text, $separator2))"/>
                        </xsl:call-template>
                </xsl:otherwise>
        </xsl:choose>
        </xsl:template>

	<!-- TYPE OF RESOURCE -->

	<xsl:template match="text/text()" name="split_type_of_resource">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:typeOfResource>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:typeOfResource>
                        </xsl:when>
			<xsl:otherwise>
                        	<xsl:call-template name="split_type_of_resource">
                                	<xsl:with-param name="text" select="normalize-space(substring-before($text, $separator))"/>
                        	</xsl:call-template>
                        	<xsl:call-template name="split_type_of_resource">
                                	<xsl:with-param name="text" select="normalize-space(substring-after($text, $separator))"/>
                        	</xsl:call-template>
                	</xsl:otherwise>
                </xsl:choose>
        </xsl:template>

</xsl:stylesheet>

