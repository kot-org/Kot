// Copyright (c) 2019, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

// zLib license (https://www.zlib.net/zlib_license.html):
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


#ifndef PLF_STACK_H
#define PLF_STACK_H



// Compiler-specific defines used by stack:
#if defined(_MSC_VER)
	#define PLF_STACK_FORCE_INLINE __forceinline

	#if _MSC_VER < 1600
		#define PLF_STACK_NOEXCEPT throw()
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1600
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT throw()
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1700
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT throw()
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
	#elif _MSC_VER == 1800
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT throw()
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) throw()
		#define PLF_STACK_INITIALIZER_LIST_SUPPORT
	#elif _MSC_VER >= 1900
		#define PLF_STACK_ALIGNMENT_SUPPORT
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
		#define PLF_STACK_NOEXCEPT noexcept
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_STACK_INITIALIZER_LIST_SUPPORT
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
		#define PLF_STACK_CONSTEXPR constexpr
	#else
		#define PLF_STACK_CONSTEXPR
	#endif

#elif defined(__cplusplus) && __cplusplus >= 201103L // C++11 support, at least
	#define PLF_STACK_FORCE_INLINE // note: GCC creates faster code without forcing inline

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4 // 4.2 and below do not support variadic templates
			#define PLF_STACK_VARIADICS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4 // 4.3 and below do not support initializer lists
			#define PLF_STACK_INITIALIZER_LIST_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ < 6) || __GNUC__ < 4
			#define PLF_STACK_NOEXCEPT throw()
			#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
		#elif __GNUC__ < 6
			#define PLF_STACK_NOEXCEPT noexcept
			#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept
		#else // C++17 support
			#define PLF_STACK_NOEXCEPT noexcept
			#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4
			#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ > 4
			#define PLF_STACK_ALIGNMENT_SUPPORT
		#endif
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#endif
	#elif defined(__GLIBCXX__) // Using another compiler type with libstdc++ - we are assuming full c++11 compliance for compiler - which may not be true
		#if __GLIBCXX__ >= 20080606 	// libstdc++ 4.2 and below do not support variadic templates
			#define PLF_STACK_VARIADICS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20090421 	// libstdc++ 4.3 and below do not support initializer lists
			#define PLF_STACK_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20160111
			#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_STACK_NOEXCEPT noexcept
			#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
			#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value)
		#elif __GLIBCXX__ >= 20120322
			#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_STACK_NOEXCEPT noexcept
			#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept
			#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept
		#else
			#define PLF_STACK_NOEXCEPT throw()
			#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
			#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
		#endif
		#if __GLIBCXX__ >= 20130322
			#define PLF_STACK_ALIGNMENT_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#endif
	#elif defined(_LIBCPP_VERSION)
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_INITIALIZER_LIST_SUPPORT
		#define PLF_STACK_ALIGNMENT_SUPPORT
		#define PLF_STACK_NOEXCEPT noexcept
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept

		#if !(defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES))
			#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#endif
	#else // Assume type traits and initializer support for other compilers and standard libraries
		#define PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_STACK_ALIGNMENT_SUPPORT
		#define PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_INITIALIZER_LIST_SUPPORT
		#define PLF_STACK_TYPE_TRAITS_SUPPORT
		#define PLF_STACK_NOEXCEPT noexcept
		#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator) noexcept(std::allocator_traits<the_allocator>::is_always_equal::value)
		#define PLF_STACK_NOEXCEPT_SWAP(the_allocator) noexcept
	#endif

	#if __cplusplus >= 201703L
		#if defined(__clang__) && ((__clang_major__ == 3 && __clang_minor__ == 9) || __clang_major__ > 3)
			#define PLF_STACK_CONSTEXPR constexpr
		#elif defined(__GNUC__) && __GNUC__ >= 7
			#define PLF_STACK_CONSTEXPR constexpr
		#elif !defined(__clang__) && !defined(__GNUC__)
			#define PLF_STACK_CONSTEXPR constexpr // assume correct C++17 implementation for other compilers
		#else
			#define PLF_STACK_CONSTEXPR
		#endif
	#else
		#define PLF_STACK_CONSTEXPR
	#endif

	#define PLF_STACK_MOVE_SEMANTICS_SUPPORT
#else
	#define PLF_STACK_FORCE_INLINE
	#define PLF_STACK_NOEXCEPT throw()
	#define PLF_STACK_NOEXCEPT_SWAP(the_allocator)
	#define PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(the_allocator)
	#define PLF_STACK_CONSTEXPR
#endif


#ifdef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, ...) std::allocator_traits<the_allocator>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, data) std::allocator_traits<the_allocator>::construct(allocator_instance, location, data)
	#endif

	#define PLF_STACK_DESTROY(the_allocator, allocator_instance, location) 			std::allocator_traits<the_allocator>::destroy(allocator_instance, location)
	#define PLF_STACK_ALLOCATE(the_allocator, allocator_instance, size, hint) 		std::allocator_traits<the_allocator>::allocate(allocator_instance, size, hint)
 	#define PLF_STACK_ALLOCATE_INITIALIZATION(the_allocator, size, hint) 			std::allocator_traits<the_allocator>::allocate(*this, size, hint)
	#define PLF_STACK_DEALLOCATE(the_allocator, allocator_instance, location, size) std::allocator_traits<the_allocator>::deallocate(allocator_instance, location, size)
