//
// Urho3D Engine
// Copyright (c) 2008-2011 Lasse ��rni
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Object.h"

/// File entry within the package file
struct PackageEntry
{
    unsigned offset_;
    unsigned size_;
    unsigned checksum_;
};

/// Stores files of a directory tree sequentially for convenient access
class PackageFile : public Object
{
    OBJECT(PackageFile);
    
public:
    /// Construct
    PackageFile(Context* context);
    /// Construct and open
    PackageFile(Context* context, const String& fileName);
    /// Destruct
    virtual ~PackageFile();
    
    /// Open the package file. Return true if successful
    bool Open(const String& fileName);
    /// Check if a file exists within the package file
    bool Exists(const String& fileName) const;
    /// Return the file entry corresponding to the name, or null if not found
    const PackageEntry* GetEntry(const String& fileName) const;
    /// Return all file entries
    const std::map<String, PackageEntry>& GetEntries() const { return entries_; }
    /// Return the package file name
    const String& GetName() const { return fileName_; }
    /// Return hash of the package file name
    StringHash GetNameHash() const { return nameHash_; }
    /// Return number of files
    unsigned GetNumFiles() const { return entries_.size(); }
    /// Return total size of the package file
    unsigned GetTotalSize() const { return totalSize_; }
    /// Return checksum of the package file contents
    unsigned GetChecksum() const { return checksum_; }
    
private:
    /// File entries
    std::map<String, PackageEntry> entries_;
    /// File name
    String fileName_;
    /// Package file name hash
    StringHash nameHash_;
    /// Package file total size
    unsigned totalSize_;
    /// Package file checksum
    unsigned checksum_;
};
