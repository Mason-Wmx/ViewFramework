#include "stdafx.h"
#include "ModelEvents.h"
#include "DocModel.h"

using namespace SIM;

SIM::DocModelEvent::DocModelEvent(DocModel& model) :Event(model)
{

}

DocModel& SIM::DocModelEvent::GetModel() const
{
	return static_cast<DocModel&>(GetSource());
}

HubProjectRefreshedEvent::HubProjectRefreshedEvent(Observable & source)
  : Event(source)
{
}

SIM::JobStatusChangedEvent::JobStatusChangedEvent(Observable & source,
                                                  const ObjectId& idPD,
                                                  SIM::CloudJobStatus eStatus)
    : Event(source), _idProblemDefinition(idPD), _eCloudJobStatus(eStatus)
{

}

SIM::ObjectId SIM::JobStatusChangedEvent::GetProblemDefinition() const
{
    return _idProblemDefinition;
}

SIM::CloudJobStatus SIM::JobStatusChangedEvent::GetJobStauts() const
{
    return _eCloudJobStatus;
}
JobEvent::JobEvent(Observable & source, const std::string& jobId)
  : Event(source)
  , _jobId(jobId)
{
}

const std::string& JobEvent::GetJobId() const
{
  return _jobId;
}

JobSubmittedEvent::JobSubmittedEvent(Observable & source, const std::string & jobId)
  : JobEvent(source, jobId)
{
}

JobCancelledEvent::JobCancelledEvent(Observable & source, const std::string & jobId)
  : JobEvent(source, jobId)
{
}