#else
	#ifdef PLF_STACK_VARIADICS_SUPPORT
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, ...) 	allocator_instance.construct(location, __VA_ARGS__)
	#else
		#define PLF_STACK_CONSTRUCT(the_allocator, allocator_instance, location, data) 	allocator_instance.construct(location, data)
	#endif

	#define PLF_STACK_DESTROY(the_allocator, allocator_instance, location) 				allocator_instance.destroy(location)
	#define PLF_STACK_ALLOCATE(the_allocator, allocator_instance, size, hint) 			allocator_instance.allocate(size, hint)
	#define PLF_STACK_ALLOCATE_INITIALIZATION(the_allocator, size, hint) 				the_allocator::allocate(size, hint)
	#define PLF_STACK_DEALLOCATE(the_allocator, allocator_instance, location, size) 	allocator_instance.deallocate(location, size)
#endif





#include <cstring>	// memset, memcpy
#include <cassert>	// assert
#include <limits>  // std::numeric_limits
#include <memory>	// std::uninitialized_copy, std::allocator


#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
	#include <cstddef> // offsetof, used in blank()
	#include <type_traits> // std::is_trivially_destructible
#endif




namespace plf
{


template <class element_type, class element_allocator_type = std::allocator<element_type> > class stack : private element_allocator_type  // Empty base class optimisation - inheriting allocator functions
{
public:
	// Standard container typedefs:
	typedef element_type																value_type;
	typedef element_allocator_type														allocator_type;

	#ifdef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::size_type		size_type;
		typedef element_type &															reference;
		typedef const element_type &													const_reference;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 		pointer;
		typedef typename std::allocator_traits<element_allocator_type>::const_pointer	const_pointer;
	#else
		typedef typename element_allocator_type::size_type			size_type;
		typedef typename element_allocator_type::reference			reference;
		typedef typename element_allocator_type::const_reference	const_reference;
		typedef typename element_allocator_type::pointer			pointer;
		typedef typename element_allocator_type::const_pointer		const_pointer;
	#endif

private:
	struct group; // Forward declaration for typedefs below

	#ifdef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<group> group_allocator_type;
		typedef typename std::allocator_traits<group_allocator_type>::pointer		group_pointer_type;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 	element_pointer_type;
	#else
		typedef typename element_allocator_type::template rebind<group>::other	group_allocator_type;
		typedef typename group_allocator_type::pointer							group_pointer_type;
		typedef typename element_allocator_type::pointer						element_pointer_type;
	#endif

	struct group : private element_allocator_type
	{
		const element_pointer_type		elements;
		group_pointer_type				next_group, previous_group;
		const element_pointer_type		end; // End is the actual end element of the group, not one-past the end element as it is in list


		#ifdef PLF_STACK_VARIADICS_SUPPORT
			group(const size_type elements_per_group, group_pointer_type const previous = NULL):
				elements(PLF_STACK_ALLOCATE_INITIALIZATION(element_allocator_type, elements_per_group, (previous == NULL) ? 0 : previous->elements)),
				next_group(NULL),
				previous_group(previous),
				end(elements + elements_per_group - 1)
			{}


		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
			group(const size_type elements_per_group, group_pointer_type const previous = NULL) PLF_STACK_NOEXCEPT:
				elements(NULL),
				next_group(reinterpret_cast<group_pointer_type>(elements_per_group)),
				previous_group(previous),
				end(NULL)
			{}


			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source):
				element_allocator_type(source),
				elements(PLF_STACK_ALLOCATE_INITIALIZATION(element_allocator_type, reinterpret_cast<size_type>(source.next_group), (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL),
				previous_group(source.previous_group),
				end(elements + reinterpret_cast<size_type>(source.next_group) - 1)
			{}
		#endif



		~group() PLF_STACK_NOEXCEPT
		{
			// Null check not necessary (for empty group and copied group as above) as delete will ignore.
			PLF_STACK_DEALLOCATE(element_allocator_type, (*this), elements, static_cast<size_type>((end - elements) + 1)); // Size is calculated from end and elements
		}
	};


	group_pointer_type		current_group, first_group;
	element_pointer_type		top_element, start_element, end_element;
	size_type					total_number_of_elements, min_elements_per_group;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with the least-used member variable, for empty-base-class optimization
	{
		size_type max_elements_per_group;
		explicit ebco_pair(const size_type max_elements) PLF_STACK_NOEXCEPT: max_elements_per_group(max_elements) {};
	}						group_allocator_pair;



public:

	// Default constructor:
	stack() PLF_STACK_NOEXCEPT:
		element_allocator_type(element_allocator_type()),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(0),
		min_elements_per_group((sizeof(element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(element_type)) + 1),
		group_allocator_pair(std::numeric_limits<size_type>::max() / 2)
	{
		assert(min_elements_per_group > 2);
		assert(min_elements_per_group <= group_allocator_pair.max_elements_per_group);
	}


	// Allocator-extended constructor:
	explicit stack(const element_allocator_type &alloc):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(0),
		min_elements_per_group((sizeof(element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(element_type)) + 1),
		group_allocator_pair(std::numeric_limits<size_type>::max() / 2)
	{
		assert(min_elements_per_group > 2);
		assert(min_elements_per_group <= group_allocator_pair.max_elements_per_group);
	}



	// Constructor with minimum & maximum group size parameters:
	stack(const size_type min_allocation_amount, const size_type max_allocation_amount = (std::numeric_limits<size_type>::max() / 2)) PLF_STACK_NOEXCEPT:
		element_allocator_type(element_allocator_type()),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(0),
		min_elements_per_group(min_allocation_amount),
		group_allocator_pair(max_allocation_amount)
	{
		assert(min_elements_per_group > 2);
		assert(min_elements_per_group <= group_allocator_pair.max_elements_per_group);
		assert(group_allocator_pair.max_elements_per_group <= std::numeric_limits<size_type>::max() / 2);
	}



	// Allocator-extended constructor with minimum & maximum group size parameters:
	stack(const size_type min_allocation_amount, const size_type max_allocation_amount, const element_allocator_type &alloc):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(0),
		min_elements_per_group(min_allocation_amount),
		group_allocator_pair(max_allocation_amount)
	{
		assert(min_elements_per_group > 2);
		assert(min_elements_per_group <= group_allocator_pair.max_elements_per_group);
		assert(group_allocator_pair.max_elements_per_group <= std::numeric_limits<size_type>::max() / 2); // Must be half of what the allocator can allocate, otherwise could result in overflow, because at the point where we might allocate a max group of that size, the previous groups will have a total size equal to it, as each group doubles the previous capacity of the stack.
	}



private:

	void copy_from_source(const stack &source)
	{
		assert(&source != this);

		if (total_number_of_elements == 0)
		{
			return;
		}

		group_pointer_type current_copy_group = source.first_group;
		const group_pointer_type end_copy_group = source.current_group;

		if (total_number_of_elements <= group_allocator_pair.max_elements_per_group) // most common case
		{
			min_elements_per_group = total_number_of_elements;
			initialize();
			min_elements_per_group = source.min_elements_per_group;

			// Copy groups to this stack:
			while (current_copy_group != end_copy_group)
			{
				std::uninitialized_copy(current_copy_group->elements, current_copy_group->end + 1, top_element);
				top_element += (current_copy_group->end + 1) - current_copy_group->elements;
				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			std::uninitialized_copy(source.start_element, source.top_element + 1, top_element);
			end_element = (top_element += (source.top_element - source.start_element)); // This should make top_element == the last "pushed" element, rather than the one past it
		}
		else // uncommon edge case, so not optimising:
		{
			min_elements_per_group = group_allocator_pair.max_elements_per_group;
			total_number_of_elements = 0;
			initialize();

			while (current_copy_group != end_copy_group)
			{
				for (element_pointer_type element_to_copy = current_copy_group->elements; element_to_copy != current_copy_group->end + 1; ++element_to_copy)
				{
					push(*element_to_copy);
				}

				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			for (element_pointer_type element_to_copy = source.start_element; element_to_copy != source.top_element + 1; ++element_to_copy)
			{
				push(*element_to_copy);
			}

			min_elements_per_group = source.min_elements_per_group;
		}
	}


public:


	// Copy constructor:
	stack(const stack &source):
		element_allocator_type(source),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(source.total_number_of_elements),
		min_elements_per_group(source.min_elements_per_group),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
	{
		copy_from_source(source);
	}



	// Allocator-extended copy constructor:
	stack(const stack &source, const allocator_type &alloc):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_number_of_elements(source.total_number_of_elements),
		min_elements_per_group(source.min_elements_per_group),
		group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
	{
		copy_from_source(source);
	}



	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		// move constructor
		stack(stack &&source) PLF_STACK_NOEXCEPT:
			element_allocator_type(source),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			top_element(std::move(source.top_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_number_of_elements(source.total_number_of_elements),
			min_elements_per_group(source.min_elements_per_group),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
		{
			source.blank();
		}


		// allocator-extended move constructor
		stack(stack &&source, const allocator_type &alloc):
			element_allocator_type(alloc),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			top_element(std::move(source.top_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_number_of_elements(source.total_number_of_elements),
			min_elements_per_group(source.min_elements_per_group),
			group_allocator_pair(source.group_allocator_pair.max_elements_per_group)
		{
			source.blank();
		}
	#endif



	~stack() PLF_STACK_NOEXCEPT
	{
		destroy_all_data();
	}



private:

	void destroy_all_data() PLF_STACK_NOEXCEPT
	{
		#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
			if ((total_number_of_elements != 0) & !(std::is_trivially_destructible<element_type>::value)) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with empty destructor.
		#else // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
			if (total_number_of_elements != 0)
		#endif
		{
			while (first_group != current_group)
			{
				const element_pointer_type past_end = first_group->end + 1;

				for (element_pointer_type element_pointer = first_group->elements; element_pointer != past_end; ++element_pointer)
				{
					PLF_STACK_DESTROY(element_allocator_type, (*this), element_pointer);
				}

				const group_pointer_type next_group = first_group->next_group;
				PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, first_group);
				PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
				first_group = next_group;
			}

			// Special case for current group:
			const element_pointer_type past_end = top_element + 1;

			for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
			{
				PLF_STACK_DESTROY(element_allocator_type, (*this), element_pointer);
			}

			first_group = first_group->next_group;
			PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, current_group);
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
		}

		total_number_of_elements = 0;

		while (first_group != NULL)
		{
			current_group = first_group;
			first_group = first_group->next_group;
			PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, current_group);
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group, 1);
		}
	}



	void initialize()
	{
		first_group = current_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		// Initialize:
		try
		{
			#ifdef PLF_STACK_VARIADICS_SUPPORT
				PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, min_elements_per_group);
			#else
				PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(min_elements_per_group));
			#endif
		}
		catch (...)
		{
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = current_group = NULL;
			throw;
		}

		top_element = start_element = first_group->elements;
		end_element = first_group->end;
	}



public:

	void push(const element_type &element)
	{
		switch ((top_element == NULL) + (top_element == end_element))
		{
			case 0:
			{
				#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
					if PLF_STACK_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++top_element, element);
					}
					else
				#endif
				{
					try
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++top_element, element);
					}
					catch (...)
					{
						--top_element;
						throw;
					}
				}

				++total_number_of_elements;
				return;
			}
			case 1:
			{
				if (current_group->next_group == NULL)
				{
					current_group->next_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group);

					try
					{
						#ifdef PLF_STACK_VARIADICS_SUPPORT
							PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group);
						#else
							PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, group((total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group));
						#endif
					}
					catch (...)
					{
						PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1);
						current_group->next_group = NULL;
						throw;
					}
				}

