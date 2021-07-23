#pragma once
#include "stdafx.h"
#include "export.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkProp3DCollection.h"
#include <vtkUnsignedCharArray.h>
#include <vtkDoubleArray.h>

namespace SIM
{
	struct IdIndexPair
	{
		ObjectId Id;
		int IndexStart;
		int IndexEnd;

		IdIndexPair(ObjectId id, int indexStart, int indexEnd) :Id(id), IndexStart(indexStart), IndexEnd(indexEnd)
		{
		}

		IdIndexPair()
		{
      Id = 0;
      IndexStart = 0;
      IndexEnd = 0;

		}

		bool operator < (const IdIndexPair& rhs) const
		{
			return Id < rhs.Id;
		}

		bool operator < (const ObjectId& rhs) const
		{
			return Id < rhs;
		}
	};

	class ViewActorIterator
	{
	public:
		ViewActorIterator(vtkProp* prop = nullptr) :_prop(prop)
		{
		}

		void SetVtkProp(vtkProp* prop)
		{
			_prop = prop;
		}

		int GetCount();

		vtkProp* GetItem(int i);

		vtkActor* GetItemAsActor(int i)
		{
			return vtkActor::SafeDownCast(GetItem(i));
		}

		void ForEachItem(std::function<void(vtkProp*)> func);

		void ForEachVtkActor(std::function<void(vtkActor&, int)> func);

		int GetIndex(vtkProp* actor);
	private:
		vtkProp* _prop;
	};


	class NFDCAPPCORE_EXPORT ViewActor
	{
	public:
		ViewActor(void);
		virtual ~ViewActor(void);

		vtkSmartPointer<vtkProp>& GetActor() { return _actor; }
		void SetActor(vtkSmartPointer<vtkProp> val);

		std::string GetType() const { return _type; }
		void SetType(std::string val) { _type = val; }

		void AddObject(ObjectId obj) { _objects.push_back(obj); }
		std::vector<ObjectId> & GetObjects() { return _objects; }

		void Clear();

      
		void SetCells(ObjectId obj, int startIndex, int endIndex, int actorIndex = 0, std::string context = "");
		ObjectId GetObjectOfCell(int cellIndex, int actorIndex = 0, std::string context = "");
		void Allocate(size_t count, int actorIndex = 0, std::string context = "");
		bool GetCells(ObjectId obj, int& startIndex, int& endIndex, int actorIndex = 0, std::string context = "");
 
		void SetVertices(ObjectId obj, int startIndex, int endIndex, int actorIndex = 0);
		bool GetVertices(ObjectId obj, int& startIndex, int& endIndex, int actorIndex = 0);

		bool GetAddedToView() const { return _addedToView; }
		void SetAddedToView(bool val) { _addedToView = val; }

		void SortVertices(int actorIndex = 0);

		int GetId() const { return _id; }

		void RemoveObject(ObjectId obj);
		void RemoveCells(ObjectId obj, int actorIndex = 0, std::string context = "");
		void ClearCellsAndVertices();
		virtual void ClearBeforeRebuild();
		std::vector<int>* GetOrderedCells(int actorIndex, std::string context);
		std::vector<ObjectId>* GetOrderedObjects(int actorIndex, std::string context);

		ViewActorIterator& GetActorIterator() { return _iterator; }

		void SetCellBound(vtkIdType cellId, double* bound, int actorIndex = 0);
		std::map<vtkIdType, double*>& GetCellCache(int actorIndex) { return _cellBoundCache[actorIndex]; }

		void SetRenderer(int id) { _renderer = id; }
		int GetRenderer() { return _renderer; }

        vtkSmartPointer<vtkAbstractCellLocator> GetLocator(vtkIdType index) { return _locator[index]; }
        void SetLocator(vtkIdType index,vtkSmartPointer<vtkAbstractCellLocator> locator) { _locator[index] = locator; }
	private:
		std::string _type;
		vtkSmartPointer<vtkProp> _actor;
		std::map<int,std::map<std::string, std::vector<int>>> _cells;
		std::map<int,std::map<std::string, std::vector<ObjectId>>> _cellObjects;
		std::vector<ObjectId> _objects;
		std::map<int,std::vector<IdIndexPair>> _vertices;
		std::map<int, std::map<vtkIdType, double*>> _cellBoundCache;
		bool _addedToView;
		bool _pickable;
		int _id;
		static int _nextId;
		ViewActorIterator _iterator;
		int _renderer = 0;
        std::map<vtkIdType,vtkSmartPointer<vtkAbstractCellLocator>> _locator;
	};


}

