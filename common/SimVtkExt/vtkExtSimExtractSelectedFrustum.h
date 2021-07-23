#pragma once
#include "stdafx.h"
#include "export.h"
#include <vtkExtractSelectedFrustum.h>

namespace SIM
{

	class SIMVTKEXT_EXPORT vtkExtSimExtractSelectedFrustum : public vtkExtractSelectedFrustum
	{
	public:
		static vtkExtSimExtractSelectedFrustum* New();

		vtkTypeMacro(vtkExtSimExtractSelectedFrustum, vtkExtractSelectedFrustum)

		vtkSetMacro(RespectBoundCells, int);
		vtkGetMacro(RespectBoundCells, int);
		vtkBooleanMacro(RespectBoundCells, int);

		void SetCellCache(std::map<vtkIdType, double*>* cache) { _cellCache = cache; }
	protected:
		vtkExtSimExtractSelectedFrustum();
		virtual ~vtkExtSimExtractSelectedFrustum() {}
		virtual int RequestData(vtkInformation *,
			vtkInformationVector **, vtkInformationVector *) override;

		int BoxFrustumIsect(double bounds[], vtkCell *cell);
	
	private:
		// Not implemented.
		vtkExtSimExtractSelectedFrustum(const vtkExtSimExtractSelectedFrustum&) {}
		void operator = (const vtkExtSimExtractSelectedFrustum&) {}

		int RespectBoundCells;
		std::map<vtkIdType, double*>* _cellCache;
	};


}