				current_group = current_group->next_group;
				start_element = top_element = current_group->elements;

				#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
					if PLF_STACK_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, element);
					}
					else
				#endif
				{
					try
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, element);
					}
					catch (...)
					{
						current_group = current_group->previous_group;
						start_element = current_group->elements;
						top_element = current_group->end;
						throw;
					}
				}


				end_element = current_group->end;
				++total_number_of_elements;
				return;
			}
			case 2: // ie. empty stack, must initialize
			{
				initialize();

				#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
					if PLF_STACK_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, element);
					}
					else
				#endif
				{
					try
					{
						PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, element);
					}
					catch (...)
					{
						clear();
						throw;
					}
				}


				++total_number_of_elements;
				return;
			}
		}
	}



	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		// Note: the reason for code duplication from non-move push, as opposed to using std::forward for both, was because most compilers didn't actually create as-optimal code in that strategy. Also C++03 compatibility.
		void push(element_type &&element)
		{
			switch ((top_element == NULL) + (top_element == end_element))
			{
				case 0:
				{
					#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
						if PLF_STACK_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++top_element, std::move(element));
						}
						else
					#endif
					{
						try
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++top_element, std::move(element));
						}
						catch (...)
						{
							--top_element;
							throw;
						}
					}

					++total_number_of_elements;
					return;
				}
				case 1:
				{
					if (current_group->next_group == NULL)
					{
						current_group->next_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group);

						try
						{
							#ifdef PLF_STACK_VARIADICS_SUPPORT
								PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group);
							#else
								PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, group((total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group));
							#endif
						}
						catch (...)
						{
							PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1);
							current_group->next_group = NULL;
							throw;
						}
					}

					current_group = current_group->next_group;
					start_element = top_element = current_group->elements;

					#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
						if PLF_STACK_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::move(element));
						}
						else
					#endif
					{
						try
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::move(element));
						}
						catch (...)
						{
							current_group = current_group->previous_group;
							start_element = current_group->elements;
							top_element = current_group->end;
							throw;
						}
					}


					end_element = current_group->end;
					++total_number_of_elements;
					return;
				}
				case 2: // ie. empty stack, must initialize
				{
					initialize();

					#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
						if PLF_STACK_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::move(element));
						}
						else
					#endif
					{
						try
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::move(element));
						}
						catch (...)
						{
							clear();
							throw;
						}
					}


					++total_number_of_elements;
					return;
				}
			}
		}
	#endif




	#ifdef PLF_STACK_VARIADICS_SUPPORT
		template<typename... arguments>
		void emplace(arguments &&... parameters)
		{
			switch ((top_element == NULL) + (top_element == end_element))
			{
				case 0:
				{
					#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
						if PLF_STACK_CONSTEXPR (std::is_nothrow_constructible<element_type, arguments ...>::value)
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++top_element, std::forward<arguments>(parameters)...);
						}
						else
					#endif
					{
						try
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), ++top_element, std::forward<arguments>(parameters)...);
						}
						catch (...)
						{
							--top_element;
							throw;
						}
					}

					++total_number_of_elements;
					return;
				}
				case 1:
				{
					if (current_group->next_group == NULL)
					{
						current_group->next_group = PLF_STACK_ALLOCATE(group_allocator_type, group_allocator_pair, 1, current_group);

						try
						{
							PLF_STACK_CONSTRUCT(group_allocator_type, group_allocator_pair, current_group->next_group, (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group, current_group);
						}
						catch (...)
						{
							PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, current_group->next_group, 1);
							current_group->next_group = NULL;
							throw;
						}
					}

					current_group = current_group->next_group;
					start_element = top_element = current_group->elements;

					#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
						if PLF_STACK_CONSTEXPR (std::is_nothrow_constructible<element_type, arguments ...>::value)
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::forward<arguments>(parameters)...);
						}
						else
					#endif
					{
						try
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::forward<arguments>(parameters)...);
						}
						catch (...)
						{
							current_group = current_group->previous_group;
							start_element = current_group->elements;
							top_element = current_group->end;
							throw;
						}
					}


					end_element = current_group->end;
					++total_number_of_elements;
					return;
				}
				case 2: // ie. empty stack, must initialize
				{
					initialize();

					#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
						if PLF_STACK_CONSTEXPR (std::is_nothrow_constructible<element_type, arguments ...>::value)
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::forward<arguments>(parameters)...);
						}
						else
					#endif
					{
						try
						{
							PLF_STACK_CONSTRUCT(element_allocator_type, (*this), top_element, std::forward<arguments>(parameters)...);
						}
						catch (...)
						{
							clear();
							throw;
						}
					}

					++total_number_of_elements;
					return;
				}
			}
		}


	#endif



	inline PLF_STACK_FORCE_INLINE reference top() const // Exception may occur if stack is empty in release mode
	{
		assert(!empty());
		return *top_element;
	}



	void pop() // Exception will occur if stack is empty
	{
		assert(!empty());

		#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
			if PLF_STACK_CONSTEXPR (!(std::is_trivially_destructible<element_type>::value))
		#endif
		{
			PLF_STACK_DESTROY(element_allocator_type, (*this), top_element);
		}

		// ie. if total_number_of_elements != 0 after decrement, or we were not already at the start of a non-first group
		if (total_number_of_elements-- == 1 || top_element != start_element) // If total_number_of_elements is now 0 after decrement, this essentially moves top_element back to it's initial position (start_element - 1). But otherwise, this is just a regular pop
		{
			--top_element;
		}
		else
		{ // ie. is start element, but not first group in stack (if it were, total_number_of_elements would be 0 after decrement)
			current_group = current_group->previous_group;
			start_element = current_group->elements;
			end_element = top_element = current_group->end;
		}
	}



	inline stack & operator = (const stack &source)
	{
		assert(&source != this);

		destroy_all_data();
		stack temp(source);

		#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		stack & operator = (stack &&source) PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT(allocator_type)
		{
			assert (&source != this);

			destroy_all_data();

			#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
				if PLF_STACK_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value)
				{
					std::memcpy(static_cast<void *>(this), &source, sizeof(stack));
				}
				else
			#endif
			{
				current_group = std::move(source.current_group);
				first_group = std::move(source.first_group);
				top_element = std::move(source.top_element);
				start_element = std::move(source.start_element);
				end_element = std::move(source.end_element);
				total_number_of_elements = source.total_number_of_elements;
				min_elements_per_group = source.min_elements_per_group;
				group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
			}

			source.blank();
			return *this;
		}
	#endif



	inline PLF_STACK_FORCE_INLINE bool empty() const PLF_STACK_NOEXCEPT
	{
		return total_number_of_elements == 0;
	}



	inline PLF_STACK_FORCE_INLINE size_type size() const PLF_STACK_NOEXCEPT
	{
		return total_number_of_elements;
	}



	inline size_type max_size() const PLF_STACK_NOEXCEPT
	{
		#ifdef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
      	return std::allocator_traits<element_allocator_type>::max_size(*this);
		#else
      	return element_allocator_type::max_size();
      #endif
	}



	size_type capacity() const PLF_STACK_NOEXCEPT
	{
		size_type total_size = 0;
		group_pointer_type temp_group = first_group;

		while (temp_group != NULL)
		{
			total_size += static_cast<size_type>((temp_group->end + 1) - temp_group->elements);
			temp_group = temp_group->next_group;
		}

		return total_size;
	}



	size_type approximate_memory_use() const PLF_STACK_NOEXCEPT
	{
		size_type memory_use = sizeof(*this);
		group_pointer_type temp_group = first_group;

		while (temp_group != NULL)
		{
			memory_use += static_cast<size_type>((((temp_group->end + 1) - temp_group->elements) * sizeof(value_type)) + sizeof(group));
			temp_group = temp_group->next_group;
		}

		return memory_use;
	}



