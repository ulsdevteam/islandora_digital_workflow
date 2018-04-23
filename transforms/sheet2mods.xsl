<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:copyrightMD="http://www.cdlib.org/inside/diglib/copyrightMD">

<xsl:output method="xml" indent="yes"/>

        <xsl:template match="/sheet">
        <mods:mods>
                <!-- There MUST always be a title in the source XML, so there is no logic here.  Wrap it in the
                     right tags and render the selected `title` value -->
                <mods:titleInfo>
                        <mods:title>
                                <xsl:value-of select="title" />
                        </mods:title>
                </mods:titleInfo>

                <xsl:if test="count(subject_lcsh|subject_location|subject_name) &gt; 0">
                        <mods:subject authority="lcsh">
                                <xsl:for-each select="subject_lcsh">
                                        <mods:topic>
                                                <xsl:value-of select="." />
                                        </mods:topic>
                                </xsl:for-each>
                                <xsl:for-each select="subject_location">
                                        <mods:geographic>
                                                <xsl:value-of select="." />
                                        </mods:geographic>
                                </xsl:for-each>
                                <xsl:for-each select="subject_name">
                                        <mods:name>
                                                <xsl:value-of select="." />
                                        </mods:name>
                                </xsl:for-each>
                        </mods:subject>
                </xsl:if>

                <xsl:if test="count(subject_local) &gt; 0">
                        <mods:subject authority="local">
                                <xsl:for-each select="subject_local">
                                        <xsl:value-of select="." />
                                </xsl:for-each>
                        </mods:subject>
                </xsl:if>

                <xsl:if test="count(subject|scale) &gt; 0">
                        <xsl:for-each select="subject">
                        <!-- split the value into separate nodes if needed -->
                        <xsl:call-template name="split_subjects">
                                <xsl:with-param name="text" select="."/>
                        </xsl:call-template>

                        </xsl:for-each>
                        <xsl:for-each select="scale">
                                <mods:subject>
                                        <mods:cartographics>
                                                <mods:scale>
                                                        <xsl:value-of select="." />
                                                </mods:scale>
                                        </mods:cartographics>
                                </mods:subject>
                        </xsl:for-each>
                </xsl:if>
                <xsl:if test="count(source_collection_id|source_container|source_ownership|source_collection_date|source_citation|source_collection|parent_id|parent_identifier) &gt; 0">
                        <mods:relatedItem type="host">
                                <xsl:for-each select="parent_identifier">
                                        <mods:identifier>
                                                <xsl:value-of select="." />
                                        </mods:identifier>
                                </xsl:for-each>
                                <xsl:for-each select="parent_id">
                                        <mods:identifier>
                                                <xsl:value-of select="." />
                                        </mods:identifier>
                                </xsl:for-each>
                                <xsl:for-each select="source_collection_id">
                                        <mods:identifier>
                                                <xsl:value-of select="." />
                                        </mods:identifier>
                                </xsl:for-each>
                                <xsl:for-each select="source_container">
                                        <mods:note type="container">
                                                <xsl:value-of select="." />
                                        </mods:note>
                                </xsl:for-each>
                                <xsl:for-each select="source_ownership">
                                        <mods:note type="ownership">
                                                <xsl:value-of select="." />
                                        </mods:note>
                                </xsl:for-each>
                                <xsl:for-each select="source_collection_date">
                                        <mods:originInfo>
                                                <mods:dateCreated>
                                                        <xsl:value-of select="." />
                                                </mods:dateCreated>
                                        </mods:originInfo>
                                </xsl:for-each>
                                <xsl:for-each select="source_citation">
                                        <mods:note type="prefercite">
                                                <xsl:value-of select="." />
                                        </mods:note>
                                </xsl:for-each>
                                <xsl:for-each select="source_collection">
                                        <mods:titleInfo>
                                                <mods:title>
                                                        <xsl:value-of select="." />
                                                </mods:title>
                                        </mods:titleInfo>
                                </xsl:for-each>
                        </mods:relatedItem>
                </xsl:if>

                <xsl:if test="count(contributor) &gt; 0">
                        <xsl:for-each select="contributor">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="." />
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">contributor</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(interviewee) &gt; 0">
                        <xsl:for-each select="interviewee">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="." />
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">interviewee</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(interviewer) &gt; 0">
                        <xsl:for-each select="interviewer">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="." />
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">interviewer</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(creator) &gt; 0">
                        <xsl:for-each select="creator">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="." />
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">creator</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(depositor) &gt; 0">
                        <xsl:for-each select="depositor">
                                <mods:name>
                                        <mods:namePart>
                                                <xsl:value-of select="." />
                                        </mods:namePart>
                                        <mods:role>
                                                <mods:roleTerm type="text">depositor</mods:roleTerm>
                                        </mods:role>
                                </mods:name>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(address|gift_of) &gt; 0">
                        <xsl:for-each select="address">
                                <mods:note type="address">
                                        <xsl:value-of select="." />
                                </mods:note>
                        </xsl:for-each>
                        <xsl:for-each select="gift_of">
                                <mods:note type="donor">
                                        <xsl:value-of select="." />
                                </mods:note>
                        </xsl:for-each>
                </xsl:if>

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

                <xsl:if test="count(rights_holder|copyright_status|publication_status) &gt; 0">
                        <mods:accessCondition>
                                <copyrightMD:copyright>
                                        <xsl:attribute name="copyright.status">
                                                <xsl:value-of select="copyright_status" />
                                        </xsl:attribute>
                                        <xsl:attribute name="publication.status">
                                                <xsl:value-of select="publication_status" />
                                        </xsl:attribute>
                                        <xsl:for-each select="rights_holder">
                                                <copyrightMD:rights.holder>
                                                        <copyrightMD:name>
                                                                <xsl:value-of select="." />
                                                        </copyrightMD:name>
                                                </copyrightMD:rights.holder>
                                        </xsl:for-each>
                                </copyrightMD:copyright>
                        </mods:accessCondition>
                </xsl:if>

                <xsl:if test="count(genre) &gt; 0">
                        <mods:genre>
                                <xsl:value-of select="genre" />
                        </mods:genre>
                </xsl:if>

                <xsl:if test="count(identifier) &gt; 0">
                        <mods:identifier type="pitt">
                                <xsl:value-of select="identifier" />
                        </mods:identifier>
                </xsl:if>

                <xsl:if test="count(source_id|source_identifier) &gt; 0">
                        <xsl:for-each select="source_id">
                                <mods:identifier type="source">
                                        <xsl:value-of select="." />
                                </mods:identifier>
                        </xsl:for-each>
                        <xsl:for-each select="source_identifier">
                                <mods:identifier type="source">
                                        <xsl:value-of select="." />
                                </mods:identifier>
                        </xsl:for-each>
                </xsl:if>

                <xsl:if test="count(date_digitized|normalized_date|normalized_date_qualifier|date|sort_date|pub_place|publisher) &gt; 0">
                        <mods:originInfo>
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
                                <xsl:if test="count(date_digitized) &gt; 0">
                                        <mods:dateCaptured>
                                                <xsl:value-of select="date_digitized" />
                                        </mods:dateCaptured>
                                </xsl:if>
                                <xsl:for-each select="date">
                                        <mods:dateOther type="display">
                                                <xsl:value-of select="." />
                                        </mods:dateOther>
                                </xsl:for-each>
                                <xsl:for-each select="sort_date">
                                        <mods:dateOther type="sort">
                                                <xsl:value-of select="." />
                                        </mods:dateOther>
                                </xsl:for-each>
                                <xsl:for-each select="pub_place">
                                        <mods:place>
                                                <mods:placeTerm type="text">
                                                        <xsl:value-of select="." />
                                                </mods:placeTerm>
                                        </mods:place>
                                </xsl:for-each>
                                <xsl:for-each select="publisher">
                                        <mods:publisher>
                                                <xsl:value-of select="." />
                                        </mods:publisher>
                                </xsl:for-each>
                        </mods:originInfo>
                </xsl:if>

                <xsl:if test="count(dimension|format) &gt; 0">
                        <mods:physicalDescription>
                                <xsl:if test="count(dimension) &gt; 0">
                                        <mods:extent>
                                                <xsl:value-of select="dimension" />
                                        </mods:extent>
                                </xsl:if>
                                <xsl:if test="count(format) &gt; 0">
                                        <mods:form>
                                                <xsl:value-of select="format" />
                                        </mods:form>
                                </xsl:if>
                        </mods:physicalDescription>
                </xsl:if>

                <xsl:if test="count(type_of_resource) &gt; 0">
                        <mods:typeOfResource>
                                <xsl:value-of select="type_of_resource" />
                        </mods:typeOfResource>
                </xsl:if>

        </mods:mods>
        </xsl:template>

        <!-- when ";", this means a new subject/topic.  when contains double dash, split into new /topic nodes. -->
        <xsl:template match="text/text()" name="split_subjects">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="';'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:subject>
                                        <xsl:call-template name="split_topics">
                                                <xsl:with-param name="text" select="normalize-space($text)"/>
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

        <!-- split the topics, when passed a value that contains a double-dash, split into new topic nodes -->
        <xsl:template match="text/text()" name="split_topics">
                <xsl:param name="text" select="."/>
                <xsl:param name="separator" select="'--'"/>
                <xsl:choose>
                        <xsl:when test="not(contains($text, $separator))">
                                <mods:topic>
                                        <xsl:value-of select="normalize-space($text)"/>
                                </mods:topic>
                        </xsl:when>
                        <xsl:otherwise>
                                <mods:topic>
                                    <xsl:value-of select="normalize-space(substring-before($text, $separator))"/>
                                </mods:topic>
                                <xsl:call-template name="split_topics">
                                        <xsl:with-param name="text" select="substring-after($text, $separator)"/>
                                </xsl:call-template>
                        </xsl:otherwise>
                </xsl:choose>
        </xsl:template>

</xsl:stylesheet>