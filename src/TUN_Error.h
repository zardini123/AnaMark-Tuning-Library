// TUN_Error.h: Interface of class CErr.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// This class provides basic error handling.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TUN_ERROR_H__26166046_5A6B_488C_B226_71921493B2E2__INCLUDED_)
#define AFX_TUN_ERROR_H__26166046_5A6B_488C_B226_71921493B2E2__INCLUDED_





#include <string>





namespace TUN
{





class CErr
{
public:
	CErr()
	{
		SetOK();
	}



	virtual ~CErr()
	{
	}



	// Checks for OK
	bool	IsOK() const
	{
		return m_strErrorMsg.empty();
	}



	// Returns error message
	std::string	GetLastError() const
	{
		return m_strErrorMsg;
	}



	// Set the error message
	// SetError(NULL) works like SetOK() as it clears the error message and
	// returns true. If szErrorMsg != NULL, error message is set and
	// return value is false.
	bool	SetError(const char * szErrorMsg, long lLineNr = -1)
	{
		if ( szErrorMsg == NULL )
			m_strErrorMsg = "";
		else
		{
			//              012345
			char	sz[16] = "Line 1234567890"; // Enough for 32-Bit
			if ( lLineNr >= 0 )
				// CHANGED:  Removed ltoa usage
				// Original usage:  ... ltoa(lLineNr, &sz[5], 10) ...
				m_strErrorMsg = "Line " + std::to_string(lLineNr) + ": " + szErrorMsg;
			else
				m_strErrorMsg = szErrorMsg;
		}

		return IsOK();
	}

	bool	SetError(const CErr & err)
	{
		*this = err;
		return IsOK();
	}



	// returns always true
	bool	SetOK()
	{
		return SetError(NULL);
	}



private:
	std::string	m_strErrorMsg;
};





} // namespace TUN





#endif // !defined(AFX_TUN_ERROR_H__26166046_5A6B_488C_B226_71921493B2E2__INCLUDED_)