private:

	#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
		void move_from_source(stack &source)
		{
			assert(&source != this);

			if (total_number_of_elements == 0)
			{
				return;
			}
	
			group_pointer_type current_copy_group = source.first_group;
			const group_pointer_type end_copy_group = source.current_group;
	
			if (total_number_of_elements <= group_allocator_pair.max_elements_per_group) // most common case
			{
				min_elements_per_group = total_number_of_elements;
				initialize();
				min_elements_per_group = source.min_elements_per_group;

				// Copy groups to this stack:
				while (current_copy_group != end_copy_group)
				{
					std::uninitialized_copy(std::make_move_iterator(current_copy_group->elements), std::make_move_iterator(current_copy_group->end + 1), top_element);
					top_element += (current_copy_group->end + 1) - current_copy_group->elements;
					current_copy_group = current_copy_group->next_group;
				}

				// Handle special case of last group:
				std::uninitialized_copy(std::make_move_iterator(source.start_element), std::make_move_iterator(source.top_element + 1), top_element);
				end_element = (top_element += (source.top_element - source.start_element)); // This should make top_element == the last "pushed" element, rather than the one past it
			}
			else // uncommon edge case, so not optimising:
			{
				min_elements_per_group = group_allocator_pair.max_elements_per_group;
				total_number_of_elements = 0;
				initialize();

				while (current_copy_group != end_copy_group)
				{
					for (element_pointer_type element_to_copy = current_copy_group->elements; element_to_copy != current_copy_group->end + 1; ++element_to_copy)
					{
						push(std::move(*element_to_copy));
					}

					current_copy_group = current_copy_group->next_group;
				}

				// Handle special case of last group:
				for (element_pointer_type element_to_copy = source.start_element; element_to_copy != source.top_element + 1; ++element_to_copy)
				{
					push(std::move(*element_to_copy));
				}

				min_elements_per_group = source.min_elements_per_group;
			}
		}
	#endif



	inline void consolidate()
	{
		#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
			stack temp(((min_elements_per_group > total_number_of_elements) ? min_elements_per_group : ((total_number_of_elements > group_allocator_pair.max_elements_per_group) ? group_allocator_pair.max_elements_per_group : total_number_of_elements)), group_allocator_pair.max_elements_per_group); // Make first allocated group as large total number of elements, where possible
			temp.total_number_of_elements = total_number_of_elements;

			#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
				if PLF_STACK_CONSTEXPR (std::is_move_assignable<element_type>::value && std::is_move_constructible<element_type>::value)
				{
					temp.move_from_source(*this);
				}
				else
			#endif
			{
				temp.copy_from_source(*this);
			}

			temp.min_elements_per_group = min_elements_per_group; // reset to correct value for future clear() or erasures
			*this = std::move(temp);
		#else
			stack temp(*this);
			swap(temp);
		#endif
	}



