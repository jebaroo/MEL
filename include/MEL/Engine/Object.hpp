// MIT License
//
// MEL - Mechatronics Engine and Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_OBJECT_HPP
#define MEL_OBJECT_HPP

#include <string>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <MEL/Core/Time.hpp>

namespace mel {

class Engine;
class Component;

class Object {
public:

    /// Constructs a child Object
    Object(const std::string& name, Object* parent = nullptr);

    /// Virtual destructor
    virtual ~Object();

    /// Adds a Component to the Object
    template <typename T, typename ... Args>
    void add_component(Args ... args) {
        add_component(new T(args...), std::type_index(typeid(T)));
    }

    /// Gets a Component attached to the Object
    template <typename T>
    T* get_component() {
        return static_cast<T*>(get_component(std::type_index(typeid(T))));
    }

    /// Adds a child Object to this Object
    void add_child(Object* child);

    /// Removes a child Object from this Object
    void remove_child(Object* child_object);

    /// Removes a child Object from this Object by name
    void remove_child(const std::string& child_name);

    /// Removes all child Objects from tis Object
    void remove_all_children();

    /// Returns a child Object from this Object by name
    Object* get_child(const std::string& child_name);

    /// Returns a child T Object from this Object by name
    template <typename T>
    T* get_child(const std::string& name) {
        return dynamic_cast<T*>(get_child(name));
    }

    /// Returns the first T Object from this Object
    template <typename T>
    T* get_child() {
        for (std::size_t i = 0; i < children_.size(); ++i) {
            T* object = dynamic_cast<T*>(children_[i]);
            if (object)
                return object;
        }
        return nullptr;
    }

public:

    const std::string name; ///< the name of this Object

private:

    friend class Engine;

    /// Adds a Component to the Object (internal)
    void add_component(Component* component, std::type_index type);

    /// Gets a Component attached to the Object (internal)
    Component* get_component(std::type_index type);

    /// Recursively sets the Engine of this Object and all child Objects
    void set_engine(Engine* engine);

    /// Recursively starts all Components and child Objects
    void start_all();

    /// Recursively updates all Components and child Objects
    void update_all();

    /// Recursively late updates all Components and child Objects
    void late_update_all();

    /// Recursively stops all Components and child Objects
    void stop_all();

private:

    Engine* engine_;  ///< pointer to Engine this Object runs on
    Object* parent_;  ///< pointer to parent Oject of this Object

    /// Children Objects of this Object
    std::vector<Object*> children_;
    /// Map of childlren names to children_ indices
    std::unordered_map<std::string, std::size_t> children_map_;

    /// Componets attached to this Object
    std::vector<Component*> components_;
    /// Map of Component types to components_ indices
    std::unordered_map<std::type_index, std::size_t> components_map_;

};

}  // namespace mel

#endif  // MEL_OBJECT_HPP