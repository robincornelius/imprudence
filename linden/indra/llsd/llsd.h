/** 
 * @file llsd.h
 * @brief LLSD flexible data system.
 *
 * $LicenseInfo:firstyear=2005&license=mit$
 * 
 * Copyright (c) 2005-2010, Linden Research, Inc.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * $/LicenseInfo$
 */

#ifndef LL_LLSD_H
#define LL_LLSD_H

#include <map>
#include <string>
#include <vector>

#include "stdtypes.h"
#include "stub.h"


/**
	LLSD provides a flexible data system similar to the data facilities of
	dynamic languages like Perl and Python.  It is created to support exchange
	of structured data between loosly coupled systems.  (Here, "loosly coupled"
	means not compiled together into the same module.)
	
	Data in such exchanges must be highly tollerant of changes on either side
	such as:
			- recompilation
			- implementation in a different langauge
			- addition of extra parameters
			- execution of older versions (with fewer parameters)

	To this aim, the C++ API of LLSD strives to be very easy to use, and to
	default to "the right thing" whereever possible.  It is extremely tollerant
	of errors and unexpected situations.
	
	The fundimental class is LLSD.  LLSD is a value holding object.  It holds
	one value that is either undefined, one of the scalar types, or a map or an
	array.  LLSD objects have value semantics (copying them copies the value,
	though it can be considered efficient, due to shareing.), and mutable.

	Undefined is the singular value given to LLSD objects that are not
	initialized with any data.  It is also used as the return value for
	operations that return an LLSD,
	
	The sclar data types are:
		- Boolean	- true or false
		- Integer	- a 32 bit signed integer
		- Real		- a 64 IEEE 754 floating point value
		- UUID		- a 128 unique value
		- String	- a sequence of zero or more Unicode chracters
		- Date		- an absolute point in time, UTC,
						with resolution to the second
		- URI		- a String that is a URI
		- Binary	- a sequence of zero or more octets (unsigned bytes)
	
	A map is a dictionary mapping String keys to LLSD values.  The keys are
	unique within a map, and have only one value (though that value could be
	an LLSD array).
	
	An array is a sequence of zero or more LLSD values.
	
	@nosubgrouping
*/

class LLSD
{
public:
		LLSD();		///< initially Undefined
		~LLSD();	///< this class may NOT be subclassed

	/** @name Copyable and Assignable */
	//@{
		LLSD(const LLSD&);
		void assign(const LLSD& other);
		LLSD& operator=(const LLSD& other)	{ assign(other); return *this; }

	//@}

	void clear();	///< resets to Undefined


	/** @name Scalar Types
	    The scalar types, and how they map onto C++
	*/
	//@{
		typedef bool			Boolean;
		typedef S32				Integer;
		typedef F64				Real;
		typedef std::string		String;
		typedef stub::uuid		UUID;
		typedef stub::date		Date;
		typedef stub::uri		URI;
		typedef std::vector<U8>	Binary;
	//@}
	
	/** @name Scalar Constructors */
	//@{
		LLSD(Boolean);
		LLSD(Integer);
		LLSD(Real);
		LLSD(const String&);
		LLSD(const UUID&);
		LLSD(const Date&);
		LLSD(const URI&);
		LLSD(const Binary&);
	//@}

	/** @name Convenience Constructors */
	//@{
		LLSD(F32); // F32 -> Real
	//@}
	
	/** @name Scalar Assignment */
	//@{
		void assign(Boolean);
		void assign(Integer);
		void assign(Real);
		void assign(const String&);
		void assign(const UUID&);
		void assign(const Date&);
		void assign(const URI&);
		void assign(const Binary&);
		
		LLSD& operator=(Boolean v)			{ assign(v); return *this; }
		LLSD& operator=(Integer v)			{ assign(v); return *this; }
		LLSD& operator=(Real v)				{ assign(v); return *this; }
		LLSD& operator=(const String& v)	{ assign(v); return *this; }
		LLSD& operator=(const UUID& v)		{ assign(v); return *this; }
		LLSD& operator=(const Date& v)		{ assign(v); return *this; }
		LLSD& operator=(const URI& v)		{ assign(v); return *this; }
		LLSD& operator=(const Binary& v)	{ assign(v); return *this; }
	//@}

	/**
		@name Scalar Accessors
		@brief Fetch a scalar value, converting if needed and possible
		
		Conversion among the basic types, Boolean, Integer, Real and String, is
		fully defined.  Each type can be converted to another with a reasonable
		interpretation.  These conversions can be used as a convenience even
		when you know the data is in one format, but you want it in another.  Of
		course, many of these conversions lose information.

		Note: These conversions are not the same as Perl's.  In particular, when
		converting a String to a Boolean, only the empty string converts to
		false.  Converting the String "0" to Boolean results in true.

		Conversion to and from UUID, Date, and URI is only defined to and from
		String.  Conversion is defined to be information preserving for valid
		values of those types.  These conversions can be used when one needs to
		convert data to or from another system that cannot handle these types
		natively, but can handle strings.

		Conversion to and from Binary isn't defined.

		Conversion of the Undefined value to any scalar type results in a
		reasonable null or zero value for the type.
	*/
	//@{
		Boolean	asBoolean() const;
		Integer	asInteger() const;
		Real	asReal() const;
		String	asString() const;
		UUID	asUUID() const;
		Date	asDate() const;
		URI		asURI() const;
		Binary	asBinary() const;