public:


	void change_group_sizes(const size_type min_allocation_amount, const size_type max_allocation_amount)
	{
		assert(min_allocation_amount > 2);
		assert(min_allocation_amount <= max_allocation_amount);
		assert(max_allocation_amount <= std::numeric_limits<size_type>::max() / 2);

		min_elements_per_group = min_allocation_amount;
		group_allocator_pair.max_elements_per_group = max_allocation_amount;
		free_unused_memory();

		if (first_group != NULL && (static_cast<size_type>((first_group->end + 1) - first_group->elements) < min_allocation_amount || static_cast<size_type>((current_group->end + 1) - current_group->elements) > max_allocation_amount))
		{
			consolidate();
		}
	}



	inline void change_minimum_group_size(const size_type min_allocation_amount)
	{
		change_group_sizes(min_allocation_amount, group_allocator_pair.max_elements_per_group);
	}



	inline void change_maximum_group_size(const size_type max_allocation_amount)
	{
		change_group_sizes(min_elements_per_group, max_allocation_amount);
	}


private:

	inline void blank() PLF_STACK_NOEXCEPT
	{
		#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
			if PLF_STACK_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value) // if all pointer types are trivial, we can just nuke it from orbit with memset (NULL is always 0 in C++):
			{
				std::memset(static_cast<void *>(this), 0, offsetof(stack, min_elements_per_group));
			}
			else
		#endif
		{
			current_group = NULL;
			first_group = NULL;
			top_element = NULL;
			start_element = NULL;
			end_element = NULL;
			total_number_of_elements = 0;
		}
	}


