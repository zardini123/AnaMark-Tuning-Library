// TUN_Scale.h: Interface of the class CFormula.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Implementation of formula handling to define note tuning
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMULA_H__C961C403_D327_4021_8ACD_DE1ABEC27066__INCLUDED_)
#define AFX_FORMULA_H__C961C403_D327_4021_8ACD_DE1ABEC27066__INCLUDED_







#include <cmath>

#include "TUN_Error.h"
#include "TUN_StringTools.h"




namespace TUN
{





// Common constants, defined in TUN_Scale.h and TUN_Scale.cpp
extern const long	MaxNumOfNotes;





// Handling of RV-parameters
// RV = given by *R*eference or *V*alue
struct SRVParam
{
	SRVParam() {}
	virtual ~SRVParam() {}


	// Parameter definition
	enum eRVParamType
	{
		t_Value,	// Parameter given as value
		t_AbsRef,	// Parameter given as absolute reference
		t_RelRef	// Parameter given as relative reference
	}			m_paramtype;

	long		m_lRef = 0;		// Reference (absolute or relative, depends on m_paramtype)
	double		m_dblValue = 0;	// Value (m_paramtype == t_Value)


	// Retrieve the resolved value (= references are resolved)
	double GetValue(std::vector<double> & vdblNoteFrequenciesHz, long scaleNoteNumber) const
	{
		switch ( m_paramtype )
		{
		case t_AbsRef:	return vdblNoteFrequenciesHz.at(m_lRef);
		case t_RelRef:	return vdblNoteFrequenciesHz.at(scaleNoteNumber + m_lRef);
		default:		return m_dblValue;
		}
	}


	// Set RVParameter from string with error checking
	bool SetFromStr(const std::string & str, std::string::size_type & pos)
	{
		switch ( str.at(pos) )
		{
		case '=':	m_paramtype = t_AbsRef;	return strx::Eval(str, ++pos, m_lRef);
		case '>':	m_paramtype = t_RelRef;	return strx::Eval(str, ++pos, m_lRef);
		default:	m_paramtype = t_Value;	return strx::Eval(str, pos, m_dblValue);
		}
	}


	// Convert RVParameter to string
	std::string GetAsStr() const
	{
		switch ( m_paramtype )
		{
		case t_AbsRef:	return "=" + strx::ltostr(m_lRef);
		case t_RelRef:	return ">" + strx::ltostr(m_lRef);
		default:		return strx::dtostr(m_dblValue);
		}
	}
};





// Handling of formulas
class CFormula
{
public:
	// At construction time, the note index, the formula refers to, must be given.
	// ATTENTION: The calling source code is responsible that lMyIndex is valid!
	// There is no error checking against it!
	CFormula(long lMyIndex) : m_lMyIndex(lMyIndex) {}
	virtual ~CFormula() {}

	long	GetMyIndex() const { return m_lMyIndex; }
	long	GetOpenLoopValue() const { return 999; } // Loop -999 or +999 means: Loop until the begin or end of the scale


	// Reset the formula so that it equals the notes frequency unchanged
	// by a relative reference to itself
	void Reset()
	{
		m_dblEnsureHz = 0;
		m_rvpRangeHz.m_paramtype = SRVParam::t_RelRef;
		m_rvpRangeHz.m_lRef = 0;
		m_dblMUL = 1;
		m_dblDIV = 1;
		m_dblCENTS = 0;
		m_rvpShiftHz.m_paramtype = SRVParam::t_Value;
		m_rvpShiftHz.m_dblValue = 0;
		m_lLoop = 0;
	}


	// Set formula to represent a cent value referred to a frequency.
	void SetToCentsWithFreq(double dblCents, double dblBaseFreqHz)
	{
		Reset();
		m_rvpRangeHz.m_paramtype = SRVParam::t_Value;
		m_rvpRangeHz.m_dblValue = dblBaseFreqHz;
		m_dblCENTS = dblCents;
	}


	// Set formula to represent a cent value referred absolutely to a note.
	void SetToCentsAbsRef(double dblCents, long lAbsRef = 0)
	{
		Reset();
		m_rvpRangeHz.m_paramtype = SRVParam::t_AbsRef;
		m_rvpRangeHz.m_lRef = lAbsRef;
		m_dblCENTS = dblCents;
	}


	// Set formula to represent a frequency value.
	void SetToHz(double dblHz)
	{
		Reset();
		m_rvpRangeHz.m_paramtype = SRVParam::t_Value;
		m_rvpRangeHz.m_dblValue = dblHz;
	}


	// Set formula to represent a frequency value.
	void SetToEnsureHz(double dblEnsureHz)
	{
		Reset();
		m_dblEnsureHz = dblEnsureHz;
	}


