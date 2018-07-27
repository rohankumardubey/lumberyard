/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#include "StdAfx.h"
#include "XmlArchive.h"
#include "PakFile.h"

//////////////////////////////////////////////////////////////////////////
// CXmlArchive
bool CXmlArchive::Load(const QString& file)
{
    bLoading = true;

    char filename[AZ_MAX_PATH_LEN] = { 0 };
    AZ::IO::FileIOBase::GetInstance()->ResolvePath(file.toUtf8().data(), filename, AZ_MAX_PATH_LEN);

    QFile cFile(filename);
    if (!cFile.open(QFile::ReadOnly))
    {
        CLogFile::FormatLine("Warning: Loading of %s failed", filename);
        return false;
    }
    CArchive ar(&cFile, CArchive::load);

    QString str;
    ar >> str;

    root = XmlHelpers::LoadXmlFromBuffer(str.toUtf8().data(), str.toUtf8().length());
    if (!root)
    {
        // If we didn't extract valid XML, attempt to check the header to see if we're dealing with an improperly serialized archive
        // When deserializing QStrings, we use readStringLength in EditorUtils, which mimics MFC's decoding.
        // In this encoding, the length is first read as an unsigned 8-bit value, if that is 0xFF then the next two bytes are read
        // If the 16 bit uint is 0xFFFF, then the next four bytes are read, etc. up to a final 64 bit value.

        // In 1.09, there was a bug in which we'd serialize out the 32-bit length improperly like so:
        // 0xFF 0xFF 0x00 <4 byte proper length>
        
        // Note that the header could also historically start with  0xFF 0xFF 0xFE to indicate wide strings prior to the length data
        // but we don't have to deal with that here as the broken version of the code never prepended this
        cFile.seek(0);
        quint8 len8;
        ar >> len8;

        quint16 len16;
        ar >> len16;

        // Possible bad header, attempt to read 32 bit length.
        if (len8 == 0xff && len16 == 0xff)
        {
            // This version of operator<< only serialized out UTF8 strings up to 32 bits of length, no need to 64-bit check or do wchar.
            quint32 len32;
            ar >> len32;

            str = QString::fromUtf8(cFile.read(len32));
            root = XmlHelpers::LoadXmlFromBuffer(str.toUtf8().data(), str.toUtf8().length());
        }

        if (!root)
        {
            CLogFile::FormatLine("Warning: Loading of %s failed", filename);
            return false;
        }
    }

    bool loaded;
    try
    {
        loaded = pNamedData->Serialize(ar);
    }
    catch (...)
    {
        loaded = false;
    }

    if (!loaded)
    {
        CLogFile::FormatLine("Error: Can't load xml file: '%s'! File corrupted. Binary file possibly was corrupted by Source Control if it was marked like text format.", filename);
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
void CXmlArchive::Save(const QString& file)
{
    char filename[AZ_MAX_PATH_LEN] = { 0 };
    AZ::IO::FileIOBase::GetInstance()->ResolvePath(file.toUtf8().data(), filename, AZ_MAX_PATH_LEN);

    bLoading = false;
    if (!root)
    {
        return;
    }

    QFile cFile(filename);
    // Open the file for writing, create it if needed
    if (!cFile.open(QFile::WriteOnly))
    {
        CLogFile::FormatLine("Warning: Saving of %s failed", filename);
        return;
    }
    // Create the archive object
    CArchive ar(&cFile, CArchive::store);

    _smart_ptr<IXmlStringData> pXmlStrData = root->getXMLData(5000000);

    // Need convert to QString for CArchive::operator<<
    QString str = pXmlStrData->GetString();
    ar << str;

    pNamedData->Serialize(ar);
}

//////////////////////////////////////////////////////////////////////////
bool CXmlArchive::SaveToPak(const QString& levelPath, CPakFile& pakFile)
{
    _smart_ptr<IXmlStringData> pXmlStrData = root->getXMLData(5000000);

    // Save xml file.
    QString xmlFilename = "Level.editor_xml";
    pakFile.UpdateFile(xmlFilename.toUtf8().data(), (void*)pXmlStrData->GetString(), pXmlStrData->GetStringLength());

    if (pakFile.GetArchive())
    {
        CLogFile::FormatLine("Saving pak file %s", (const char*)pakFile.GetArchive()->GetFullPath());
    }

    pNamedData->Save(pakFile);
    return true;
}

//////////////////////////////////////////////////////////////////////////
bool CXmlArchive::LoadFromPak(const QString& levelPath, CPakFile& pakFile)
{
    // Add a path separator to GetRelativePath because it may or may not return a path with a trailing separator.
    QString xmlFilename = Path::AddSlash(Path::GetRelativePath(levelPath)) + "Level.editor_xml";
    root = XmlHelpers::LoadXmlFromFile(xmlFilename.toUtf8().data());
    if (!root)
    {
        return false;
    }

    return pNamedData->Load(levelPath, pakFile);
}