public:

	void clear() PLF_STACK_NOEXCEPT
	{
		destroy_all_data();
		blank();
	}



	bool operator == (const stack &rh) const PLF_STACK_NOEXCEPT
	{
		assert (this != &rh);

		if (total_number_of_elements != rh.total_number_of_elements)
		{
			return false;
		}
		else if (total_number_of_elements == 0 && rh.total_number_of_elements == 0)
		{
			return true;
		}

		group_pointer_type this_group = first_group, rh_group = rh.first_group;
		element_pointer_type this_pointer = first_group->elements, rh_pointer = rh.first_group->elements;

		while(true)
		{
			if (*this_pointer != *rh_pointer)
			{
				return false;
			}

			if (this_pointer == top_element)
			{
				break;
			}

			if (this_pointer++ == this_group->end) // incrementing in the more common case where this is not true - combining the equality test and increment usually compiles into a single instruction
			{
				this_group = this_group->next_group;
				this_pointer = this_group->elements;
			}

			if (rh_pointer++ == rh_group->end)
			{
				rh_group = rh_group->next_group;
				rh_pointer = rh_group->elements;
			}
		}

		return true;
	}



	inline bool operator != (const stack &rh) const PLF_STACK_NOEXCEPT
	{
		return !(*this == rh);
	}



	// Remove trailing stack groups (not removed in general 'pop' usage for performance reasons)
	void free_unused_memory() PLF_STACK_NOEXCEPT
	{
		if (current_group == NULL) // ie. stack is empty
		{
			return;
		}

		group_pointer_type temp_group = current_group->next_group;
		current_group->next_group = NULL; // Set to NULL regardless of whether it is already NULL (avoids branching). Cuts off rest of groups from this group.

		while (temp_group != NULL)
		{
			const group_pointer_type next_group = temp_group->next_group;
			PLF_STACK_DESTROY(group_allocator_type, group_allocator_pair, temp_group);
			PLF_STACK_DEALLOCATE(group_allocator_type, group_allocator_pair, temp_group, 1);
			temp_group = next_group;
		}
	}



	void shrink_to_fit()
	{
		if (first_group == NULL || total_number_of_elements == capacity())
		{
			return;
		}
		else if (total_number_of_elements == 0) // Edge case
		{
			clear();
			return;
		}

		const size_type original_min_elements = min_elements_per_group;
		min_elements_per_group = (total_number_of_elements < group_allocator_pair.max_elements_per_group) ? total_number_of_elements : group_allocator_pair.max_elements_per_group;
		min_elements_per_group = (min_elements_per_group < 3) ? 3 : min_elements_per_group;
		consolidate();
		min_elements_per_group = original_min_elements;
	}



	void reserve(size_type reserve_amount)
	{
		assert(reserve_amount > 2);

		if (reserve_amount > group_allocator_pair.max_elements_per_group)
		{
			reserve_amount = group_allocator_pair.max_elements_per_group;
		}
		else if (reserve_amount < min_elements_per_group)
		{
			reserve_amount = min_elements_per_group;
		}
		else if (reserve_amount > max_size())
		{
			reserve_amount = max_size();
		}

		if (first_group == NULL) // If this is a newly-created stack, no pushes yet
		{
			const size_type original_min_elements = min_elements_per_group;
			min_elements_per_group = reserve_amount;
			initialize();
			min_elements_per_group = original_min_elements;
		}
		else if (reserve_amount <= capacity())
		{
			return;
		}
		else
		{
			// Reallocate all data:
			const size_type original_min_elements = min_elements_per_group;
			min_elements_per_group = reserve_amount;
			consolidate();
			min_elements_per_group = original_min_elements;
		}
	}



    inline allocator_type get_allocator() const PLF_STACK_NOEXCEPT
    {
		return element_allocator_type();
	}



	void append(stack &source) PLF_STACK_NOEXCEPT_SWAP(element_allocator_type)
	{
		// Process: if there are unused memory spaces at the end of the last current back group of the chain, fill those up
		// with elements from the source back group. Then link the destination stack's groups to the source stack's groups.

		if (source.total_number_of_elements == 0)
		{
			return;
		}
		else if (total_number_of_elements == 0)
		{
			#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
				*this = std::move(source);
			#else
				destroy_all_data();
				swap(source);
			#endif

			source.current_group = NULL;
			source.top_element = NULL;
			source.start_element = NULL;
			source.end_element = NULL;
			return;
		}


		total_number_of_elements += source.total_number_of_elements;


		// Fill up the last group in the destination with pointers from the source:
		size_type elements_to_be_transferred = static_cast<size_type>(end_element - top_element++);
		size_type available_to_be_transferred = static_cast<size_type>((source.top_element - source.start_element) + 1);

		while (elements_to_be_transferred >= available_to_be_transferred)
		{
			// Use the fastest method for moving elements, while preserving values if allocator provides non-trivial pointers:
			#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
				if PLF_STACK_CONSTEXPR (std::is_trivially_copyable<element_type>::value && std::is_trivially_destructible<element_type>::value)
				{
					std::memcpy(static_cast<void *>(&*top_element), static_cast<void *>(&*source.start_element), available_to_be_transferred * sizeof(element_type));
				}
				#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
					else if PLF_STACK_CONSTEXPR (std::is_move_constructible<element_type>::value)
					{
						std::uninitialized_copy(std::make_move_iterator(source.start_element), std::make_move_iterator(source.top_element + 1), top_element);
					}
				#endif
				else
			#endif
			{
				std::uninitialized_copy(source.start_element, source.top_element + 1, top_element);

				for (element_pointer_type element_pointer = source.start_element; element_pointer != source.top_element + 1; ++element_pointer)
				{
					PLF_STACK_DESTROY(element_allocator_type, source, element_pointer);
				}
			}

			top_element += available_to_be_transferred;

			if (source.current_group == source.first_group)
			{
				--top_element;
				source.clear();
				return;
			}

			elements_to_be_transferred -= available_to_be_transferred;
			source.current_group = source.current_group->previous_group;
			source.start_element = source.current_group->elements;
			source.end_element = source.top_element = source.current_group->end;
			available_to_be_transferred = static_cast<size_type>((source.top_element - source.start_element) + 1);
		}


		if (elements_to_be_transferred != 0)
		{
			element_pointer_type const start = source.top_element - (elements_to_be_transferred - 1);

			#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
				if PLF_STACK_CONSTEXPR (std::is_trivially_copyable<element_type>::value && std::is_trivially_destructible<element_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
				{
					std::memcpy(static_cast<void *>(&*top_element), static_cast<void *>(&*start), elements_to_be_transferred * sizeof(element_type));
				}
				#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT
					else if PLF_STACK_CONSTEXPR (std::is_move_constructible<element_type>::value)
					{
						std::uninitialized_copy(std::make_move_iterator(start), std::make_move_iterator(source.top_element + 1), top_element);
					}
				#endif
				else
			#endif
			{
				std::uninitialized_copy(start, source.top_element + 1, top_element);

				// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
				for (element_pointer_type element_pointer = start; element_pointer != source.top_element + 1; ++element_pointer)
				{
					PLF_STACK_DESTROY(element_allocator_type, source, element_pointer);
				}
			}

			source.top_element = start - 1;
		}

		// Trim trailing groups on both, link source and destinations groups and remove references to source groups from source:
		source.free_unused_memory();
        free_unused_memory();


		current_group->next_group = source.first_group;
		source.first_group->previous_group = current_group;

		current_group = source.current_group;
		top_element = source.top_element;
		start_element = source.start_element;
		end_element = source.end_element;

		// Correct group sizes if necessary:
		if (source.min_elements_per_group < min_elements_per_group)
		{
			min_elements_per_group = source.min_elements_per_group;
		}

		if (source.group_allocator_pair.max_elements_per_group < group_allocator_pair.max_elements_per_group)
		{
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;
		}

		source.blank();
	}



	void swap(stack &source) PLF_STACK_NOEXCEPT_SWAP(element_allocator_type)
	{
		#ifdef PLF_STACK_TYPE_TRAITS_SUPPORT
			if PLF_STACK_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value) // if all pointer types are trivial we can just copy using memcpy - faster
			{
				char temp[sizeof(stack)];
				std::memcpy(&temp, static_cast<void *>(this), sizeof(stack));
				std::memcpy(static_cast<void *>(this), static_cast<void *>(&source), sizeof(stack));
				std::memcpy(static_cast<void *>(&source), &temp, sizeof(stack));
			}
			#ifdef PLF_STACK_MOVE_SEMANTICS_SUPPORT // If pointer types are not trivial, moving them is probably going to be more efficient than copying them below
				else if PLF_STACK_CONSTEXPR (std::is_move_assignable<group_pointer_type>::value && std::is_move_assignable<element_pointer_type>::value && std::is_move_constructible<group_pointer_type>::value && std::is_move_constructible<element_pointer_type>::value)
				{
					stack temp(std::move(source));
					source = std::move(*this);
					*this = std::move(temp);
				}
			#endif
			else
		#endif
		{
			const group_pointer_type	swap_current_group = current_group, swap_first_group = first_group;
			const element_pointer_type	swap_top_element = top_element, swap_start_element = start_element, swap_end_element = end_element;
			const size_type				swap_total_number_of_elements = total_number_of_elements, swap_min_elements_per_group = min_elements_per_group, swap_max_elements_per_group = group_allocator_pair.max_elements_per_group;

			current_group = source.current_group;
			first_group = source.first_group;
			top_element = source.top_element;
			start_element = source.start_element;
			end_element = source.end_element;
			total_number_of_elements = source.total_number_of_elements;
			min_elements_per_group = source.min_elements_per_group;
			group_allocator_pair.max_elements_per_group = source.group_allocator_pair.max_elements_per_group;

			source.current_group = swap_current_group;
			source.first_group = swap_first_group;
			source.top_element = swap_top_element;
			source.start_element = swap_start_element;
			source.end_element = swap_end_element;
			source.total_number_of_elements = swap_total_number_of_elements;
			source.min_elements_per_group = swap_min_elements_per_group;
			source.group_allocator_pair.max_elements_per_group = swap_max_elements_per_group;
		}
	}


}; // stack



template <class element_type, class element_allocator_type>
inline void swap (stack<element_type, element_allocator_type> &a, stack<element_type, element_allocator_type> &b) PLF_STACK_NOEXCEPT_SWAP(element_allocator_type)
{
	a.swap(b);
}




} // plf namespace

#undef PLF_STACK_FORCE_INLINE

#undef PLF_STACK_TYPE_TRAITS_SUPPORT
#undef PLF_STACK_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_STACK_VARIADICS_SUPPORT
#undef PLF_STACK_MOVE_SEMANTICS_SUPPORT
#undef PLF_STACK_NOEXCEPT
#undef PLF_STACK_NOEXCEPT_SWAP
#undef PLF_STACK_NOEXCEPT_MOVE_ASSIGNMENT
#undef PLF_STACK_CONSTEXPR

#undef PLF_STACK_CONSTRUCT
#undef PLF_STACK_DESTROY
#undef PLF_STACK_ALLOCATE
#undef PLF_STACK_ALLOCATE_INITIALIZATION
#undef PLF_STACK_DEALLOCATE


#endif // PLF_STACK_H
