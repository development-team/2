/*
 * opencog/atomspace/AtomSpaceImpl.h
 *
 * Copyright (C) 2010-2011 OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Joel Pitt <joel@opencog.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_ATOMSPACE_IMPL_H
#define _OPENCOG_ATOMSPACE_IMPL_H

#include <algorithm>
#include <list>
#include <set>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/signal.hpp>

#include <opencog/atomspace/AtomTable.h>
#include <opencog/atomspace/AttentionValue.h>
#include <opencog/atomspace/AttentionBank.h>
#include <opencog/atomspace/BackingStore.h>
#include <opencog/atomspace/ClassServer.h>
#include <opencog/atomspace/HandleSet.h>
#include <opencog/atomspace/SpaceServer.h>
#include <opencog/atomspace/TemporalTable.h>
#include <opencog/atomspace/TruthValue.h>
#include <opencog/util/exceptions.h>
#include <opencog/util/recent_val.h>

namespace opencog
{

class AtomSpaceImpl;

typedef boost::signal<void (AtomSpaceImpl*,Handle)> AtomSignal;

/** 
 * WARNING: The AtomSpaceImpl class contains methods that are only to be called by
 * AtomSpace requests that are running within the AtomSpaceAsync event loop.
 */
class AtomSpaceImpl
{
    friend class SavingLoading;
    friend class SaveRequest;

    SpaceServer* spaceServer;
    /**
     * Used to fetch atoms from disk.
     */
    BackingStore *backing_store;

    /** Provided signals */
    AtomSignal _addAtomSignal;
    AtomSignal _removeAtomSignal;
    AtomSignal _mergeAtomSignal;

    AttentionBank bank;

public:
    AtomSpaceImpl(void);
    ~AtomSpaceImpl();

    void setSpaceServer(SpaceServer* ss) {spaceServer = ss;};
    AttentionBank& getAttentionBank();

    /**
     * Register a provider of backing storage.
     */
    void registerBackingStore(BackingStore *);
    void unregisterBackingStore(BackingStore *);

    /**
     * Recursively store the atom to the backing store.
     * I.e. if the atom is a link, then store all of the atoms
     * in its outgoing set as well, recursively.
     */
    void storeAtom(Handle h) {
        if (backing_store) backing_store->storeAtom(h);
    }

    /**
     * Return the atom with the indicated handle. This method will
     * explicitly use the backing store to obtain an instance of the
     * atom. If an atom corresponding to the handle cannot be found,
     * then an undefined handle is returned. If the atom is found, 
     * then the corresponding atom is guaranteed to have been
     * instantiated in the atomspace.
     */
    Handle fetchAtom(Handle);

    /**
     * Use the backing store to load the entire incoming set of the atom.
     * If the flag is true, then the load is done recursively. 
     * This method queries the backing store to obtain all atoms that 
     * contain this one in their outgoing sets. All of these atoms are
     * then loaded into this atomtable/atomspace.
     */
    Handle fetchIncomingSet(Handle, bool);

    /**
     * @return a const reference to the AtomTable
     */
    const AtomTable& getAtomTable() const;

    /**
     * Return the number of atoms contained in the space.
     */
    int getSize() const { return atomTable.getSize(); }

    /**
     * Prints atoms of this AtomTable to the given output stream.
     * @param output  the output stream where the atoms will be printed.
     * @param type  the type of atoms that should be printed.
     * @param subclass  if true, matches all atoms whose type is
     *              subclass of the given type. If false, matches
     *              only atoms of the exact type.
     */
    void print(std::ostream& output = std::cout,
               Type type = ATOM, bool subclass = true) const;

    //helpers for GDB debugging, because trying to get std::cout is annoying
    void printGDB() const;
    void printTypeGDB(Type t) const;

    /** Add a new node to the Atom Table,
     * if the atom already exists then the old and the new truth value is merged
     *  @param t     Type of the node
     *  @param name  Name of the node
     *  @param tvn   Optional TruthValue of the node. If not provided, uses the
     *  DEFAULT_TV (see TruthValue.h)
     */
    Handle addNode(Type t, const std::string& name = "", const TruthValue& tvn = TruthValue::DEFAULT_TV());

