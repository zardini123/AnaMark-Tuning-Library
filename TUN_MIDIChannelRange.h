// TUN_MIDIChannelRange.h: Interface of the class CMIDIChannelRange.
//
// (C)opyright in 2003-2009 by Mark Henning, Germany
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIDICHANNELRANGE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_)
#define AFX_MIDICHANNELRANGE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_





#pragma warning( disable : 4786 )

#include <string>

#include "TUN_StringTools.h"





namespace TUN
{





class CMIDIChannelRange
{
public:
	CMIDIChannelRange() { Reset(); }
	virtual ~CMIDIChannelRange() {}


	void Reset()
	{
		m_lFrom = m_lTo = 0;
	}


	// Sets range by a string which can have the syntax "#" or "#-#"
	// whereas '#' denotes a positive integer number
	bool SetFromStr(std::string str)
	{
		Reset();

		std::string::size_type	pos = 0;
		long					lFrom;
		long					lTo;
		if ( !strx::Eval(strx::RemoveSpaces(str), pos, lFrom) )
			return false;
		if ( pos == str.size() )
			return Set(lFrom);
		else
		{
			if ( str.at(pos++) != '-' )
				return false; // '-' missing
			if ( !strx::Eval(str, pos, lTo) )
				return false;
			return Set(lFrom, lTo);
		}
	}


	bool Set(long lChannel)
	{
		return Set(lChannel, lChannel);
	}

	bool Set(long lFrom, long lTo)
	{
		if ( (lFrom > 0) && (lTo > 0) && (lFrom <= lTo) && (lTo <= 65535) )
		{
			m_lFrom = lFrom;
			m_lTo = lTo;
			return true;
		}
		else
		{
			Reset();
			return false;
		}
	}


	std::string GetAsStr() const
	{
		std::string	str;
		str = strx::ltostr(m_lFrom);
		if ( m_lFrom != m_lTo )
			str += "-" + strx::ltostr(m_lTo);
		return str;
	}


	bool IsInside(long lChannel) const
	{
		return ( (lChannel >= m_lFrom) && (lChannel <= m_lTo) );
	}


	long GetFrom() const { return m_lFrom; }
	long GetTo() const { return m_lTo; }


private:
	long	m_lFrom;
	long	m_lTo;
};





} // namespace TUN





#endif // !defined(AFX_MIDICHANNELRANGE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_)
