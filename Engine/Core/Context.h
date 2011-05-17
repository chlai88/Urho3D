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

#include "Attribute.h"
#include "Object.h"

#include <map>
#include <set>
#include <vector>

/// Execution context within a process. Provides access to the subsystems, object factories and attributes, and event receivers
class Context : public RefCounted
{
public:
    /// Construct and create the Time & Profiler subsystems
    Context();
    /// Destruct
    ~Context();
    
    /// Create an object by type. Return pointer to it or null if no factory found
    SharedPtr<Object> CreateObject(ShortStringHash objectType);
    /// Register a factory for an object type. If exists already, will not be replaced
    void RegisterFactory(ObjectFactory* factory);
    /// Register a subsystem. If exists already, will not be replaced
    void RegisterSubsystem(Object* subsystem);
    /// Register object attribute
    void RegisterAttribute(ShortStringHash objectType, const AttributeInfo& attr);
    /// Remove object attribute
    void RemoveAttribute(ShortStringHash objectType, const String& name);
    /// Copy base class attributes to derived class
    void CopyBaseAttributes(ShortStringHash baseType, ShortStringHash derivedType);
    /// Add event receiver
    void AddEventReceiver(Object* receiver, StringHash eventType);
    /// Add event receiver for specific event
    void AddEventReceiver(Object* receiver, Object* sender, StringHash eventType);
    /// Remove an event sender from all receivers. Called on its destruction
    void RemoveEventSender(Object* sender);
    /// Remove event receiver from specific events
    void RemoveEventReceiver(Object* receiver, Object* sender, StringHash eventType);
    /// Remove event receiver from non-specific events
    void RemoveEventReceiver(Object* receiver, StringHash eventType);
    
    /// Set current event handler. Called by Object
    void SetEventHandler(EventHandler* handler)
    {
        handler_ = handler;
    }
    
    /// Begin event send
    void BeginSendEvent(Object* sender)
    {
        senders_.push_back(sender);
    }
    
    /// End event send. Clean up event receivers removed in the meanwhile
    void EndSendEvent()
    {
        senders_.pop_back();
    
        // Clean up dirtied event receiver groups when event handling finishes
        if (senders_.empty())
        {
            if (!dirtySpecificReceivers_.empty())
            {
                for (std::set<std::pair<Object*, StringHash> >::iterator i = dirtySpecificReceivers_.begin();
                    i != dirtySpecificReceivers_.end(); ++i)
                {
                    std::vector<Object*>& receivers = specificReceivers_[*i];
                    for (std::vector<Object*>::iterator j = receivers.begin(); j != receivers.end();)
                    {
                        if (*j == 0)
                            j = receivers.erase(j);
                        else
                            ++j;
                    }
                }
                dirtySpecificReceivers_.clear();
            }
            
            if (!dirtyReceivers_.empty())
            {
                for (std::set<StringHash>::iterator i = dirtyReceivers_.begin(); i != dirtyReceivers_.end(); ++i)
                {
                    std::vector<Object*>& receivers = receivers_[*i];
                    for (std::vector<Object*>::iterator j = receivers.begin(); j != receivers.end();)
                    {
                        if (*j == 0)
                            j = receivers.erase(j);
                        else
                            ++j;
                    }
                }
                dirtyReceivers_.clear();
            }
        }
    }
    
    /// Template version of registering an object factory
    template <class T> void RegisterFactory();
    /// Template version of registering an object attribute
    template <class T> void RegisterAttribute(const AttributeInfo& attr);
    /// Template version of removing an object attribue
    template <class T> void RemoveAttribute(const char* name);
    /// Template version of copying base class attributes to derived class
    template <class T, class U> void CopyBaseAttributes();
    
    /// Return subsystem by type
    Object* GetSubsystem(ShortStringHash type) const;
    /// Return all subsystems
    const std::map<ShortStringHash, SharedPtr<Object> >& GetSubsystems() const { return subsystems_; }
    /// Return all object factories
    const std::map<ShortStringHash, SharedPtr<ObjectFactory> >& GetObjectFactories() const { return factories_; }
    /// Return attributes for all object types
    const std::map<ShortStringHash, std::vector<AttributeInfo> >& GetAllAttributes() const { return attributes_; }
    /// Return active event sender. Null outside event handling
    Object* GetSender() const;
    /// Return active event handler. Set by Object. Null outside event handling
    EventHandler* GetHandler() const { return handler_; }
    /// Return object type name from hash, or empty if unknown
    const String& GetTypeName(ShortStringHash type) const;
    /// Template version of returning a subsystem
    template <class T> T* GetSubsystem() const;
    
    /// Return attribute descriptions for an object type, or null if none defined
    const std::vector<AttributeInfo>* GetAttributes(ShortStringHash type) const
    {
        std::map<ShortStringHash, std::vector<AttributeInfo> >::const_iterator i = attributes_.find(type);
        return (i != attributes_.end()) ? &i->second : 0;
    }
    
    /// Return event receivers for a sender and event type, or null if they do not exist
    std::vector<Object*>* GetReceivers(Object* sender, StringHash eventType)
    {
        std::map<std::pair<Object*, StringHash>, std::vector<Object*> >::iterator i = 
            specificReceivers_.find(std::make_pair(sender, eventType));
        return (i != specificReceivers_.end()) ? &i->second : 0;
    }
    
    /// Return event receivers for an event type, or null if they do not exist
    std::vector<Object*>* GetReceivers(StringHash eventType)
    {
        std::map<StringHash, std::vector<Object*> >::iterator i = receivers_.find(eventType);
        return (i != receivers_.end()) ? &i->second : 0;
    }
    
private:
    /// Object factories
    std::map<ShortStringHash, SharedPtr<ObjectFactory> > factories_;
    /// Subsystems
    std::map<ShortStringHash, SharedPtr<Object> > subsystems_;
    /// Attribute descriptions per object type
    std::map<ShortStringHash, std::vector<AttributeInfo> > attributes_;
    /// Event receivers for non-specific events
    std::map<StringHash, std::vector<Object*> > receivers_;
    /// Event receivers for specific senders' events
    std::map<std::pair<Object*, StringHash>, std::vector<Object*> > specificReceivers_;
    /// Event sender stack
    std::vector<Object*> senders_;
    /// Event types that have had receivers removed during event handling
    std::set<StringHash> dirtyReceivers_;
    /// Event types for specific senders that have had receivers removed during event handling
    std::set<std::pair<Object*, StringHash> > dirtySpecificReceivers_;
    /// Active event handler. Not stored in a stack for performance reasons; is needed only in esoteric cases
    WeakPtr<EventHandler> handler_;
};

template <class T> void Context::RegisterFactory()
{
    RegisterFactory(new ObjectFactoryImpl<T>(this));
}

template <class T> void Context::RegisterAttribute(const AttributeInfo& attr)
{
    RegisterAttribute(T::GetTypeStatic(), attr);
}

template <class T> void Context::RemoveAttribute(const char* name)
{
    RemoveAttribute(T::GetTypeStatic(), name);
}

template <class T, class U> void Context::CopyBaseAttributes()
{
    CopyBaseAttributes(T::GetTypeStatic(), U::GetTypeStatic());
}

template <class T> T* Context::GetSubsystem() const
{
    return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
}
