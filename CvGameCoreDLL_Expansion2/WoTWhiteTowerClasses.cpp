
#include "CvGameCoreDLLPCH.h"
#include "WoTWhiteTowerClasses.h"
#include "CvMinorCivAI.h"
#include "WoTSerialization.h"

//======================================================================================================
//					WoTWhiteTowerAjahInfo
//======================================================================================================

WoTWhiteTowerAjahInfo::WoTWhiteTowerAjahInfo() :
	m_iDefaultColor(-1)
{

}
WoTWhiteTowerAjahInfo::~WoTWhiteTowerAjahInfo()
{

}
bool WoTWhiteTowerAjahInfo::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	const char* szTextVal = kResults.GetText("Color");
	m_iDefaultColor = GC.getInfoTypeForString(szTextVal, true);

	return true;
}
int WoTWhiteTowerAjahInfo::GetColorType() const
{
	return m_iDefaultColor;
}

//======================================================================================================
//					WoTMinorCivAjahs
//======================================================================================================

WoTMinorCivAjahs::WoTMinorCivAjahs() :
	m_eMajorityAjah(NO_AJAH),
	m_piAjahInfluences(NULL)
{

}

WoTMinorCivAjahs::~WoTMinorCivAjahs()
{
	Uninit();
}

void WoTMinorCivAjahs::Write(FDataStream& kStream)
{
	uint uiVersion = 0;
	kStream << uiVersion;

	kStream << m_eMajorityAjah;

	kStream << ArrayWrapper<int>(GC.GetNumWhiteTowerAjahInfos(), m_piAjahInfluences);
}

void WoTMinorCivAjahs::Read(FDataStream& kStream)
{
	uint uiVersion;

	kStream >> uiVersion;

	kStream >> m_eMajorityAjah;

	kStream >> ArrayWrapper<int>(GC.GetNumWhiteTowerAjahInfos(), m_piAjahInfluences);
}

void WoTMinorCivAjahs::Uninit()
{
	SAFE_DELETE_ARRAY(m_piAjahInfluences);
}

void WoTMinorCivAjahs::Init(CvMinorCivAI* pOwner)
{
	m_pOwner = pOwner;

	m_piAjahInfluences = FNEW(int[GC.GetNumWhiteTowerAjahInfos()], c_eCiv5GameplayDLL, 0);

	Reset();

	CvMinorCivInfo* pOwnerInfo = GC.getMinorCivInfo(m_pOwner->GetMinorCivType());

	for (int i = 0; i < GC.GetNumWhiteTowerAjahInfos(); i++)
	{
		AjahTypes eAjah = static_cast<AjahTypes>(i);
		m_piAjahInfluences[i] = pOwnerInfo->GetAjahStartingInfluence(eAjah);
	}
}

void WoTMinorCivAjahs::Reset()
{
	m_eMajorityAjah = NO_AJAH;

	for (int i = 0; i < GC.GetNumWhiteTowerAjahInfos(); i++)
	{
		m_piAjahInfluences = 0;
	}
}

AjahTypes WoTMinorCivAjahs::GetMajorityAjah() const
{
	return m_eMajorityAjah;
}

int WoTMinorCivAjahs::GetAjahInfluence(AjahTypes eAjah) const
{
	CvAssertMsg(eAjah < GC.GetNumWhiteTowerAjahInfos(), "Index out of bounds");
	CvAssertMsg(eAjah > NO_AJAH, "Index out of bounds");
	return m_piAjahInfluences[eAjah];
}

int WoTMinorCivAjahs::GetAjahInfluenceTimes100(AjahTypes eAjah) const
{
	return GetAjahInfluence(eAjah) * 100;
}

void WoTMinorCivAjahs::SetAjahInfluence(AjahTypes eAjah, int iNewInfluence)
{
	CvAssertMsg(eAjah < GC.GetNumWhiteTowerAjahInfos(), "Index out of bounds");
	CvAssertMsg(eAjah > NO_AJAH, "Index out of bounds");
	m_piAjahInfluences[eAjah] = iNewInfluence;
}

void WoTMinorCivAjahs::ChangeAjahInfluence(AjahTypes eAjah, int iChange)
{
	SetAjahInfluence(eAjah, GetAjahInfluence(eAjah) + iChange);
}

int WoTMinorCivAjahs::GetAjahInfluencePercent(AjahTypes eAjah) const
{
	int iAjahInfluence = GetAjahInfluenceTimes100(eAjah);
	int iInfluenceTotal = 0;
	for (int i = 0; i < GC.GetNumWhiteTowerAjahInfos(); i++)
	{
		AjahTypes eLoopAjah = static_cast<AjahTypes>(i);
		iInfluenceTotal += max(0, GetAjahInfluenceTimes100(eLoopAjah));
	}

	return iAjahInfluence / iInfluenceTotal;
}