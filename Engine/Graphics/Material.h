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

#include "GraphicsDefs.h"
#include "Resource.h"
#include "Vector4.h"

class Pass;
class Technique;
class Texture;
class Texture2D;
class TextureCube;

/// Material's technique list entry
struct TechniqueEntry
{
    /// Construct with defaults
    TechniqueEntry();
    /// Construct with parameters
    TechniqueEntry(Technique* technique, unsigned qualityLevel, float lodDistance);
    /// Destruct
    ~TechniqueEntry();
    
    /// Technique
    SharedPtr<Technique> technique_;
    /// Quality level
    int qualityLevel_;
    /// LOD distance
    float lodDistance_;
};

/// Describes how to render 3D geometries
class Material : public Resource
{
    OBJECT(Material);
    
public:
    /// Construct
    Material(Context* context);
    /// Destruct
    ~Material();
    /// Register object factory
    static void RegisterObject(Context* context);
    
    /// Load resource. Return true if successful
    virtual bool Load(Deserializer& source);
    /// Save resource. Return true if successful
    virtual bool Save(Serializer& dest);
    
    /// Set number of techniques
    void SetNumTechniques(unsigned num);
    /// Set technique
    void SetTechnique(unsigned index, Technique* technique, unsigned qualityLevel = 0, float lodDistance = 0.0f);
    /// Set vertex shader parameter
    void SetVertexShaderParameter(VSParameter parameter, const Vector4& value);
    /// Set pixel shader parameter
    void SetPixelShaderParameter(PSParameter parameter, const Vector4& value);
    /// Set texture
    void SetTexture(TextureUnit unit, Texture* texture);
    /// Set texture coordinate transform
    void SetUVTransform(const Vector2& offset, float rotation, const Vector2& repeat);
    /// Set texture coordinate transform
    void SetUVTransform(const Vector2& offset, float rotation, float repeat);
    /// Set culling mode
    void SetCullMode(CullMode mode);
    /// Set culling mode for shadows
    void SetShadowCullMode(CullMode mode);
    /// Mark needing shadow casting and occlusion re-evaluation
    void SetDirty();
    /// Reset all shader pointers
    void ReleaseShaders();
    /// Clone material
    SharedPtr<Material> Clone(const String& cloneName = String()) const;
    /// Mark material for auxiliary view rendering
    void MarkForAuxView(unsigned frameNumber);
    
    /// Return number of techniques
    unsigned GetNumTechniques() const { return techniques_.size(); }
    /// Return all techniques
    const std::vector<TechniqueEntry>& GetTechniques() const { return techniques_; }
    /// Return technique entry by index
    const TechniqueEntry& GetTechniqueEntry(unsigned index) const;
    /// Return technique by index
    Technique* GetTechnique(unsigned index) const;
    /// Return pass by technique index and pass type
    Pass* GetPass(unsigned index, PassType pass) const;
    /// Return all textures
    const std::vector<SharedPtr<Texture> >& GetTextures() const { return textures_; }
    /// Return texture by unit
    Texture* GetTexture(TextureUnit unit) const;
    /// Return all vertex shader parameters
    const std::map<VSParameter, Vector4>& GetVertexShaderParameters() const { return vsParameters_; }
    /// Return all pixel shader parameters
    const std::map<PSParameter, Vector4>& GetPixelShaderParameters() const { return psParameters_; }
    /// Return normal culling mode
    CullMode GetCullMode() const { return cullMode_; }
    /// Return culling mode for shadows
    CullMode GetShadowCullMode() const { return shadowCullMode_; }
    /// Return last auxiliary view rendered frame number
    unsigned GetAuxViewFrameNumber() const { return auxViewFrameNumber_; }
    /// Return whether should render occlusion
    bool GetOcclusion() const { return occlusion_; }
    
    /// Return name for texture unit
    static const String& GetTextureUnitName(TextureUnit unit);
    
private:
    /// Re-evaluate occlusion rendering
    void Update();
    
    /// Techniques
    std::vector<TechniqueEntry> techniques_;
    /// Textures
    std::vector<SharedPtr<Texture> > textures_;
    /// Vertex shader parameters
    std::map<VSParameter, Vector4> vsParameters_;
    /// Pixel shader parameters
    std::map<PSParameter, Vector4> psParameters_;
    /// Normal culling mode
    CullMode cullMode_;
    /// Culling mode for shadow rendering
    CullMode shadowCullMode_;
    /// Last auxiliary view rendered frame number
    unsigned auxViewFrameNumber_;
    /// Render occlusion flag
    bool occlusion_;
};
