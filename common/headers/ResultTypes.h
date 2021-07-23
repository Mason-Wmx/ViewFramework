#pragma once

namespace SIM
{
	enum class EResultType : unsigned short
	{
		RES_REYNOLDS_NUMBER,
		RES_VELOCITY,
		RES_FLOW_RATE,
		RES_MASS_FLOW_RATE,
		RES_TEMPERATURE,
		RES_PRESSURE,
		RES_ORIENTATION,

		RES_NODE_REYNOLDS_NUMBER,
		RES_NODE_VELOCITY,
		RES_NODE_FLOW_RATE,
		RES_NODE_MASS_FLOW_RATE,
		RES_NODE_ORIENTATION,

		RES_UNDEFINED
	};

	enum class ECalculationResult : unsigned short
	{
		CALC_UNDEFINED,
		CALC_SUCCEEDED,
		CALC_FAILED
	};

	enum class EResultState : unsigned short
	{
		STATE_UNDEFINED,
		STATE_UP_TO_DATE,
		STATE_OUT_OF_DATE
	};
}