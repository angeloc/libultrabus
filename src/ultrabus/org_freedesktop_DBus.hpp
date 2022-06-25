/*
 * Copyright (C) 2021,2022 Dan Arrhenius <dan@ultramarin.se>
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
#ifndef ULTRABUS_ORG_FREEDESKTOP_DBUS_HPP
#define ULTRABUS_ORG_FREEDESKTOP_DBUS_HPP

#include <ultrabus/Connection.hpp>
#include <ultrabus/MessageHandler.hpp>
#include <ultrabus/retvalue.hpp>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <set>
#include <cstdint>


namespace ultrabus {


    /**
     * Proxy class to use methods in standard DBus <code>interface org.freedesktop.DBus</code>.
     *
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-messages
     *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages</a>
     */
    class org_freedesktop_DBus : public MessageHandler {
    public:
        /**
         * Callback called when when the owner of a bus name has changed.
         * @param name The bus name that has an owner change.
         * @param old_owner The old owner, or an empty string if none.
         * @param new_owner The new owner, or an empty string if none.
         */
        using name_owner_changed_cb_t = std::function<void (const std::string& name,
                                                            const std::string& old_owner,
                                                            const std::string& new_owner)>;

        /**
         * Callback called when a bus name is acquired or lost.
         * @param name The bus name that is acquired or lost.
         */
        using name_cb_t = std::function<void (const std::string& name)>;

        /**
         * Constructor.
         * Constructs an object used for calling methods in the standard
         * DBUs interface <code>org.freedesktop.DBus</code>.
         * @param connection A DBus connection.
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-messages
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages</a>
         */
        org_freedesktop_DBus (Connection& connection);

        /**
         * Destructor.
         * Clean up resources. Removes any added callback.
         */
        virtual ~org_freedesktop_DBus () = default;

        /**
         * Hello.
         * This is the first message an application sends to the message
         * bus to obtain a unique name. This message is automatically
         * sent by the Connection object when connecting to the bus.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous hello method instead.
         *
         * @return A unique name assigned to this connection.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-hello
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: Hello</a>
         */
        retvalue<std::string> hello ();

        /**
         * Asynchronous call to send Hello.
         *
         * Hello is the first message an application sends to the message
         * bus to obtain a unique name. This message is automatically
         * sent by the Connection object when connecting to the bus.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-hello
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: Hello</a>
         */
        int hello (std::function<void (retvalue<std::string>& retval)> callback);

        /**
         * Request a name DBus connection name.
         * Ask the message bus to give this connection a specific name.
         * If another application already owns the name this application
         * may be put in a queue to take the ownership, it may 'steal' the
         * ownership, or simply fail. The bahavior depends on the flags
         * used when asking for ownership.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous request_name method instead.
         *
         * @param bus_name The requested bus name.
         * @param flags The following flags may be OR'ed together:
         *                - \b DBUS_NAME_FLAG_ALLOW_REPLACEMENT Allow another
         *                  application to take ownership of this name it
         *                  that application uses the flag
         *                  DBUS_NAME_FLAG_REPLACE_EXISTING.
         *                - \b DBUS_NAME_FLAG_REPLACE_EXISTING Try ot take
         *                  ownership of the name if another application
         *                  already owns it.
         *                - \b DBUS_NAME_FLAG_DO_NOT_QUEUE If taking ownership
         *                  of the name fails, do not put this application
         *                  in queue for the ownership.
         * @return The return value can be one of the following values:
         *           - \b DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER This
         *             application is now the primary owner of the name.
         *           - \b DBUS_REQUEST_NAME_REPLY_IN_QUEUE The application
         *             is currently in queue for ownership of the name.
         *           - \b DBUS_REQUEST_NAME_REPLY_EXISTS The application
         *             failed to get ownership of the name and is not put
         *             in a queue.
         *           - \b DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER The application
         *             already have ownership of the name.
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-request-name
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: RequestName</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<uint32_t> request_name (const std::string bus_name, uint32_t flags=0);

        /**
         * Asynchronous call to request a name DBus connection name.
         * Ask the message bus to give this connection a specific name.
         * If another application already owns the name this application
         * may be put in a queue to take the ownership, it may 'steal' the
         * ownership, or simply fail. The bahavior depends on the flags
         * used when asking for ownership.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param bus_name The requested bus name.
         * @param flags The following flags may be OR'ed together:
         *                - \b DBUS_NAME_FLAG_ALLOW_REPLACEMENT Allow another
         *                  application to take ownership of this name it
         *                  that application uses the flag
         *                  DBUS_NAME_FLAG_REPLACE_EXISTING.
         *                - \b DBUS_NAME_FLAG_REPLACE_EXISTING Try ot take
         *                  ownership of the name if another application
         *                  already owns it.
         *                - \b DBUS_NAME_FLAG_DO_NOT_QUEUE If taking ownership
         *                  of the name fails, do not put this application
         *                  in queue for the ownership.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-request-name
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: RequestName</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int request_name (const std::string bus_name,
                          uint32_t flags,
                          std::function<void (retvalue<uint32_t>& retval)> callback);

        /**
         * Release a previously requested DBus connection name.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>release_name</code> method instead.
         *
         * @param bus_name The bus name to release.
         * @return One of the following return values:
         *           - \b DBUS_RELEASE_NAME_REPLY_RELEASED The application
         *             has released ownership of the name.
         *           - \b DBUS_RELEASE_NAME_REPLY_NON_EXISTENT The application
         *             tried to release a name that didn't exist on the bus.
         *           - \b DBUS_RELEASE_NAME_REPLY_NOT_OWNER The application
         *             tried to release a name it didn't own.
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-release-name
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ReleaseName</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<uint32_t> release_name (const std::string bus_name);

        /**
         * Asynchronous call to release a previously
         * requested DBus connection name.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param bus_name The bus name to release.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-release-name
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ReleaseName</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int release_name (const std::string bus_name,
                          std::function<void (retvalue<uint32_t>& retval)> callback);

        /**
         * List the connections currently queued for owning a bus name.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>list_queued_owners</code> method instead.
         *
         * @param bus_name A bus name.
         * @return A list of unique bus names waiting to own the specified bus name.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-list-queued-owners
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ListQueuedOwners</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<std::vector<std::string>> list_queued_owners (const std::string& bus_name);

        /**
         * Asynchronous call to list the connections currently
         * queued for owning a bus name.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param bus_name A bus name.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-list-queued-owners
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ListQueuedOwners</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int list_queued_owners (const std::string& bus_name,
                                std::function<void (retvalue<std::vector<std::string>>& retval)> callback);

        /**
         * Return a set of all bus names.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>list_names</code> method instead.
         *
         * @return A set of all bus name.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-list-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ListNames</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<std::set<std::string>> list_names ();

        /**
         * Asynchronous call to return a set of all bus names.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-list-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ListNames</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int list_names (std::function<void (retvalue<std::set<std::string>>& retval)> callback);

        /**
         * Returns a set of all names that can be activated on the bus.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>list_activatable_names</code> method instead.
         *
         * @return A set of all names that can be activated on the bus.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-list-activatable-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ListActivatableNames</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<std::set<std::string>> list_activatable_names ();

        /**
         * Asynchronous call to return a set of all names that can be activated on the bus.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-list-activatable-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: ListActivatableNames</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int list_activatable_names (std::function<void (retvalue<std::set<std::string>>& retval)> callback);

        /**
         * Checks if the specified bus name exists (currently has an owner).
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>name_has_owner</code> method instead.
         *
         * @return <code>true</code> if the specified bus name exists.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-name-exists
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: NamesHasOwner</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<bool> name_has_owner (const std::string bus_name);

        /**
         * Asynchronous call to check if the specified bus name exists (currently has an owner).
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-name-exists
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: NamesHasOwner</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int name_has_owner (const std::string bus_name, std::function<void (retvalue<bool>& retval)> callback);

        /**
         * Tries to launch the executable associated with a
         * name (service activation), as an explicit request.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>start_service_by_name</code> method instead.
         *
         * @param service The service (bus name) to start.
         * @param flags Flags (currently not used).
         * @return One of the following return values:
         *           - \b DBUS_START_REPLY_SUCCESS The service was
         *             successfully started.
         *           - \b DBUS_START_REPLY_ALREADY_RUNNING A connection
         *             already owns the given name.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-start-service-by-name
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: StartServiceByName</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<uint32_t> start_service_by_name (const std::string service, uint32_t flags=0);

        /**
         * Asynchronous call to try to launch the executable associated with a
         * name (service activation), as an explicit request.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service The service (bus name) to start.
         * @param flags Flags (currently not used).
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-start-service-by-name
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: StartServiceByName</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int start_service_by_name (const std::string service,
                                   uint32_t flags,
                                   std::function<void (retvalue<uint32_t>& retval)> callback);

        /**
         * Add to or modify the environment variables of activated services.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>update_activation_environment</code> method instead.
         *
         * @param env The environment variables and values to add/modify.
         * @return 0 on success, -1 on failure.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-update-activation-environment
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: UpdateActivationEnvironment</a>
         */
        retvalue<int> update_activation_environment (const std::map<std::string, std::string>& env);

        /**
         * Asynchronous call to add to or modify the environment variables of activated services.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param env The environment variables and values to add/modify.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-update-activation-environment
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: UpdateActivationEnvironment</a>
         */
        int update_activation_environment (const std::map<std::string, std::string>& env,
                                           std::function<void (retvalue<int>& retval)> callback);

        /**
         * Returns the unique connection name of the primary owner of the name given.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_name_owner</code> method instead.
         *
         * @param bus_name The bus name to query.
         * @return A unique connection name.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-name-owner
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetNameOwner</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<std::string> get_name_owner (const std::string bus_name);

        /**
         * Asynchronous call to get the unique connection name of the primary owner of the name given.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param bus_name The bus name to query.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-name-owner
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetNameOwner</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int get_name_owner (const std::string bus_name,
                            std::function<void (retvalue<std::string>& retval)> callback);

        /**
         * Returns the Unix user ID of the process connected to the server.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_connection_unix_user</code> method instead.
         *
         * @param service The bus name to query.
         * @return A Unix user id.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-connection-unix-user
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetConnectionUnixUser</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<uint32_t> get_connection_unix_user (const std::string service);

        /**
         * Asynchronous call to get the Unix user ID of the process connected to the server.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service The bus name to query.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-connection-unix-user
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetConnectionUnixUser</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int get_connection_unix_user (const std::string service,
                                      std::function<void (retvalue<uint32_t>& retval)> callback);

        /**
         * Returns the Unix process ID of the process connected to the server.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_connection_unix_process_id</code> method instead.
         *
         * @param service The bus name to query.
         * @return A Unix process id.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-connection-unix-process-id
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetConnectionUnixProcessID</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<uint32_t> get_connection_unix_process_id (const std::string service);

        /**
         * Asynchronous call to get the Unix process ID of the process connected to the server.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service The bus name to query.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-connection-unix-process-id
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetConnectionUnixProcessID</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int get_connection_unix_process_id (const std::string service,
                                            std::function<void (retvalue<uint32_t>& retval)> callback);

        /**
         * Returns as many credentials as possible for the process connected to the server.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_connection_credentials</code> method instead.
         *
         * @param service The bus name to query.
         * @return Connection credentials.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-connection-credentials
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetConnectionCredentials</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<std::map<std::string, dbus_variant>> get_connection_credentials (
                const std::string service);

        /**
         * Asynchronous call to get as many credentials as possible for the process connected to the server.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service The bus name to query.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-connection-credentials
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetConnectionCredentials</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int get_connection_credentials (
                const std::string service,
                std::function<void (retvalue<std::map<std::string, dbus_variant>>& retval)> callback);

        /**
         * Adds a match rule to match messages going through the message bus.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>add_match</code> method instead.
         *
         * @param rule Match rule to add to the connection.
         * @return 0 on success, -1 on failure.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-add-match
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: AddMatch</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Match Rules</a>
         */
        retvalue<int> add_match (const std::string rule);

        /**
         * Asynchronous call to add a match rule to match messages going through the message bus.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param rule Match rule to add to the connection.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-add-match
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: AddMatch</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Match Rules</a>
         */
        int add_match (const std::string rule, std::function<void (retvalue<int>& retval)> callback);

        /**
         * Remove the first rule that matches.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>remove_match</code> method instead.
         *
         * @param rule Match rule to remove from the connection.
         * @return 0 on success, -1 on failure.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-remove-match
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: RemoveMatch</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Match Rules</a>
         */
        retvalue<int> remove_match (const std::string rule);

        /**
         * Asynchronous call to remove the first rule that matches.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param rule Match rule to remove from the connection.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-remove-match
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: RemoveMatch</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Match Rules</a>
         */
        int remove_match (const std::string rule, std::function<void (retvalue<int>& retval)> callback);

        /**
         * Get the unique ID of the bus.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_id</code> method instead.
         *
         * @return Unique ID identifying the bus daemon.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-id
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetId</a>
         */
        retvalue<std::string> get_id (void);

        /**
         * Asynchronous call to get the unique ID of the bus.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-get-id
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: GetId</a>
         */
        int get_id (std::function<void (retvalue<std::string>& retval)> callback);

        /**
         * Converts the connection into a monitor connection
         * which can be used as a debugging/monitoring tool.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>become_monitor</code> method instead.
         *
         * @param rules An optional list of match rules.
         * @return 0 on success, -1 on failure.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-become-monitor
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: BecomeMonitor</a>
         */
        retvalue<int> become_monitor (std::list<std::string> rules={});

        /**
         * Asynchronous call to convert the connection into a monitor
         * connection which can be used as a debugging/monitoring tool.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param rules An optional list of match rules.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-become-monitor
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: BecomeMonitor</a>
         */
        int become_monitor (std::list<std::string> rules,
                            std::function<void (retvalue<int>& retval)> callback);

        /**
         * Set a callback to be called when the owner of a bus name has changed.
         * @param callback The callback to be called when the owner of a bus name has changed.
         *                 If <code>nullptr</code>, remove the callback.
         *
         * @see name_owner_changed_cb_t
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-name-owner-changed
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: NameOwnerChanged</a>
         */
        void set_name_owner_changed_cb (name_owner_changed_cb_t callback);

        /**
         * Set a callback to be called when the application loses ownership of a bus name.
         * @param callback The callback to be called, or <code>nullptr</code> to remove the callback.
         *
         * @see name_cb_t
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-name-lost
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: NameLost</a>
         */
        void set_name_lost_cb (name_cb_t callback);

        /**
         * Set a callback to be called when the application acquires ownership of a bus name.
         * @param callback The callback to be called, or <code>nullptr</code> to remove the callback.
         *
         * @see name_cb_t
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#bus-messages-name-acquired
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Messages: NameAcquired</a>
         */
        void set_name_acquired_cb (name_cb_t callback);


    protected:
        virtual bool on_signal (Message& msg);


    private:
        std::mutex cb_mutex;
        name_owner_changed_cb_t name_owner_changed_cb;
        name_cb_t name_lost_cb;
        name_cb_t name_acquired_cb;

        std::string unique_bus_name;
        void get_bus_name ();
        void on_signal_impl (Message& msg, std::unique_lock<std::mutex>& cb_lock);
    };



}
#endif