    /**
     * Add a new link to the Atom Table
     * If the atom already exists then the old and the new truth value
     * is merged
     * @param t         Type of the link
     * @param outgoing  a const reference to a HandleSeq containing
     *                  the outgoing set of the link
     * @param tvn       Optional TruthValue of the node. If not
     *                  provided, uses the DEFAULT_TV (see TruthValue.h)
     */
    Handle addLink(Type t, const HandleSeq& outgoing,
                   const TruthValue& tvn = TruthValue::DEFAULT_TV());

    /**
     * DEPRECATED!
     *
     * Add an atom an optional TruthValue object to the Atom Table
     * This is a deprecated function; do not use it in new code,
     * if at all possible.
     *
     * @param atom the handle of the Atom to be added
     * @param tvn the TruthValue object to be associated to the added
     *        atom. NULL if the own atom's tv must be used.
     * @deprecated This is a legacy code left-over from when one could
     * have non-real atoms, i.e. those whose handles were
     * less than 500, and indicated types, not atoms.
     * Instead of using that method, one should use
     * addNode or addLink (which is a bit faster too).
     */
    Handle addRealAtom(const Atom& atom,
                       const TruthValue& tvn = TruthValue::NULL_TV());

    /**
     * Removes an atom from the atomspace
     *
     * @param h The Handle of the atom to be removed.
     * @param recursive Recursive-removal flag; if set, the links in the
     *        incoming set of the atom to be removed will also be
     *        removed.
     * @return True if the Atom for the given Handle was successfully
     *         removed. False, otherwise.
     *
     * When the atom is removed from the atomspace, all memory associated
     * with it is also deleted; in particular, the atom is removed from
     * the TLB as well, so that future TLB lookups will be invalid. 
     */
    bool removeAtom(Handle h, bool recursive = false);

    /**
     * Retrieve from the Atom Table the Handle of a given node
     *
     * @param t     Type of the node
     * @param str   Name of the node
    */
    Handle getHandle(Type t, const std::string& str) const;

    /**
     * Retrieve from the Atom Table the Handle of a given link
     * @param t        Type of the node
     * @param outgoing a reference to a HandleSeq containing
     *        the outgoing set of the link.
    */
    Handle getHandle(Type t, const HandleSeq& outgoing) const;

    /** Get the atom referred to by Handle h represented as a string. */
    std::string atomAsString(Handle h, bool terse = true) const;

    /** Retrieve the name of a given Handle */
    const std::string& getName(Handle) const;

    /** Retrieve the type of a given Handle */
    Type getType(Handle) const;

    /** Retrieve the TruthValue of a given Handle */
    const TruthValue& getTV(Handle, VersionHandle = NULL_VERSION_HANDLE) const;

    /** Change the TruthValue of a given Handle
     * @return whether TV was successfully set
     */
    bool setTV(Handle, const TruthValue&, VersionHandle = NULL_VERSION_HANDLE);

    /** Change the primary TV's mean of a given Handle */
    void setMean(Handle, float mean) throw (InvalidParamException);

    /** Retrieve the doubly normalised Short-Term Importance between -1..1
     * for a given AttentionValueHolder. STI above and below threshold
     * normalised separately and linearly.
     *
     * @param h The attention value holder to get STI for
     * @param average Should the recent average max/min STI be used, or the
     * exact min/max?
     * @param clip Should the returned value be clipped to -1..1? Outside this
     * range can be return if average=true
     * @return normalised STI between -1..1
     */
    float getNormalisedSTI(AttentionValueHolder *avh, bool average=true, bool clip=false) const;

    /** Retrieve the linearly normalised Short-Term Importance between 0..1
     * for a given AttentionValueHolder.
     *
     * @param h The attention value holder to get STI for
     * @param average Should the recent average max/min STI be used, or the
     * exact min/max?
     * @param clip Should the returned value be clipped to 0..1? Outside this
     * range can be return if average=true
     * @return normalised STI between 0..1
     */
    float getNormalisedZeroToOneSTI(AttentionValueHolder *avh, bool average=true, bool clip=false) const;

