#pragma once
#include "stdafx.h"
#include "DummyObject.h"
#include "../NfdcAppCore/Request.h"

namespace SIM
{
	class DummyRequest: public Request
	{
	public:
		DummyRequest(Document& document,double x, double y,double z, double diameter,double height, ObjectId parent = NullObjectKey);
		~DummyRequest(void);

		static std::shared_ptr<DummyRequest> Create(Document& document,double x, double y,double z, double diameter,double height, ObjectId parent = NullObjectKey);

		virtual bool Execute();

		virtual bool Undo();
	private:
		double _x;
		double _y;
		double _z;
		double _diameter;
		double _height;
		ObjectId _parent;
		std::shared_ptr<DummyObject> _obj;
	};

	class DummyEditRequest : public Request
	{
	public:
		DummyEditRequest(Document& document, std::shared_ptr<Object> obj, double x, double y, double z, double diameter, double height);
		~DummyEditRequest(void);
						
		virtual bool Execute();

		virtual bool Undo();
	private:
		double _x;
		double _y;
		double _z;
		double _diameter;
		double _height;

		double _prevX;
		double _prevY;
		double _prevZ;
		double _prevDiameter;
		double _prevHeight;

		std::shared_ptr<Object> _obj;
	};

	class DummyGroupableRequest : public Request
	{
	public:
		DummyGroupableRequest(Document& document, double x, double y, double z, double diameter, double height, ObjectId parent = NullObjectKey);
		~DummyGroupableRequest(void);

		static std::shared_ptr<DummyGroupableRequest> Create(Document& document, double x, double y, double z, double diameter, double height, ObjectId parent = NullObjectKey);

		virtual bool Execute();

		virtual bool Undo();
	private:
		double _x;
		double _y;
		double _z;
		double _diameter;
		double _height;
		ObjectId _parent;
		std::shared_ptr<DummyGroupableObject> _obj;
		std::shared_ptr<DummyObjectsGroup> _grp;
	};

}

