#pragma once
#include "stdafx.h"
#include "../NfdcDataModel/Object.h"

namespace SIM
{
	class DummyObject:public Object
	{
	public:
		DummyObject(double x, double y,double z, double diameter,double height, ObjectId parent = NullObjectKey);
		DummyObject(const DummyObject& pattarn);
		~DummyObject(void);

		double GetX() const { return _x; }
		void SetX(double val) { _x = val; }
		
		double GetY() const { return _y; }
		void SetY(double val) { _y = val; }
		
		double GetZ() const { return _z; }
		void SetZ(double val) { _z = val; }

		double GetDiameter() const { return _diameter; }
		void SetDiameter(double val) { _diameter = val; }

		double GetHeight() const { return _height; }
		void SetHeight(double val) { _height = val; }

		std::string GetName() const override { return typeid(DummyObject).name(); }

		int renderer = 0;
	protected:
		double _x;
		double _y;
		double _z;
		double _diameter;
		double _height;
	};



	class DummyObjectGlyph :public Object
	{
	public:
		DummyObjectGlyph(double x, double y, double z, double height, ObjectId parent = NullObjectKey);
		~DummyObjectGlyph(void);

		double GetX() const { return _x; }
		void SetX(double val) { _x = val; }

		double GetY() const { return _y; }
		void SetY(double val) { _y = val; }

		double GetZ() const { return _z; }
		void SetZ(double val) { _z = val; }

		double GetHeight() const { return _height; }
		void SetHeight(double val) { _height = val; }

	private:
		double _x;
		double _y;
		double _z;
		double _height;
	};


	class DummyGroupableObject : public DummyObject
	{
	public:
		DummyGroupableObject(double x, double y, double z, double diameter, double height, ObjectId parent = NullObjectKey);
		DummyGroupableObject(const DummyGroupableObject& pattern);
		~DummyGroupableObject(void);

		virtual void OnParentRemoved(
			ObjectId id, 
			Model& model, 
			std::unordered_set<ObjectId>& objectsToRemove, 
			std::unordered_set<ObjectId>& objectsToUpdate,
            std::vector<std::shared_ptr<ObjectModification>>& modifications) override;
	};


	class DummyObjectsGroup : public Object
	{
	public:
		DummyObjectsGroup(double x, double y, double z, double height, ObjectId parent = NullObjectKey);
		DummyObjectsGroup(const DummyObjectsGroup & pattern);
		~DummyObjectsGroup(void);

		double GetX() const { return _x; }
		void SetX(double val) { _x = val; }

		double GetY() const { return _y; }
		void SetY(double val) { _y = val; }

		double GetZ() const { return _z; }
		void SetZ(double val) { _z = val; }

		double GetHeight() const { return _height; }
		void SetHeight(double val) { _height = val; }

		static const std::string Name;

		virtual void OnChildRemoved(
			ObjectId id, 
			Model& model, 
			std::unordered_set<ObjectId>& objectsToRemove, 
			std::unordered_set<ObjectId>& objectsToUpdate,
            std::vector<std::shared_ptr<ObjectModification>>& modifications) override;

	protected:
		double _x;
		double _y;
		double _z;
		double _height;
	};
}