    /** Retrieve the Long-term Importance of a given AttentionValueHolder */
    AttentionValue::lti_t getLTI(AttentionValueHolder *avh) const;

    /** Retrieve the Very-Long-Term Importance of a given
     * AttentionValueHolder */
    AttentionValue::vlti_t getVLTI(AttentionValueHolder *avh) const;

    /** Retrieve the AttentionValue of a given Handle */
    const AttentionValue& getAV(Handle h) const {
        return bank.getAV(atomTable.getAtom(h));
    }

    /** Change the AttentionValue of a given Handle */
    void setAV(Handle h, const AttentionValue &av) {
        bank.setAV(atomTable.getAtom(h), av);
    }

    /** Change the Short-Term Importance of a given Handle */
    void setSTI(Handle h, AttentionValue::sti_t stiValue) {
        bank.setSTI(atomTable.getAtom(h), stiValue);
    }

    /** Change the Long-term Importance of a given Handle */
    void setLTI(Handle h, AttentionValue::lti_t ltiValue) {
        bank.setLTI(atomTable.getAtom(h), ltiValue);
    }

    /** Increase the Very-Long-Term Importance of a given Handle by 1 */
    void incVLTI(Handle h) {
        bank.incVLTI(atomTable.getAtom(h));
    }

    /** Decrease the Very-Long-Term Importance of a given Handle by 1 */
    void decVLTI(Handle h) {
        bank.decVLTI(atomTable.getAtom(h));
    }

    /** Retrieve the Short-Term Importance of a given Handle */
    AttentionValue::sti_t getSTI(Handle h) const {
        return bank.getSTI(atomTable.getAtom(h));
    }

    /** Retrieve the doubly normalised Short-Term Importance between -1..1
     * for a given Handle. STI above and below threshold normalised separately
     * and linearly.
     *
     * @param h The atom handle to get STI for
     * @param average Should the recent average max/min STI be used, or the
     * exact min/max?
     * @param clip Should the returned value be clipped to -1..1? Outside this
     * range can be return if average=true
     * @return normalised STI between -1..1
     */
    float getNormalisedSTI(Handle h, bool average=true, bool clip=false) const {
        return getNormalisedSTI(atomTable.getAtom(h), average, clip);
    }

    /** Retrieve the linearly normalised Short-Term Importance between 0..1
     * for a given Handle.
     *
     * @param h The atom handle to get STI for
     * @param average Should the recent average max/min STI be used, or the
     * exact min/max?
     * @param clip Should the returned value be clipped to 0..1? Outside this
     * range can be return if average=true
     * @return normalised STI between 0..1
     */
    float getNormalisedZeroToOneSTI(Handle h, bool average=true, bool clip=false) const {
        return getNormalisedZeroToOneSTI(atomTable.getAtom(h), average, clip);
    }

    /** Retrieve the Long-term Importance of a given Handle */
    AttentionValue::lti_t getLTI(Handle h) const {
        return bank.getLTI(atomTable.getAtom(h));
    }

    /** Retrieve the Very-Long-Term Importance of a given Handle */
    AttentionValue::vlti_t getVLTI(Handle h) const {
        return bank.getVLTI(atomTable.getAtom(h));
    }

    /** Clone an atom from the AtomSpace.
     * Threads outside of the AtomSpace thread can safely use this pointer and modify
     * the atom.
     * @param h Handle of atom to clone
     * @return A smart pointer to the atom
     * @note Any changes to the atom object must be committed using
     * AtomSpace::commitAtom for them to be merged with the AtomSpace.
     * Otherwise changes are lost.
     */
    boost::shared_ptr<Atom> cloneAtom(const Handle& h) const;

    /** Commit an atom that has been cloned from the AtomSpace.
     *
     * @param a Atom to commit
     * @return whether the commit was successful
     */
    bool commitAtom(const Atom& a);

    /** Get hash for an atom
     */
    size_t getAtomHash(const Handle& h) const;

