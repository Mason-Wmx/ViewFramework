/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkFFMPEGVideoSource.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkFFMPEGVideoSource
 * @brief   Reader for ffmpeg supported formats
 *
 * Note this this class make use of multiple threads when decoding files. It has
 * a feed thread, a video drain thread, and an audio drain thread. The decoding
 * may use multiple threads as well as specified by DecodingThreads ivar.
 *
 * @sa
 * vtkVideoSource
*/

#ifndef vtkFFMPEGVideoSource_h
#define vtkFFMPEGVideoSource_h

#include "vtkIOFFMPEGModule.h" // For export macro
#include "vtkVideoSource.h"
#include "vtkMultiThreader.h" // for ivar
#include "vtkNew.h" // for ivar
#include <functional> // for audio callback

class vtkFFMPEGVideoSourceInternal;

class vtkConditionVariable;
class vtkMutexLock;
class vtkFFMPEGVideoSource;

// audio callback struct, outside the class so that we
// can forward ref it
struct vtkFFMPEGVideoSourceAudioCallbackData
{
  int NumberOfSamples;
  int BytesPerSample;
  int NumberOfChannels;
  int SampleRate;
  int DataType;
  bool Packed;
  unsigned char** Data;
  vtkFFMPEGVideoSource *Caller;
  void *ClientData;
};

class VTKIOFFMPEG_EXPORT vtkFFMPEGVideoSource : public vtkVideoSource
{
public:
  static vtkFFMPEGVideoSource *New();
  vtkTypeMacro(vtkFFMPEGVideoSource,vtkVideoSource);

  /**
   * Standard VCR functionality: Record incoming video.
   */
  void Record() override;

  /**
   * Standard VCR functionality: Play recorded video.
   */
  void Play() override;

  /**
   * Standard VCR functionality: Stop recording or playing.
   */
  void Stop() override;

  /**
   * Grab a single video frame.
   */
  void Grab() override;

  //@{
  /**
   * Request a particular frame size (set the third value to 1).
   */
  void SetFrameSize(int x, int y, int z) override;
  void SetFrameSize(int dim[3]) override {
    this->SetFrameSize(dim[0], dim[1], dim[2]); };
  //@}

  /**
   * Request a particular frame rate (default 30 frames per second).
   */
  void SetFrameRate(float rate) override;

  /**
   * Request a particular output format (default: VTK_RGB).
   */
  void SetOutputFormat(int format) override;

  /**
   * Initialize the driver (this is called automatically when the
   * first grab is done).
   */
  void Initialize() override;

  /**
   * Free the driver (this is called automatically inside the
   * destructor).
   */
  void ReleaseSystemResources() override;

  //@{
  /**
   * Specify file name of the video
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  /**
   * The internal function which actually does the grab.  You will
   * definitely want to override this if you develop a vtkVideoSource
   * subclass.
   */
  void InternalGrab() override;

  // is the video at the end of file?
  // Usefull for while loops
  vtkGetMacro(EndOfFile,bool);

  // we do not use Invoke Observers here because this callback
  // will happen in a different thread that could conflict
  // with events from other threads. In this function you should
  // not block the thread (for example waiting for audio to play)
  // instead you should have enough buffering that you can consume
  // the provided data and return. Typically even 1 second of
  // buffer storage is enough to prevent blocking.
  typedef std::function<void(vtkFFMPEGVideoSourceAudioCallbackData &data)> AudioCallbackType;
  void SetAudioCallback(AudioCallbackType cb, void *clientData)
  {
    this->AudioCallback = cb;
    this->AudioCallbackClientData = clientData;
  }

  //@{
  /**
   * How many threads to use for the decoding codec
   * this will be in addition to the feed and drain threads.
   * the default value is 4.
   */
  vtkSetMacro(DecodingThreads, int);
  vtkGetMacro(DecodingThreads, int);
  //@}

protected:
  vtkFFMPEGVideoSource();
  ~vtkFFMPEGVideoSource();

  AudioCallbackType AudioCallback;
  void *AudioCallbackClientData;

  int DecodingThreads;

  static void *DrainAudioThread(
    vtkMultiThreader::ThreadInfo *data);
  void *DrainAudio(vtkMultiThreader::ThreadInfo *data);
  int DrainAudioThreadId;

  static void *DrainThread(
    vtkMultiThreader::ThreadInfo *data);
  void *Drain(vtkMultiThreader::ThreadInfo *data);
  int DrainThreadId;

  bool EndOfFile;

  vtkNew<vtkConditionVariable> FeedCondition;
  vtkNew<vtkMutexLock> FeedMutex;
  vtkNew<vtkConditionVariable> FeedAudioCondition;
  vtkNew<vtkMutexLock> FeedAudioMutex;
  static void *FeedThread(
    vtkMultiThreader::ThreadInfo *data);
  void *Feed(vtkMultiThreader::ThreadInfo *data);
  int FeedThreadId;

  char *FileName;

  vtkFFMPEGVideoSourceInternal *Internal;

  void ReadFrame();

private:
  vtkFFMPEGVideoSource(const vtkFFMPEGVideoSource&) = delete;
  void operator=(const vtkFFMPEGVideoSource&) = delete;
};

#endif
