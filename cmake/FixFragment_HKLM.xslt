<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:xs="http://www.w3.org/2001/XMLSchema"
        xmlns:fn="http://www.w3.org/2005/xpath-functions"
   xmlns:w="http://schemas.microsoft.com/wix/2006/wi">

    <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes" />

    <!-- Identity template -->
    <xsl:template match="@* | node()">
        <xsl:copy>
            <xsl:apply-templates select="@* | node()" />
        </xsl:copy>
    </xsl:template>

    <xsl:template match="/w:Wix" >
        <w:Wix>
            <xsl:apply-templates select="w:Fragment" />
        </w:Wix>
    </xsl:template>

    <xsl:template match="w:Fragment" >
        <w:Fragment>
            <xsl:apply-templates select="w:DirectoryRef" />
            <xsl:apply-templates select="w:ComponentGroup"/>
        </w:Fragment>
    </xsl:template>

    <xsl:template match="/w:Wix/w:Fragment/w:DirectoryRef">
        <w:DirectoryRef Id="{@Id}" />
    </xsl:template>

    <xsl:template match="w:Component">
        <w:Component Id="{@Id}" Guid="{@Guid}" Directory="{@Directory}">
            <xsl:apply-templates select="w:File" />
            <xsl:apply-templates select="w:ProgId" />
            <xsl:apply-templates select="w:RegistryValue" />
        </w:Component>
    </xsl:template>

    <xsl:template match="w:TypeLib">
        <w:TypeLib Id="{@Id}" Description="{@Description}" HelpDirectory="{@HelpDirectory}" Language="{@Language}" MajorVersion="{@MajorVersion}" MinorVersion="{@MinorVersion}">
            <xsl:apply-templates select="*" />
        </w:TypeLib>
    </xsl:template>

    <xsl:template match="w:File">
        <w:File Id="{@Id}" Source="{@Source}">
            <xsl:apply-templates select="w:TypeLib" />
        </w:File>
    </xsl:template>

    <xsl:template match="w:RegistryValue">
        <w:RegistryValue Key="{@Key}" Value="{@Value}" Type="{@Type}" Action="{@Action}">
            <xsl:if test="@Name != ''"> <xsl:attribute name="Name"> <xsl:value-of select="@Name" /> </xsl:attribute> </xsl:if>
            <xsl:if test="@Id != ''"> <xsl:attribute name="Id"> <xsl:value-of select="@Id" /> </xsl:attribute> </xsl:if>
            <xsl:choose>
                <xsl:when test="@Root='HKCU'">
                    <xsl:attribute name="Root">HKLM</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="Root"><xsl:value-of select="@Root" /></xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:if test="@Name = 'Path'">
                <xsl:attribute name="KeyPath">yes</xsl:attribute>
            </xsl:if>
        </w:RegistryValue>
    </xsl:template>

    <xsl:template match="w:ComponentGroup">
        <w:ComponentGroup Id="{@Id}">
            <xsl:apply-templates select="w:Component"/>
            <!-- Intersection in XSLT 1.0 (Michael Kay) -->
        </w:ComponentGroup>
    </xsl:template>

    <xsl:template match="w:ComponentRef">
        <w:ComponentRef Id="{@Id}"/>
    </xsl:template>

</xsl:stylesheet>