    bool isValidHandle(const Handle& h) const;

    /**
     * Returns neighboring atoms, following links and returning their
     * target sets.
     * @param h Get neighbours for the atom this handle points to.
     * @param fanin Whether directional links point to this node should be
     * considered.
     * @param fanout Whether directional links point from this node to
     * another should be considered.
     * @param linkType Follow only these types of links.
     * @param subClasses Follow subtypes of linkType too.
     */
    HandleSeq getNeighbors(const Handle& h, bool fanin=true, bool fanout=true,
            Type linkType=LINK, bool subClasses=true) const;

    /** Retrieve a single Handle from the outgoing set of a given link */
    Handle getOutgoing(Handle, int idx) const;

    /** Retrieve the arity of a given link */
    int getArity(Handle) const;

    /** Return whether s is the source handle in a link l
     * @note Only ORDERED_LINKs have a source handle.
     */ 
    bool isSource(Handle source, Handle link) const;

    /** Retrieve the outgoing set of a given link */
    const HandleSeq& getOutgoing(Handle h) const;

    /** Retrieve the incoming set of a given atom */
    HandleSeq getIncoming(Handle);

    /** Convenience functions... */
    bool isNode(const Handle& h) const;
    bool isLink(const Handle& h) const;

    /**
     * Gets a set of handles that matches with the given arguments.
     *
     * @param result An output iterator.
     * @param type the type of the atoms to be searched
     * @param name the name of the atoms to be searched.
     *        For searching only links, use "" or a search by type.
     * @param subclass if sub types of the given type are accepted in this search
     * @param vh only atoms that contains versioned TVs with
     *        the given VersionHandle are returned. If NULL_VERSION_HANDLE is given,
     *        it does not restrict the result.
     *
     * @return The set of atoms of a given type (subclasses optionally).
     *
     * @note The matched entries are appended to a container whose
     * OutputIterator is passed as the first argument. Example of a
     * call to this method, which would return all entries in AtomSpace:
     * @code
     *      std::list<Handle> ret;
     *      atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 Type type,
                 const std::string& name,
                 bool subclass = true,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {
        HandleEntry * handleEntry = atomTable.getHandleSet(name.c_str(), type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Gets a set of handles that matches with the given type
     * (subclasses optionally).
     *
     * @param result An output iterator.
     * @param type The desired type.
     * @param subclass Whether type subclasses should be considered.
     * @param vh only atoms that contains versioned TVs with the given VersionHandle are returned.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     *
     * @return The set of atoms of a given type (subclasses optionally).
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 Type type,
                 bool subclass = false,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms of a given type which have atoms of a
     * given target type in their outgoing set (subclasses optionally).
     *
     * @param result An output iterator.
     * @param type The desired type.
     * @param targetType The desired target type.
     * @param subclass Whether type subclasses should be considered.
     * @param targetSubclass Whether target type subclasses should be considered.
     * @param vh only atoms that contains versioned TVs with the given VersionHandle are returned.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @param targetVh only atoms whose target contains versioned TVs with the given VersionHandle are returned.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of a given type and target type (subclasses
     * optionally).
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 Type type,
                 Type targetType,
                 bool subclass,
                 bool targetSubclass,
                 VersionHandle vh = NULL_VERSION_HANDLE,
                 VersionHandle targetVh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(type, targetType, subclass, targetSubclass, vh, targetVh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms with a given target handle in their
     * outgoing set (atom type and its subclasses optionally).
     *
     * @param result An output iterator.
     * @param handle The handle that must be in the outgoing set of the atom.
     * @param type The type of the atom.
     * @param subclass Whether atom type subclasses should be considered.
     * @param vh only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of the given type with the given handle in
     * their outgoing set.
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 Handle handle,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(handle, type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms with the given target handles and types
     * (order is considered) in their outgoing sets, where the type and
     * subclasses of the atoms are optional.
     *
     * @param result An output iterator.
     * @param handles An array of handles to match the outgoing sets of the searched
     * atoms. This array can be empty (or each of its elements can be null), if
     * the handle value does not matter or if it does not apply to the
     * specific search.
     * Note that if this array is not empty, it must contain "arity" elements.
     * @param types An array of target types to match the types of the atoms in
     * the outgoing set of searched atoms.
     * @param subclasses An array of boolean values indicating whether each of the
     * above types must also consider subclasses. This array can be null,
     * what means that subclasses will not be considered. Note that if this
     * array is not null, it must contains "arity" elements.
     * @param arity The length of the outgoing set of the atoms being searched.
     * @param type The type of the atom.
     * @param subclass Whether atom type subclasses should be considered.
     * @param vh only atoms that contains versioned TVs with the given VersionHandle are returned.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of the given type with the matching
     * criteria in their outgoing set.
     *
     * @note The matched entries are appended to a container whose OutputIterator
     * is passed as the first argument. Example of call to this method, which
     * would return all entries in AtomSpace:
     * @code
     *     std::list<Handle> ret;
     *     atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 const HandleSeq& handles,
                 Type* types,
                 bool* subclasses,
                 Arity arity,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(handles, types,
                subclasses, arity, type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms of a given name (atom type and subclasses
     * optionally).
     *
     * @param result An output iterator.
     * @param name The desired name of the atoms.
     * @param type The type of the atom.
     * @param subclass Whether atom type subclasses should be considered.
     * @param vh only atoms that contains versioned TVs with the given VersionHandle are returned.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of the given type and name.
     *
     * @note The matched entries are appended to a container whose
     * OutputIterator is passed as the first argument.
     *
     * @note Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 const char* name,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(name, type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms whose outgoing set contains at least one
     * atom with the given name and type (atom type and subclasses
     * optionally).
     *
     * @param result An output iterator.
     * @param targetName The name of the atom in the outgoing set of the searched
     * atoms.
     * @param targetType The type of the atom in the outgoing set of the searched
     * atoms.
     * @param type type of the atom.
     * @param subclass Whether atom type subclasses should be considered.
     * @param vh return only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of the given type and name whose outgoing
     * set contains at least one atom of the given type and name.
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 const char* targetName,
                 Type targetType,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE,
                 VersionHandle targetVh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(targetName, targetType, type, subclass, vh, targetVh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms with the given target names and/or types
     * (order is considered) in their outgoing sets, where the type
     * and subclasses arguments of the searched atoms are optional.
     *
     * @param result An output iterator.
     * @param names An array of names to match the outgoing sets of the searched
     * atoms. This array (or each of its elements) can be null, if
     * the names do not matter or if do not apply to the specific search.
     * Note that if this array is not null, it must contain "arity" elements.
     * @param types An array of target types to match the types of the atoms in
     * the outgoing set of searched atoms. If array of names is not null,
     * this parameter *cannot* be null as well. Besides, if an element in a
     * specific position in the array of names is not null, the corresponding
     * type element in this array *cannot* be NOTYPE as well.
     * @param subclasses An array of boolean values indicating whether each of the
     * above types must also consider subclasses. This array can be null,
     * what means that subclasses will not be considered. Not that if this
     * array is not null, it must contains "arity" elements.
     * @param arity The length of the outgoing set of the atoms being searched.
     * @param type The optional type of the atom.
     * @param subclass Whether atom type subclasses should be considered.
     * @param vh return only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of the given type with the matching
     * criteria in their outgoing set.
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 const char** names,
                 Type* types,
                 bool* subclasses,
                 Arity arity,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(names, types, subclasses, arity, type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Returns the set of atoms with the given target names and/or types
     * (order is considered) in their outgoing sets, where the type
     * and subclasses arguments of the searched atoms are optional.
     *
     * @param result An output iterator.
     * @param types An array of target types to match the types of the atoms in
     * the outgoing set of searched atoms. This parameter can be null (or any of
     * its elements can be NOTYPE), what means that the type doesnt matter.
     * Not that if this array is not null, it must contains "arity" elements.
     * @param subclasses An array of boolean values indicating whether each of the
     * above types must also consider subclasses. This array can be null,
     * what means that subclasses will not be considered. Not that if this
     * array is not null, it must contains "arity" elements.
     * @param arity The length of the outgoing set of the atoms being searched.
     * @param type The optional type of the atom.
     * @param subclass Whether atom type subclasses should be considered.
     * @param vh returns only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     * @return The set of atoms of the given type with the matching
     * criteria in their outgoing set.
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSet(OutputIterator result,
                 Type* types,
                 bool* subclasses,
                 Arity arity,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(types, subclasses, arity, type, subclass, vh);
        return (toOutputIterator(result, handleEntry));
    }

    /**
     * Gets a set of handles in the Attentional Focus that matches with the given type
     * (subclasses optionally).
     *
     * @param result An output iterator.
     * @param type The desired type.
     * @param subclass Whether type subclasses should be considered.
     * @param vh returns only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     *
     * @return The set of atoms of a given type (subclasses optionally).
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace in the AttentionalFocus:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSetInAttentionalFocus(OutputIterator result,
                 Type type,
                 bool subclass,
                 VersionHandle vh = NULL_VERSION_HANDLE) const
    {
        //return getHandleSet(result, type, subclass, InAttentionalFocus(), vh);
        STIAboveThreshold s = STIAboveThreshold(bank.getAttentionalFocusBoundary());
        return getHandleSetFiltered(result, type, subclass, &s, vh);

    }

    /**
     * Gets a set of handles that matches with the given type
     * (subclasses optionally) and a given criterion.
     *
     * @param result An output iterator.
     * @param type The desired type.
     * @param subclass Whether type subclasses should be considered.
     * @param compare A criterion for including atoms. It must be something that returns a bool when called.
     * @param vh returns only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     *
     * @return The set of atoms of a given type (subclasses optionally).
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace beyond 500 LTI:
     * @code
     *         std::list<Handle> ret;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true, LTIAboveThreshold(500));
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getHandleSetFiltered(OutputIterator result,
                 Type type,
                 bool subclass,
                 AtomPredicate* compare,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {

        HandleEntry * handleEntry = atomTable.getHandleSet(type, subclass, vh);
        std::vector<Handle> hs;
        // these two lines could be replaced using a function that filters
        // a handleEntry list into an arbitrary sequence
        toOutputIterator(back_inserter(hs), handleEntry);
        return filter(hs.begin(), hs.end(), result, compare);
    }

    /**
     * Gets a set of handles that matches with the given type
     * (subclasses optionally), sorted according to the given comparison
     * structure.
     *
     * @param result An output iterator.
     * @param type The desired type.
     * @param subclass Whether type subclasses should be considered.
     * @param compare The comparison struct to use in the sort.
     * @param vh returns only atoms that contains versioned TVs with the given VersionHandle.
     *        If NULL_VERSION_HANDLE is given, it does not restrict the result.
     *
     * @return The set of atoms of a given type (subclasses optionally).
     *
     * @note The matched entries are appended to a container whose OutputIterator is passed as the first argument.
     *          Example of call to this method, which would return all entries in AtomSpace, sorted by STI:
     * @code
     *         std::list<Handle> ret;
     *         AttentionValue::STISort stiSort;
     *         atomSpaceImpl.getHandleSet(back_inserter(ret), ATOM, true, stiSort);
     * @endcode
     */
    template <typename OutputIterator> OutputIterator
    getSortedHandleSet(OutputIterator result,
                 Type type,
                 bool subclass,
                 AtomComparator* compare,
                 VersionHandle vh = NULL_VERSION_HANDLE) const {
        // get the handle set as a vector and sort it.
        std::vector<Handle> hs;

        getHandleSet(back_inserter(hs), type, subclass, vh);
        sort(hs.begin(), hs.end(), compareAtom<AtomComparator>(&atomTable, compare));

        // copy the vector and return the iterator.
        return copy(hs.begin(), hs.end(), result);
    }

