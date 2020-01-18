// TUN_Scale.h: Interface of the class CSingleScale.
//
// (C)opyright in 2003-2009 by Mark Henning, Germany
//
// Implementation of AnaMark Tuning File Format V2.00 (*.TUN)
//
// Read TUN_Scale.cpp for more informations about this class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISCALEFILE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_)
#define AFX_MULTISCALEFILE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_





#pragma warning( disable : 4786 )

#include "TUN_Scale.h"





namespace TUN
{





class CMultiScaleFile
{
public:
	CMultiScaleFile() {}
	virtual ~CMultiScaleFile() {}



	// Error handling
	const CErr &	Err() const { return m_err; }
private:
	CErr	m_err;
public:



	// Write the list of scales to the file/stream
	bool Write(const char * szFilepath, long lVersionFrom = 0, long lVersionTo = 200)
	{
		std::ofstream	ofs(szFilepath, std::ios_base::out | std::ios_base::trunc);
		return Write(ofs, lVersionFrom, lVersionTo);
	}


	bool Write(std::ostream & os, long lVersionFrom = 0, long lVersionTo = 200)
	{
		os << ";" << std::endl;
		os << "; This is an AnaMark multiple scales file" << std::endl;
		os << "; (based on AnaMark tuning map file V2.00)" << std::endl;
		os << ";" << std::endl;
		os << "; Free .MSF and .TUN file handling source code (C)2009 by Mark Henning, Germany" << std::endl;
		os << ";" << std::endl;
		os << "; Specifications and free source code see:" << std::endl;
		os << ";         " << CSingleScale::FormatSpecs() << std::endl;
		os << ";" << std::endl;

		std::list<CSingleScale>::iterator	it;
		for ( it = m_lssScales.begin() ; it != m_lssScales.end() ; ++it )
		{
			os << std::endl;
			os << std::endl;
			os << std::endl;
			os << ";======================================================================" << std::endl;
			os << ";======================================================================" << std::endl;
			os << ";======================================================================" << std::endl;
			os << std::endl;
			if ( !it->Write(os, lVersionFrom, lVersionTo, false) )
				return m_err.SetError(it->Err().GetLastError().c_str());
		}
		return m_err.SetOK();
	}



	// Add scales from the file/stream to the list
	// -1 = an error occurred
	// otherwise: Number of scale datasets found
	long	Add(const char * szFilepath)
	{
		// Open the file
		std::ifstream	ifstr(szFilepath, std::ios_base::in | std::ios_base::binary);

		if ( !ifstr )
			return m_err.SetError("Error opening the file.");

		// String which will receive the current line from the file
		CStringParser	strparser;
		strparser.InitStreamReading();

		// Read the file
		long	lResult = Add(ifstr, strparser);

		// Close the file
		ifstr.close();

		return lResult;
	}


	long	Add(std::istream & istr, CStringParser & strparser)
	{
		long	lResult = 0;
		while (true)
		{
			CSingleScale	SS;
			switch ( SS.Read(istr, strparser) )
			{
			case 0:		return lResult; // No more scales in the file
			case 1:		m_lssScales.push_back(SS); ++lResult; break;
			default:	return -1; // An error occurred
			}
		}
	}



	// Find Scale which applies to the given MIDI Channel
	// returns NULL, if there is no scale applicable
	CSingleScale *	Find(long lMIDIChannel)
	{
		std::list<CSingleScale>::iterator	it;
		for ( it = m_lssScales.begin() ; it != m_lssScales.end() ; ++it )
		{
			if ( it->AppliesToChannel(lMIDIChannel) )
				return &(*it);
		}
		return NULL;
	}



	// Direct access to the list of scales for whatever you whish to do with it:
	std::list<CSingleScale>	m_lssScales;
};





} // namespace TUN





#endif // !defined(AFX_MULTISCALEFILE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_)

