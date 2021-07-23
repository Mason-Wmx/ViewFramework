
#include "stdafx.h"
#include "ViewActor.h"
using namespace SIM;

ViewActor::ViewActor(void) :
	_addedToView(false), _pickable(true)
{
	_id = _nextId++;
}

ViewActor::~ViewActor(void)
{
	for (auto itAct : _cellBoundCache)
	{
		for (auto itCell : itAct.second)
		{
			delete itCell.second;
		}
	}
}

inline void SIM::ViewActor::SetActor(vtkSmartPointer<vtkProp> val) 
{ 
	_actor = val; 
	_iterator.SetVtkProp(val);
}

void SIM::ViewActor::SetCells(ObjectId obj, int startIndex, int endIndex, int actorIndex, std::string context)
{
	_cells[actorIndex][context].push_back(startIndex);
	_cells[actorIndex][context].push_back(endIndex);
	_cellObjects[actorIndex][context].push_back(obj);
}

bool SIM::ViewActor::GetCells(ObjectId obj, int& startIndex, int& endIndex, int actorIndex, std::string context)
{

	if (_cellObjects.find(actorIndex) != _cellObjects.end() && _cellObjects[actorIndex].find(context) != _cellObjects[actorIndex].end())
	{		
		int pos = std::find(_cellObjects[actorIndex][context].begin(), _cellObjects[actorIndex][context].end(), obj) - _cellObjects[actorIndex][context].begin();

		if (pos < _cellObjects[actorIndex][context].size())
		{
			startIndex = _cells[actorIndex][context][pos * 2];
			endIndex = _cells[actorIndex][context][pos * 2 + 1];
			return true;
		}
	}
	return false;
}

ObjectId SIM::ViewActor::GetObjectOfCell(int cellIndex, int actorIndex, std::string context)
{
	if (_cellObjects.find(actorIndex) != _cellObjects.end() && _cellObjects[actorIndex].find(context) != _cellObjects[actorIndex].end())
	{
		auto it = std::lower_bound(_cells[actorIndex][context].begin(), _cells[actorIndex][context].end(), cellIndex);

		if (it != _cells[actorIndex][context].end())
		{
			int index = it - _cells[actorIndex][context].begin();
			index = index / 2;
			return _cellObjects[actorIndex][context][index];
		}
	}
	
	return NullObjectKey;
}

void SIM::ViewActor::Clear()
{
	_objects.clear();
	_cells.clear();
}

void SIM::ViewActor::Allocate(size_t count, int actorIndex, std::string context)
{
	_objects.reserve(count);
	_cellObjects[actorIndex][context].reserve(count);
	_cells[actorIndex][context].reserve(count * 2);
}

void SIM::ViewActor::SetVertices(ObjectId obj, int startIndex, int endIndex, int actorIndex)
{
	_vertices[actorIndex].push_back(IdIndexPair(obj, startIndex, endIndex));

}

bool SIM::ViewActor::GetVertices(ObjectId obj, int& startIndex, int& endIndex, int actorIndex)
{
	auto it = std::lower_bound(_vertices[actorIndex].begin(), _vertices[actorIndex].end(), obj);
	if (it != _vertices[actorIndex].end())
	{
		if (it->Id == obj)
		{
			startIndex = it->IndexStart;
			endIndex = it->IndexEnd;
			return true;
		}
	}

	return false;
}

void SIM::ViewActor::SortVertices(int actorIndex)
{
	std::sort(_vertices[actorIndex].begin(), _vertices[actorIndex].end());
}

void SIM::ViewActor::RemoveObject(ObjectId obj)
{
	_objects.erase(std::remove(_objects.begin(), _objects.end(), obj), _objects.end());
}