    // Wrapper for comparing atoms from a HandleSeq
    template <typename Compare>
    struct compareAtom{
        Compare* c;
        const AtomTable* table;
        compareAtom(const AtomTable* _table, Compare* _c) : c(_c), table(_table) {}

        bool operator()(const Handle& h1,const Handle& h2) {
            return (*c)(*table->getAtom(h1),*table->getAtom(h2));
        }
    };

    template <typename Compare>
    struct filterAtom{
        Compare *c;
        const AtomTable* table;
        filterAtom(const AtomTable* _table, Compare *_c) : c(_c), table(_table) {}

        bool operator()(const Handle& h1) {
            return (*c)(*table->getAtom(h1));
        }
    };

    /* ----------------------------------------------------------- */
    /* The foreach routines offer an alternative interface
     * to the getHandleSet API.
     */
    /**
     * Invoke the callback on each handle of the given type.
     */
    template<class T>
    inline bool foreach_handle_of_type(Type atype,
                                       bool (T::*cb)(Handle), T *data,
                                       bool subclass = false) {
        std::list<Handle> handle_set;
        // The intended signatue is
        // getHandleSet(OutputIterator result, Type type, bool subclass)
        getHandleSet(back_inserter(handle_set), atype, subclass);

        // Loop over all handles in the handle set.
        std::list<Handle>::iterator i;
        for (i = handle_set.begin(); i != handle_set.end(); i++) {
            Handle h = *i;
            bool rc = (data->*cb)(h);
            if (rc) return rc;
        }
        return false;
    }

