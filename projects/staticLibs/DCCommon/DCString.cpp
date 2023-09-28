#include "DCString.h"
#include <format>

void DCString::appendDouble(double dValue, unsigned int uiNumDecimalPoints)
{
	switch (uiNumDecimalPoints)
	{
	case 0:
		*this += std::format("{:.0f}", dValue);
		break;
	case 1:
		*this += std::format("{:.1f}", dValue);
		break;
	case 2:
		*this += std::format("{:.2f}", dValue);
		break;
	case 3:
		*this += std::format("{:.3f}", dValue);
		break;
	case 4:
		*this += std::format("{:.4f}", dValue);
		break;
	case 5:
		*this += std::format("{:.5f}", dValue);
		break;
	case 6:
		*this += std::format("{:.6f}", dValue);
		break;
	case 7:
		*this += std::format("{:.7f}", dValue);
		break;
	case 8:
		*this += std::format("{:.8f}", dValue);
		break;
	case 9:
		*this += std::format("{:.9f}", dValue);
		break;
	default:
		*this += std::format("{:.2f}", dValue);
	}
}


void DCString::appendFloat(float fValue, unsigned int uiNumDecimalPoints)
{
	switch (uiNumDecimalPoints)
	{
	case 0:
		*this += std::format("{:.0f}", fValue);
		break;
	case 1:
		*this += std::format("{:.1f}", fValue);
		break;
	case 2:
		*this += std::format("{:.2f}", fValue);
		break;
	case 3:
		*this += std::format("{:.3f}", fValue);
		break;
	case 4:
		*this += std::format("{:.4f}", fValue);
		break;
	case 5:
		*this += std::format("{:.5f}", fValue);
		break;
	case 6:
		*this += std::format("{:.6f}", fValue);
		break;
	case 7:
		*this += std::format("{:.7f}", fValue);
		break;
	case 8:
		*this += std::format("{:.8f}", fValue);
		break;
	case 9:
		*this += std::format("{:.9f}", fValue);
		break;
	default:
		*this += std::format("{:.2f}", fValue);
	}
}

void DCString::appendInt(int iInt)
{
	*this += std::to_string(iInt);
}

void DCString::appendUnsignedInt(unsigned int uiInt)
{
	*this += std::to_string(uiInt);
}