		operator Boolean() const	{ return asBoolean(); }
		operator Integer() const	{ return asInteger(); }
		operator Real() const		{ return asReal(); }
		operator String() const		{ return asString(); }
		operator UUID() const		{ return asUUID(); }
		operator Date() const		{ return asDate(); }
		operator URI() const		{ return asURI(); }
		operator Binary() const		{ return asBinary(); }

		// This is needed because most platforms do not automatically
		// convert the boolean negation as a bool in an if statement.
		bool operator!() const {return !asBoolean();}
	//@}
	
	/** @name Character Pointer Helpers
		These are helper routines to make working with char* the same as easy as
		working with strings.
	 */
	//@{
		LLSD(const char*);
		void assign(const char*);
		LLSD& operator=(const char* v)	{ assign(v); return *this; }
	//@}
	
	/** @name Map Values */
	//@{
		static LLSD emptyMap();
		
		bool has(const String&) const;
		LLSD get(const String&) const;
		void insert(const String&, const LLSD&);
		void erase(const String&);
		LLSD& with(const String&, const LLSD&);
		
		LLSD& operator[](const String&);
		LLSD& operator[](const char* c)			{ return (*this)[String(c)]; }
		const LLSD& operator[](const String&) const;
		const LLSD& operator[](const char* c) const	{ return (*this)[String(c)]; }
	//@}
	
	/** @name Array Values */
	//@{
		static LLSD emptyArray();
		
		LLSD get(Integer) const;
		void set(Integer, const LLSD&);
		void insert(Integer, const LLSD&);
		void append(const LLSD&);
		void erase(Integer);
		LLSD& with(Integer, const LLSD&);
		
		const LLSD& operator[](Integer) const;
		LLSD& operator[](Integer);
	//@}

	/** @name Iterators */
	//@{
		int size() const;

		typedef std::map<String, LLSD>::iterator		map_iterator;
		typedef std::map<String, LLSD>::const_iterator	map_const_iterator;
		
		map_iterator		beginMap();
		map_iterator		endMap();
		map_const_iterator	beginMap() const;
		map_const_iterator	endMap() const;
		
		typedef std::vector<LLSD>::iterator			array_iterator;
		typedef std::vector<LLSD>::const_iterator	array_const_iterator;
		
		array_iterator			beginArray();
		array_iterator			endArray();
		array_const_iterator	beginArray() const;
		array_const_iterator	endArray() const;
	//@}
	
	/** @name Type Testing */
	//@{
		enum Type {
			TypeUndefined,
			TypeBoolean,
			TypeInteger,
			TypeReal,
			TypeString,
			TypeUUID,
			TypeDate,
			TypeURI,
			TypeBinary,
			TypeMap,
			TypeArray
		};
		
		Type type() const;
		
		bool isUndefined() const	{ return type() == TypeUndefined; }
		bool isDefined() const		{ return type() != TypeUndefined; }
		bool isBoolean() const		{ return type() == TypeBoolean; }
		bool isInteger() const		{ return type() == TypeInteger; }
		bool isReal() const			{ return type() == TypeReal; }
		bool isString() const		{ return type() == TypeString; }
		bool isUUID() const			{ return type() == TypeUUID; }
		bool isDate() const			{ return type() == TypeDate; }
		bool isURI() const			{ return type() == TypeURI; }
		bool isBinary() const		{ return type() == TypeBinary; }
		bool isMap() const			{ return type() == TypeMap; }
		bool isArray() const		{ return type() == TypeArray; }
	//@}

	/** @name Automatic Cast Protection
		These are not implemented on purpose.  Without them, C++ can perform
		some conversions that are clearly not what the programmer intended.
		
		If you get a linker error about these being missing, you have made
		mistake in your code.  DO NOT IMPLEMENT THESE FUNCTIONS as a fix.
		
		All of thse problems stem from trying to support char* in LLSD or in
		std::string.  There are too many automatic casts that will lead to
		using an arbitrary pointer or scalar type to std::string.
	 */
	//@{
		LLSD(const void*);				///< construct from aribrary pointers
		void assign(const void*);		///< assign from arbitrary pointers
		LLSD& operator=(const void*);	///< assign from arbitrary pointers
		
		bool has(Integer) const;		///< has only works for Maps
	//@}
	
	/** @name Implementation */
	//@{
public:
		class Impl;
private:
		Impl* impl;
	//@}
	
	/** @name Unit Testing Interface */
	//@{
public:
		static U32 allocationCount();	///< how many Impls have been made
		static U32 outstandingCount();	///< how many Impls are still alive
	//@}

private:
	/** @name Debugging Interface */
	//@{
		/// Returns pointer to human readable representation.
		//  The returned char pointer is only valid until the next call to this
		//  function. This call is intended to be used from the debugger.
		static const char *dump(const LLSD &llsd);
	//@}
};


#endif // LL_LLSD_H
