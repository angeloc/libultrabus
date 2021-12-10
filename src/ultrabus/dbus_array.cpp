/*
 * Copyright (C) 2017,2021 Dan Arrhenius <dan@ultramarin.se>
 *
 * This file is part of libultrabus.
 *
 * libultrabus is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <ultrabus/types.hpp>
#include <ultrabus/dbus_array.hpp>
#include <ultrabus/utils.hpp>
#include <stdexcept>
#include <sstream>
#include <dbus/dbus.h>


//#define ENABLE_DBUS_ARRAY_TRACE

#ifdef ENABLE_DBUS_ARRAY_TRACE
#  include <cstdio>
#  define DBUS_ARRAY_TRACE(format, ...) fprintf(stderr, format "\n", ## __VA_ARGS__)
#else
#  define DBUS_ARRAY_TRACE(format, ...)
#endif


namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator::iterator ()
        : a {nullptr},
          index {0}
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator::iterator (std::vector<dbus_type_ptr>* array, size_t i)
        : a {array},
          index {i}
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::iterator::operator= (
            const dbus_array::iterator& rhs)
    {
        if (this != &rhs) {
            a = rhs.a;
            index = rhs.index;
        }
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::iterator::operator++ ()
    {
        if (++index > a->size())
            index = a->size ();
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::iterator::operator++ (int)
    {
        dbus_array::iterator i = *this;
        index++;
        if (index > a->size())
            index = a->size ();
        return i;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::iterator::operator-- ()
    {
        if (index > 0)
            --index;
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::iterator::operator-- (int)
    {
        dbus_array::iterator i = *this;
        if (index > 0)
            --index;
        return i;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_type& dbus_array::iterator::operator*()
    {
        return *(a->at(index));
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_type* dbus_array::iterator::operator->()
    {
        return a->at(index).get ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool dbus_array::iterator::operator== (const dbus_array::iterator& rhs) const
    {
        return (index == rhs.index) && (a == rhs.a);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool dbus_array::iterator::operator!= (const dbus_array::iterator& rhs) const
    {
        return (index != rhs.index) || (a != rhs.a);
    }




    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::dbus_array ()
        : dbus_type (DBUS_TYPE_ARRAY_AS_STRING),
          element_sig {""}
    {
        DBUS_ARRAY_TRACE ("dbus_array - Default constructor");
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::dbus_array (const dbus_array& a)
    {
        DBUS_ARRAY_TRACE ("dbus_array::dbus_array(const dbus_array&) - copy constructor");
        copy (a);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::dbus_array (dbus_array&& a)
    {
        DBUS_ARRAY_TRACE ("dbus_array::dbus_array(dbus_array&&) - move constructor");
        move (std::forward<dbus_array>(a));
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::dbus_array (const dbus_type& a)
    {
        DBUS_ARRAY_TRACE ("dbus_array::dbus_array(const dbus_type&) - copy constructor");
        copy (a);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::dbus_array (dbus_type&& obj)
    {
        DBUS_ARRAY_TRACE ("dbus_array::dbus_array(dbus_type&&) - move constructor");
        move (std::forward<dbus_type>(obj));
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::dbus_array (const std::string& element_signature)
        : element_sig (element_signature)
    {
        DBUS_ARRAY_TRACE ("dbus_array::dbus_array(const std::string&) - constructor");
        sig = std::string(DBUS_TYPE_ARRAY_AS_STRING) + element_sig;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array& dbus_array::operator= (const dbus_array& obj)
    {
        DBUS_ARRAY_TRACE ("dbus_array::operator=(const dbus_array& obj) - assignment operator");
        if (&obj != this)
            copy (obj);
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array& dbus_array::operator= (dbus_array&& obj)
    {
        DBUS_ARRAY_TRACE ("dbus_array::operator=(dbus_array&& obj) - move operator");
        if (&obj != this)
            move (std::forward<dbus_array>(obj));
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool dbus_array::is_array () const
    {
        return true;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int dbus_array::type_code () const
    {
        return DBUS_TYPE_ARRAY;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_type& dbus_array::operator[] (std::size_t n)
    {
        if (n >= elements.size())
            throw std::out_of_range ("index out of bounds");
        return *elements[n];
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    std::size_t dbus_array::size () const
    {
        return elements.size ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool dbus_array::empty () const
    {
        return elements.empty ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int dbus_array::add (const dbus_type& element)
    {
        if (element_sig.empty()) {
            element_sig = element.signature ();
            sig = std::string(DBUS_TYPE_ARRAY_AS_STRING) + element_sig;
        }
        else if (element_sig != element.signature()) {
            return -1;
        }

        auto new_element = clone_dbus_type (element);
        if (new_element == nullptr)
            throw std::invalid_argument ("Invalid dbus_type");

        elements.push_back (new_element);
        return 0;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int dbus_array::add (dbus_type&& element)
    {
        if (element_sig.empty()) {
            element_sig = element.signature ();
            sig = std::string(DBUS_TYPE_ARRAY_AS_STRING) + element_sig;
        }
        else if (element_sig != element.signature()) {
            return -1;
        }

        auto new_element = clone_dbus_type (std::forward<dbus_type>(element));
        if (new_element == nullptr)
            throw std::invalid_argument ("Invalid dbus_type");

        elements.emplace_back (new_element);
        return 0;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array& dbus_array::operator<< (const dbus_type& element)
    {
        if (add(element)) {
            std::stringstream ss;
            ss << "Can't add and element of type '"
               << element.signature()
               << "' to a dbus_array with elements of type '"
               << element_sig << '\'';
            throw std::invalid_argument (ss.str());
        }
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array& dbus_array::operator<< (dbus_type&& element)
    {
        if (add(std::forward<dbus_type>(element))) {
            std::stringstream ss;
            ss << "Can't add and element of type '"
               << element.signature()
               << "' to a dbus_array with elements of type '"
               << element_sig << '\'';
            throw std::invalid_argument (ss.str());
        }
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool dbus_array::can_add (const dbus_type& element) const
    {
        return elements.empty() || element.signature()==element_sig;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int dbus_array::remove (size_t n)
    {
        auto i = elements.begin ();
        i += n;
        if (i == elements.end())
            return -1;
        elements.erase (i);
        return 0;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_array::clear ()
    {
        elements.clear ();
        element_sig = "";
        sig = std::string(DBUS_TYPE_ARRAY_AS_STRING);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_array::clear (const std::string& element_signature)
    {
        elements.clear ();
        element_sig = element_signature;
        sig = std::string(DBUS_TYPE_ARRAY_AS_STRING) + element_sig;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    const std::string dbus_array::element_signature () const
    {
        return element_sig;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    const std::string dbus_array::str () const
    {
        std::stringstream ss;
        ss << '{';
        for (auto i=elements.begin(); i!=elements.end();) {
            ss << (*i)->str ();
            if (++i != elements.end())
                ss << ',';
        }
        ss << '}';
        return ss.str ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::begin ()
    {
        return iterator (&elements, 0);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_array::iterator dbus_array::end ()
    {
        return iterator (&elements, elements.size());
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_array::copy (const dbus_type& a)
    {
        DBUS_ARRAY_TRACE ("dbus_array::copy(const dbus_type& obj) - obj: %s",
                          a.str().c_str());

        if (!a.is_array()) {
            std::stringstream ss;
            ss << "Can't assign a DBus type with signature '"
               << a.signature()
               << "' to a dbus_array type";
            throw std::invalid_argument (ss.str());
        }
        const dbus_array& rhs {dynamic_cast<const dbus_array&>(a)};
        sig         = rhs.sig;
        element_sig = rhs.element_sig;
        elements.clear ();
        for (auto& element : rhs.elements)
            add (*element);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_array::move (dbus_type&& obj)
    {
        DBUS_ARRAY_TRACE ("dbus_array::move(dbus_type&& obj) - obj: %s",
                          obj.str().c_str());

        if (!obj.is_array()) {
            std::stringstream ss;
            ss << "Can't move a DBus type with signature '"
               << obj.signature()
               << "' to a dbus_array type";
            throw std::invalid_argument (ss.str());
        }
        dbus_array&& rhs {dynamic_cast<dbus_array&&>(obj)};
        sig             = std::move (rhs.sig);
        element_sig     = std::move (rhs.element_sig);
        elements        = std::move (rhs.elements);
        rhs.sig         = DBUS_TYPE_ARRAY_AS_STRING;
        rhs.element_sig = "";
    }


}