    template<class T>
    inline bool foreach_handle_of_type(const char * atypename,
                                       bool (T::*cb)(Handle), T *data,
                                       bool subclass = false) {
        Type atype = classserver().getType(atypename);
        return foreach_handle_of_type(atype, cb, data, subclass);
    }

    /* ----------------------------------------------------------- */

    /**
     * Decays STI of all atoms (one cycle of importance decay).
     * Deprecated, importance updating should be done by ImportanceUpdating
     * Agent. Still used by Embodiment.
     */
    void decayShortTermImportance();

    int Nodes(VersionHandle = NULL_VERSION_HANDLE) const;
    int Links(VersionHandle = NULL_VERSION_HANDLE) const;

    bool containsVersionedTV(Handle h, VersionHandle vh) const;

    //! Clear the atomspace, remove all atoms
    void clear();

// ---- filter templates

    HandleSeq filter(AtomPredicate* compare, VersionHandle vh = NULL_VERSION_HANDLE) {
        HandleSeq result;
        _getNextAtomPrepare();
        Handle next;
        while ((next = _getNextAtom()) != Handle::UNDEFINED)
            if ((*compare)(*atomTable.getAtom(next)) && containsVersionedTV(next, vh))
                result.push_back(next);
        return result;
    }

    template<typename OutputIterator>
    OutputIterator filter(OutputIterator it, AtomPredicate* compare, VersionHandle vh = NULL_VERSION_HANDLE) {
        _getNextAtomPrepare();
        Handle next;
        while ((next = _getNextAtom()) != Handle::UNDEFINED)
            if ((*compare)(*atomTable.getAtom(next)) && containsVersionedTV(next, vh))
                * it++ = next;
        return it;
    }

