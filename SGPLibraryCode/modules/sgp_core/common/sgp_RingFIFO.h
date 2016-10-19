#ifndef __SGP_RINGFIFO_HEADER__
#define __SGP_RINGFIFO_HEADER__


/* 
 * Simple Ring Buffer FIFO, no locking version 
 * Note that with only one concurrent reader and one concurrent writer, you don't need extra locking to use these functions. 
 */ 
//--------------------------------------------------------------
// IMPORTANT ATTENTION:: capacity MUST be power of two
template <class DataType>
class RingFIFO
{
public:
	unsigned int	m_in;			// writer pos
	unsigned int	m_out;			// reader pos
	unsigned int	m_size;			// capacity 
	DataType		*m_buffer;		// data


	// must call initialize after
	RingFIFO() : m_in(0), m_out(m_in), m_size(0), m_buffer(NULL)
	{
	}
	RingFIFO( unsigned int capacity ) : m_in(0), m_out(m_in), m_size(capacity), m_buffer(NULL)
	{
        m_buffer = new DataType[m_size];
	}

	virtual ~RingFIFO() 
	{
		if( m_buffer )
		{
			delete [] m_buffer;
			m_buffer = NULL;
		}
	}

	void initialize(unsigned int capacity)
    {
        m_in = 0;
        m_out = m_in;
        m_size = capacity;
        if(m_buffer)
        {
            delete [] m_buffer;
            m_buffer = NULL;
        }
        m_buffer = new DataType[m_size];
	}
	
	void clear()
    {
        m_in = 0;
        m_out = m_in;
    }

	inline unsigned int getSize() const
    {
        if( m_in >= m_out )
        {
            return m_in - m_out;
        }
        else
        {
            return m_size + m_in - m_out ;
        }
    }

	inline unsigned int getFreeSize() const
	{
		return m_size - getSize();
	}

	bool contains( DataType data )
	{
        if( m_in >= m_out )
        {
			for (unsigned int i = m_out; i < m_in; ++i)
				if (m_buffer[i] == data) return true;
		}
		else
		{
			for (unsigned int i = 0; i < m_in; ++i)
				if (m_buffer[i] == data) return true;
			for (unsigned int i = m_out; i < m_size; ++i)
				if (m_buffer[i] == data) return true;
		}
		return false;
	}

	// puts some data into the FIFO, return actual pushed number of value 
	// If this FIFO is full, return 0
	unsigned int push( DataType *pBuffer, unsigned int len ) 
	{ 
		unsigned int l; 
		len = jmin(len, m_size - m_in + m_out); 
		/* first put the data starting from fifo->in to buffer end */ 
		l = jmin(len, m_size - (m_in & (m_size - 1))); 
		memcpy(m_buffer + (m_in & (m_size - 1)), pBuffer, l*sizeof(DataType)); 
		/* then put the rest (if any) at the beginning of the buffer */ 
		memcpy(m_buffer, pBuffer + l, (len - l)*sizeof(DataType)); 
		m_in += len; 
		return len; 
	}

	// get some data into the FIFO, return actual poped number of value
	// If this FIFO is empty, return 0
	unsigned int pop( DataType *pBuffer, unsigned int len ) 
	{ 
		unsigned int l; 
		len = jmin(len, m_in - m_out); 
		/* first get the data from fifo->out until the end of the buffer */ 
		l = jmin(len, m_size - (m_out & (m_size - 1))); 
		memcpy(pBuffer, m_buffer + (m_out & (m_size - 1)), l*sizeof(DataType));
		/* then get the rest (if any) from the beginning of the buffer */ 
		memcpy(pBuffer + l, m_buffer, (len - l)*sizeof(DataType)); 
		m_out += len; 
		return len; 
	}
};

#endif