	// Set formula by given string
	// returns false on error
	bool SetFromStr(std::string strFormula)
	{
		Reset();

		strx::RemoveSpaces(strFormula);

		std::string::size_type	pos = 0;
		while ( pos < strFormula.size() )
		{
			switch ( strFormula.at(pos++) )
			{
			case '!': // Ensure_Hz
				// Ensure_Hz overwrites everything else in this formula!
				Reset();
				if ( !strx::Eval(strFormula, pos, m_dblEnsureHz) )
					return false;
				pos = strFormula.size();
				break;
			case '#': // f_Range_Hz
				if ( !m_rvpRangeHz.SetFromStr(strFormula, pos) )
					return false;
				break;
			case '*': // MUL
				if ( !strx::Eval(strFormula, pos, m_dblMUL) )
					return false;
				break;
			case '/': // DIV
				if ( !strx::Eval(strFormula, pos, m_dblDIV) )
					return false;
				break;
			case '%': // CENTS
				if ( !strx::Eval(strFormula, pos, m_dblCENTS) )
					return false;
				break;
			case '+': // f_Shift_Hz
				if ( !m_rvpShiftHz.SetFromStr(strFormula, pos) )
					return false;
				break;
			case '~': // Loop function
				if ( !strx::Eval(strFormula, pos, m_lLoop) )
					return false;
				if ( m_lMyIndex + m_lLoop < 0 )
					m_lLoop = -GetOpenLoopValue();
				if ( m_lMyIndex + m_lLoop >= MaxNumOfNotes )
					m_lLoop = GetOpenLoopValue();
				break;
			default: // Unknown -> syntax error
				return false;
			}
		}

		return true;
	}


	// Retrieves formula as string
	std::string	GetAsStr() const
	{
		// We only add the items which deviate from the default values to
		// keep the formula as short as possible
		// For better readability, a space is placed before each item.
		if ( m_dblEnsureHz > 0 )
			return "!" + strx::dtostr(m_dblEnsureHz);
		else
		{
			std::string	strFormula;

			if ( (m_rvpRangeHz.m_paramtype != SRVParam::t_RelRef) || (m_rvpRangeHz.m_lRef != 0) )
				strFormula += " #" + m_rvpRangeHz.GetAsStr();
			if ( m_dblMUL != 1 )
				strFormula += " *" + strx::dtostr(m_dblMUL);
			if ( m_dblDIV != 1 )
				strFormula += " /" + strx::dtostr(m_dblDIV);
			if ( m_dblCENTS != 0 )
				strFormula += " %" + strx::dtostr(m_dblCENTS);
			if ( (m_rvpShiftHz.m_paramtype != SRVParam::t_Value) || (m_rvpShiftHz.m_dblValue != 0) )
				strFormula += " +" + m_rvpShiftHz.GetAsStr();
			if ( m_lLoop != 0 )
				strFormula += " ~" + strx::ltostr(m_lLoop);

			return (strFormula.empty() ? strFormula : strFormula.substr(1)); // returns without leading space
		}
	}


	// Apply formula to vector of note frequencies
	void Apply(std::vector<double> & vdblNoteFrequenciesHz) const
	{
		// Gets the sign of the amount of times to loop
		// The sign will determine which direction to loop
		long nextLoopIndexAdder = ( m_lLoop >= 0 ? +1 : -1 );
		long loopIndexOffset = 0;
		do
		{
			// The current note index in regards to the entire scale
			long	scaleNoteIndex = m_lMyIndex + loopIndexOffset;
			// scaleNoteIndex can never be greater than MaxNumOfNotes, as vector
			// 		vdblNoteFrequenciesHz's size is of MaxNumOfNotes
			if ( (scaleNoteIndex < 0) || (scaleNoteIndex >= MaxNumOfNotes) )
				break;

			// Shift entire scale instead of setting it, as the ! token has been used
			if ( m_dblEnsureHz > 0 )
			{
				double	dblFactor = m_dblEnsureHz / vdblNoteFrequenciesHz.at(scaleNoteIndex);
				for ( long i = 0 ; i < MaxNumOfNotes ; ++i )
					vdblNoteFrequenciesHz.at(i) *= dblFactor;
			}
			else // ! token hasn't been used.
			{
				// First resolve RV-Parameters (token #)
				// If #= : Gets the frequency of another note in the scale,
				// 		where the index was a single number (Absolute reference)
				// If #> : Gets the frequency of another note in the scale,
				// 		where that index is the current note index offset by an integer
				// (Relative reference)
				double	dblRangeHz = m_rvpRangeHz.GetValue(vdblNoteFrequenciesHz, scaleNoteIndex);
				// Token +
				double	dblShiftHz = m_rvpShiftHz.GetValue(vdblNoteFrequenciesHz, scaleNoteIndex);

				// Calculate
				vdblNoteFrequenciesHz.at(scaleNoteIndex) =
					dblRangeHz * m_dblMUL / m_dblDIV * pow(2, m_dblCENTS/1200) + dblShiftHz;
			}
			loopIndexOffset += nextLoopIndexAdder;
		} while ( std::abs(loopIndexOffset) < std::abs(m_lLoop) );
	}

// Parameters of the formula
private:
	// The note index the formula is refering to
	// All formulas must be refering to a note number
	long		m_lMyIndex;
	// Ensure frequency in Hz at the note number this formula is associated with
	// 		(m_lMyIndex) by shifting the scale
	// Assigned from ! token
	double		m_dblEnsureHz;
	// Set using # token
	SRVParam	m_rvpRangeHz;
	double		m_dblMUL;
	double		m_dblDIV;
	double		m_dblCENTS;
	// Set using + token
	SRVParam	m_rvpShiftHz;
	// Loop number provided with ~ token
	// Number of times a function should be repeated
	long		m_lLoop;
};





} // namespace TUN





#endif // !defined(AFX_FORMULA_H__C961C403_D327_4021_8ACD_DE1ABEC27066__INCLUDED_)
