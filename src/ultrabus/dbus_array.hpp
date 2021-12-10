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
#ifndef ULTRABUS_DBUS_ARRAY_HPP
#define ULTRABUS_DBUS_ARRAY_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type.hpp>
#include <string>
#include <vector>
#include <memory>


namespace ultrabus {


    /**
     * A DBus array type.
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#container-types rel="noopener noreferrer" target="_blank">DBus Container Types at dbus.freedesktop.org</a>
     */
    class dbus_array : public dbus_type {
    public:

        /**
         * Iterator for class dbus_array.
         */
        class iterator {
        public:
            iterator ();
            iterator (std::vector<dbus_type_ptr>* array, size_t index);
            iterator operator++ ();
            iterator operator++ (int);
            iterator operator-- ();
            iterator operator-- (int);
            dbus_type& operator*();
            dbus_type* operator->();
            iterator operator= (const iterator& rhs);
            bool operator== (const iterator& rhs) const;
            bool operator!= (const iterator& rhs) const;

        private:
            std::vector<dbus_type_ptr>* a;
            size_t index;
        };


        /**
         * Default constructor.
         * Creates an empty array. The first added element will
         * define the type of elements in the array.
         */
        dbus_array ();

        /**
         * Destructor.
         */
        virtual ~dbus_array () = default;

        /**
         * Copy constructor. Make a deep copy of another dbus_array object.
         * @param a The dbus_array object to copy.
         */
        dbus_array (const dbus_array& a);

        /**
         * Move constructor.
         * @param a The dbus_array object to move.
         */
        dbus_array (dbus_array&& a);

        /**
         * Copy construcor. Make a deep copy of another dbus_array object.
         * @param obj The dbus_type object to copy.
         *            If <code>obj</code> is not a reference to a dbus_array
         *            object an empty dbus_array will be created
         *            and an error message will be logged.
         */
        dbus_array (const dbus_type& a);

        /**
         * Move construcor.
         * @param obj The dbus_type object to move to this instance.
         *            If <code>obj</code> is not a reference to a
         *            dbus_array object an empty dbus_array will be
         *            created, <code>obj</code> will not be modified
         *            and an error message will be logged.
         */
        dbus_array (dbus_type&& a);

        /**
         * Creates an empty array with a specific type on elements.
         * @param The DBus signature of the elements in the array.
         */
        explicit dbus_array (const std::string& element_signature);

        /**
         * Asignment operator. Make a deep copy of another dbus_array object.
         * @param obj The dbus_array object to copty.
         */
        dbus_array& operator= (const dbus_array& obj);

        /**
         * Move operator.
         * @param array The dbus_array object to move to this instance.
         */
        dbus_array& operator= (dbus_array&& obj);

        /**
         * Check is this is a dbus_array object.
         * @return <code>true</code> since this is an array type.
         */
        virtual bool is_array () const;

        /**
         * Return the DBus type code.
         * @return The DBus type code.
         */
        virtual int type_code () const;

        /**
         * Access the n:th element in the dbus_array.
         * @return A reference to the n:th element in the dbus_array.
         * @throw std::out_of_range If the index is out of bounds.
         */
        dbus_type& operator[] (std::size_t n);

        /**
         * Return the number of elements in the array.
         * @return The number of objects in the array.
         */
        std::size_t size () const;

        /**
         * Check if the array is empty.
         * @return <code>true</code> if the array is empty.
         */
        bool empty () const;

        /**
         * Add an object to the array.
         * If this is the first object added to the array it
         * will define what kind of DBus type the array will hold.
         * If an object with a different DBus type is added,
         * exception <code>std::invalid_argument</code> will be thrown.
         * @param element A copy of <code>element</code> will be added to the array.
         * @return 0 on success. -1 if an object with an invalid DBus type is added.
         */
        int add (const dbus_type& element);

        /**
         * Add an object to the array.
         * If this is the first object added to the array it
         * will define what kind of DBus type the array will hold.
         * If an object with a different DBus type is added,
         * exception <code>std::invalid_argument</code> will be thrown.
         * @param element Object that will be moved to the array.
         * @return 0 on success. -1 if an object with an invalid DBus type is added.
         */
        int add (dbus_type&& element);

        /**
         * Add an object to the array.
         * If this is the first object added to the array it
         * will define what kind of DBus type the array will hold.
         * If an object with a different DBus type is added,
         * exception <code>std::invalid_argument</code> will be thrown.
         * @param element A copy of <code>element</code> will be added to the array.
         * @throw std::invalid_argument If an object with an invalid
         *        DBus type is added.
         */
        dbus_array& operator<< (const dbus_type& element);

        /**
         * Add an object to the array.
         * If this is the first object added to the array it
         * will define what kind of DBus type the array will hold.
         * If an object with a different DBus type is added,
         * exception <code>std::invalid_argument</code> will be thrown.
         * @param element Object that will be moved to the array.
         * @throw std::invalid_argument If an object with an invalid
         *        DBus type is added.
         */
        dbus_array& operator<< (dbus_type&& element);

        /**
         * Check is this element can be added to the array.
         * A DBus array can only contain elements of the same type.
         * @return <code>true</code> if the element can be added to the array.
         */
        bool can_add (const dbus_type& element) const;

        /**
         * Remove an element from the array.
         * @param n Index of the element to remove.
         * @return 0 on success. -1 if the index is out of bounds.
         */
        int remove (size_t n);

        /**
         * Clear the array from items.
         */
        void clear ();

        /**
         * Clear the array from items and set the specific type of elements it can contain.
         * @param The DBus signature of the elements in the array.
         */
        void clear (const std::string& element_signature);

        /**
         * Return the DBus signature of the type of elements in the array.
         * If the array is empty the signature will be an empty strung.
         * @return The signature of the type of elements the array holds.
         */
        const std::string element_signature () const;

        /**
         * Return a string representation of the object.
         * @return A string representing the dbus_array object.
         */
        virtual const std::string str () const;

        /**
         * Return an iterator representing the beginning of the array.
         */
        iterator begin ();

        /**
         * Return an iterator representing the end of the array.
         */
        iterator end ();

    protected:
        /**
         * Make a deep copy another dbus_array object.
         * @param a The dbus_array object to copy.
         */
        virtual void copy (const dbus_type& rhs);

        /**
         * Move another dbus_array object to this instance.
         * @param array The dbus_type object to move.
         *              If <code>obj</code> is not a reference to a
         *              dbus_array object an empty dbus_array will be
         *              created, <code>obj</code> will not be modified
         *              and an error message will be logged.
         */
        virtual void move (dbus_type&& rhs);

    private:
        std::vector<dbus_type_ptr> elements;
        std::string element_sig;
    };

    /**
     * An alias of a shared pointer to a dbus_array class.
     * @see dbus_array
     */
    using dbus_array_ptr = std::shared_ptr<dbus_array>;

    /**
     * Cast an dbus_type pointer to an dbus_array pointer.
     * @return A shared pointer to an dbus_array object or a nullptr.
     */
    static inline dbus_array_ptr dbus_type_to_dbus_array (const dbus_type_ptr& ptr) {
        return ptr!=nullptr ? std::dynamic_pointer_cast<dbus_array>(ptr) : nullptr;
    }



}


#endif
