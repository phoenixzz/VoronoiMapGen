#ifndef __SGP_ARRAYSTACK_HEADER__
#define __SGP_ARRAYSTACK_HEADER__

#include "sgp_Array.h"


//==============================================================================
/**
    Holds a resizable stack of primitive or copy-by-value objects.
	Real Data stored in Array data structure

    Examples of arrays are: ArrayStack<int> or ArrayStack<MyClass*>

	@see Array
*/
template <typename ElementType,
          typename TypeOfCriticalSectionToUse = DummyCriticalSection>
class ArrayStack
{
private:
    typedef PARAMETER_TYPE (ElementType) ParameterType;
public:
	/** Creates an empty ArrayStack. */
    ArrayStack() noexcept
       : m_top (0)
    {
    }
	ArrayStack( int p_size )
    {
		m_data.ensureStorageAllocated( p_size );
        m_top = 0;
    }

	~ArrayStack()
	{
	}

	bool Push( ParameterType p_data )
    {
        if( m_data.size() != m_top )
        {
            m_data.setUnchecked(m_top, p_data);
            m_top++;
            return true;
        }
        return false;
    }
    void Pop()
    {
        if( m_top > 0 )
            m_top--;
    }

    inline int Count() const
    {
        return m_top;
    }

	inline ElementType Top() const
    {
        return m_data.getUnchecked(m_top - 1);
    }
	inline ElementType& getReferenceTop() const
	{
		return m_data.getReference(m_top - 1);
	}

	inline void Clear()
	{
		m_data.clear();
		m_top = 0;
	}

	inline void Resize( int p_size )
    {
        if( p_size < m_top )
        {
            m_top = p_size;
        }
        m_data.resize( p_size );
    }

	inline int Size() const
	{
		return m_data.size();
	}

	inline ElementType& getReference( const int index ) const
	{
		return m_data.getReference(index);
	}

private:
	int m_top;
	Array<ElementType, TypeOfCriticalSectionToUse> m_data;
};

#endif