    /**
     * Filter handles from a sequence according to the given criterion.
     *
     * @param begin iterator for the sequence
     * @param end iterator for the sequence
     * @param struct or function embodying the criterion
     * @return The handles in the sequence that match the criterion.
     */
    template<typename InputIterator>
    HandleSeq filter(InputIterator begin, InputIterator end, AtomPredicate* compare) const {
        HandleSeq result;
        for (; begin != end; begin++)
            if (filterAtom<AtomPredicate>(&atomTable,compare)(*begin))
                result.push_back(*begin);

        return result;
    }

    template<typename InputIterator, typename OutputIterator>
    OutputIterator filter(InputIterator begin, InputIterator end,
            OutputIterator it, AtomPredicate* compare) const {
        for (; begin != end; begin++)
            if (filterAtom<AtomPredicate>(&atomTable,compare)(*begin))
                * it++ = *begin;
        return it;
    }

    template<typename InputIterator>
    HandleSeq filter_InAttentionalFocus(InputIterator begin, InputIterator end) const {
        STIAboveThreshold stiAbove(bank.getAttentionalFocusBoundary());
        return filter(begin, end, &stiAbove);
    }

    struct STIAboveThreshold : public AtomPredicate {
        STIAboveThreshold(const AttentionValue::sti_t t) : threshold (t) {}

