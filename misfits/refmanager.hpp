// --8<--8<--8<--8<--
//
// Copyright (C) 2015 Smithsonian Astrophysical Observatory
//
// This file is part of misfits
//
// misfits is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -->8-->8-->8-->8--

// -*-c++-*-

#ifndef misFITS_REFMANAGER_H
#define misFITS_REFMANAGER_H

#include <string>

#include <misfits/fits.hpp>

namespace misFITS {


    namespace RefManager {

	enum Mode { Bare, Observer, Owner };

	template<class T>
	class Base {

	public:
	    virtual std::shared_ptr<T> operator () () = 0;
	    virtual ~Base() {}

	};

	template<class T, Mode Ptr> class Type;

	template<class T>
	class Type<T, Bare> : public Base<T> {

	private:
	    std::shared_ptr<T> ptr_;

	    static void null_deleter( T* ptr ) { }

	public:
	    Type( T* ptr ) : ptr_( std::shared_ptr<T>( ptr, null_deleter ) ) {}
	    std::shared_ptr<T> operator () () { return ptr_; }

	};

	template<class T>
	class Type<T, Observer> : public Base<T> {

	public:
	    typedef std::shared_ptr<T> SharedPtr;
	    typedef std::weak_ptr<T>   WeakPtr;

	    Type( WeakPtr& ptr ) : ptr_( ptr ) {}
	    SharedPtr operator () () { return ptr_.lock(); }

	private:
	    WeakPtr ptr_;
	};

	template<class T>
	class Type<T, Owner> : public Base<T> {

	public:
	    typedef std::shared_ptr<T> SharedPtr;

	    Type( T ptr ) : ptr_( ptr ) {}
	    Type( SharedPtr ptr ) : ptr_( ptr ) {}
	    SharedPtr operator () () { return SharedPtr( ptr_ ); }

	private:
	    SharedPtr ptr_;
	};


    }

}


#endif // ! misFITS_REFMANAGER_H