void SIM::ViewActor::RemoveCells(ObjectId obj, int actorIndex, std::string context)
{
	if (_cellObjects.find(actorIndex) != _cellObjects.end() && _cellObjects[actorIndex].find(context) != _cellObjects[actorIndex].end())
	{
		auto& cellObjects = _cellObjects[actorIndex][context];
		auto& cells = _cells[actorIndex][context];
		int pos = std::find(cellObjects.begin(), cellObjects.end(), obj) - cellObjects.begin();

		if (pos < cellObjects.size())
		{
			int startCell = cells[pos * 2];
			int endCell = cells[pos * 2 + 1];
			int dc = endCell - startCell + 1;

			cells.erase(cells.begin() + pos * 2, cells.begin() + pos * 2 + 2);

			for (auto it = cells.begin() + pos * 2; it != cells.end(); it++)
			{
				*it = *it - dc;
			}

			cellObjects.erase(std::remove(cellObjects.begin(), cellObjects.end(), obj), cellObjects.end());
		}
	}
}

void SIM::ViewActor::ClearCellsAndVertices()
{
	_cellObjects.clear();
	_cells.clear();
	_vertices.clear();
}

void SIM::ViewActor::ClearBeforeRebuild()
{
	ClearCellsAndVertices();
}

std::vector<int>* SIM::ViewActor::GetOrderedCells(int actorIndex, std::string context)
{
	if (_cells[actorIndex].find(context) != _cells[actorIndex].end())
		return &_cells[actorIndex][context];

	if (context != "")
		return GetOrderedCells(actorIndex,"");

	return nullptr;
}

std::vector<ObjectId>* SIM::ViewActor::GetOrderedObjects(int actorIndex, std::string context)
{
	if (_cellObjects[actorIndex].find(context) != _cellObjects[actorIndex].end())
		return &_cellObjects[actorIndex][context];

	if (context != "")
		return GetOrderedObjects(actorIndex, "");

	return nullptr;
}

void SIM::ViewActor::SetCellBound(vtkIdType cellId, double * bound, int actorIndex)
{
	if (_cellBoundCache[actorIndex].find(cellId) == _cellBoundCache[actorIndex].end())
		_cellBoundCache[actorIndex][cellId] = new double[6];

	for(int i=0; i<6; i++)
		_cellBoundCache[actorIndex][cellId][i] = bound[i];
}

int SIM::ViewActor::_nextId(1);

int SIM::ViewActorIterator::GetCount() {
	if (!_prop)
		return 0;

	auto assembly = vtkAssembly::SafeDownCast(_prop);

	if (assembly)
		return assembly->GetParts()->GetNumberOfItems();

	return 1;
}

vtkProp * SIM::ViewActorIterator::GetItem(int i)
{
	if (!_prop)
		return nullptr;

	auto assembly = vtkAssembly::SafeDownCast(_prop);
	if (assembly)
		return vtkProp::SafeDownCast(assembly->GetParts()->GetItemAsObject(i));

	return _prop;
}

void SIM::ViewActorIterator::ForEachItem(std::function<void(vtkProp*)> func)
{
	if (!_prop)
		return;

	auto assembly = vtkAssembly::SafeDownCast(_prop);
	if (assembly)
	{
		vtkPropCollection* props = assembly->GetParts();
		props->InitTraversal();
		for (int i = 0; i < props->GetNumberOfItems(); i++)
		{
			func(props->GetNextProp());
		}
		return;
	}

	func(_prop);
}

void SIM::ViewActorIterator::ForEachVtkActor(std::function<void(vtkActor&, int)> func)
{
	if (!_prop)
		return;

	auto assembly = vtkAssembly::SafeDownCast(_prop);
	if (assembly)
	{
		vtkPropCollection* props = assembly->GetParts();
		props->InitTraversal();
		for (int i = 0; i < props->GetNumberOfItems(); i++)
		{
			vtkActor* actor = vtkActor::SafeDownCast(props->GetNextProp());
			if (actor)
				func(*actor, i);
		}
		return;
	}

	vtkActor* actor = vtkActor::SafeDownCast(_prop);

	if (actor)
		func(*actor, 0);
}

int SIM::ViewActorIterator::GetIndex(vtkProp * actor)
{
	if (!_prop)
		return -1;

	if (_prop == actor)
		return 0;

	auto assembly = vtkAssembly::SafeDownCast(_prop);
	if (assembly)
	{
		vtkPropCollection* props = assembly->GetParts();
		props->InitTraversal();
		for (int i = 0; i < props->GetNumberOfItems(); i++)
		{
			if (props->GetNextProp() == actor)
				return i;
		}
	}

	return 0;
}