        virtual bool test(const Atom& a) {
            return a.getAttentionValue().getSTI() > threshold;
        }
        AttentionValue::sti_t threshold;
    };

    struct LTIAboveThreshold : public AtomPredicate {
        LTIAboveThreshold(const AttentionValue::lti_t t) : threshold (t) {}

        virtual bool test(const Atom& a) {
            return a.getAttentionValue().getLTI() > threshold;
        }
        AttentionValue::lti_t threshold;
    };

    // AtomSpaceRequests are not allowed to access the TLB, but they may get
    // references to specific atoms.
    inline const Atom& getAtom(Handle h) {
        return *atomTable.getAtom(h);
    }

protected:

    HandleIterator* _handle_iterator;
    TypeIndex::iterator type_itr;
    // these methods are used by the filter_* templates
    void _getNextAtomPrepare();
    Handle _getNextAtom();
    void _getNextAtomPrepare_type(Type type);
    Handle _getNextAtom_type(Type type);

private:

    AtomTable atomTable;
    std::string emptyName;

    /** The AtomSpace currently acts like event loop, but some legacy code (such as
     * saving/loading) might not like the AtomSpace changing while acting upon
     * it. Those that absolutely require it can get a lock to halt the event loop.
     * @warn This should only be used as a last resort, you need to add your
     * class as a friend class to the AtomSpace, and there is no
     * guarantee this will be available in the future.
     */
    mutable pthread_mutex_t atomSpaceLock;

    /**
     * signal connections used to keep track of atom removal in the AtomTable
     */
    boost::signals::connection removedAtomConnection; 
    boost::signals::connection addedAtomConnection; 

    /* Boundary at which an atom is considered within the attentional
     * focus of opencog. Atom's with STI less than this value are
     * not charged STI rent */
    AttentionValue::sti_t attentionalFocusBoundary;

    opencog::recent_val<AttentionValue::sti_t> maxSTI;
    opencog::recent_val<AttentionValue::sti_t> minSTI;

    /* These indicate the amount importance funds available in the
     * AtomSpace */
    long fundsSTI;
    long fundsLTI;

    /*
     * Remove stimulus from atom, only should be used when Atom is deleted.
     */
    void removeStimulus(Handle h);

    template <typename OutputIterator> OutputIterator
    toOutputIterator(OutputIterator result, HandleEntry * handleEntry) const {

        HandleEntry * toRemove = handleEntry;
        while (handleEntry) {
            *(result++) = handleEntry->handle;
            handleEntry = handleEntry->next;
        }
        // free memory
        if (toRemove) delete toRemove;
        return result;
    }

    /**
     * Creates the space map node, if not created yet.
     * returns the handle of the node.
     */
    Handle getSpaceMapNode(void);

    /**
     * Handler of the 'atom removed' signal from self
     */
    void atomRemoved(AtomSpaceImpl *a, Handle h);

    /**
     * Handler of the 'atom added' signal from self
     */
    void atomAdded(AtomSpaceImpl *a, Handle h);

public:
    // pass on the signals from the Atom Table
    AtomSignal& addAtomSignal()
        { return _addAtomSignal; }
    AtomSignal& removeAtomSignal()
        { return _removeAtomSignal; }
    AtomSignal& mergeAtomSignal()
        { return _mergeAtomSignal; }

    /**
     * Overrides and declares copy constructor and equals operator as private 
     * for avoiding large object copying by mistake.
     */
    AtomSpaceImpl& operator=(const AtomSpaceImpl&);
    AtomSpaceImpl(const AtomSpaceImpl&);

    bool saveToXML(const std::string& filename) const;

};

} // namespace opencog

#endif // _OPENCOG_ATOMSPACE_IMPL_H
